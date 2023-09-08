#include "redis.hpp"
#include <iostream>
using namespace std;

/**
 * void redisFree（）:将相应的redis客户端连接释放掉
 * rediscontext = redisConnect(addr , port) :使用端口号和地址 去连接redis-cli。
 * 
 * redisReply redisCommand（rediscontext , redis语句 ， 变量）:发送命令,变阻塞线程直到命令执行完成，奖结果返回给调用者
 * redisAppendCommend（rediscontext , redis语句 ， 变量）:发送命令，但是不会阻塞，就是将其添加到发送队列中
 * 
 * freeReplyObject(redisReply)
 * 
*/

Redis::Redis():_publish_context(nullptr) ,_subcribe_context(nullptr){
}

Redis::~Redis(){
    if(_publish_context) redisFree(_publish_context);
    if(_subcribe_context) redisFree(_subcribe_context);
}

bool Redis::connect(){
    //负责publish发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1" , 6379);
    if(nullptr == _publish_context){
        cerr << "connect redis failed!" <<endl;
        return false;
    }

    //负责subscribe订阅消息的上下文连接
    _subcribe_context = redisConnect("127.0.0.1", 6379);
    if(nullptr == _subcribe_context){
        cerr << "connect redis failed!" << endl;
        return false;
    }

    //在单独的线程中，监听通道上的事件，有消息就给业务进行上报
    thread t([&](){
        observer_channel_message();
    });
    t.detach();

    cout << "connect redis-server success!" <<endl;
    return true;
}

//向redis指定的通道channel 发布消息
bool Redis::publish(int channel , string message){
    redisReply *reply = (redisReply *)redisCommand(_publish_context , "PUBLISH %d %s",channel , message.c_str());
    if(nullptr == reply){
        cerr << "publish command failed!" <<endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel)
{
    //SUBSCRIBE命令本身会造成线程的阻塞等待，通道里面发生消息，该通道里面只做订阅通道，不接收通道消息
    //通道消息的接收专门创建一个线程，在observer_channel_message函数里面去做
    //只负责发送命令，不阻塞接受redis server响应消息，否者和notifys线程抢占响应资源
    if(REDIS_ERR == redisAppendCommand(this->_subcribe_context , "SUBSCRIBE %d" , channel)){
        cerr << "subscribe command failed!" << endl;
        return false;
    }
    //redisBufferWrite 可以循环发送缓冲区，直到缓冲器的数据发送完毕（done 被制为1）
    int done = 0 ;
    while(!done){
        if(REDIS_ERR == redisBufferWrite(this->_subcribe_context , &done)){
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

// 向redis指定的通道unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel){
    if(REDIS_ERR == redisAppendCommand(this->_subcribe_context , "UNSUBSCRIBE %d" , channel)){
        cerr<<"unsubscribe command failed" <<endl;
        return false;
    }
     // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done))
        {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

//在独立线程中接受订阅通道中的消息
void Redis::observer_channel_message(){
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(this->_subcribe_context , (void **)&reply)){
        //订阅收到的信息是一个三元的数组
        if(reply != nullptr&& reply->element[2] != nullptr && reply->element[2]->str!=nullptr){
            //给业务层报道发生的消息
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }

        freeReplyObject(reply);
    }
    cerr << ">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<" << endl;
}

void Redis::init_notify_handler(function<void(int,string)> fn)
{
    this->_notify_message_handler = fn;
}