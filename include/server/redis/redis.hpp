#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

/*
redis作为集群服务器通信的基于发布-订阅消息队列时，会遇到两个难搞的bug问题：
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611
*/
class Redis{
public:
    Redis();
    ~Redis();

    //连接redis服务器
    bool connect();

    //向redis指定的通道 发布消息
    bool publish(int channel , string message);

    //向redis指定的通道 订阅消息 
    bool subscribe(int channel);

    //向redis 指定channel 取消订阅
    bool unsubscribe(int channel);

    //在独立线程中去接受 订阅的消息
    void observer_channel_message();

    //初始化 向业务层上报通道消息的回调对象
    void init_notify_handler(function<void(int, string)> fn);

private:
    //发布用的上下文：hiredis的同步上下文对象
    redisContext *_publish_context;

    //接受用的上下文：hiredis的同步上下文对象
    redisContext *_subcribe_context;

    //回调操作，接受订阅的信息，返回给service
    function<void(int , string)> _notify_message_handler;
};

#endif
