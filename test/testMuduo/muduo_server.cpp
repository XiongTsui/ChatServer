/**
 * muduo给用户提供了两个类
 * TcpServer : 用于编写服务器的
 * TcpClient : 用于编写客户端程序的
 *
 * 把网络I/O的代码和业务代码区分开来
 *              用户连接和断开   用户的可读写事件
 */

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/**
 * 基于muduo网络库  开发服务器程序
 * 1、组合TcpServer对象
 * 2、创建EventLoop事件循环的指针
 * 3、chatServer得分构造函数
 * 4、注册处理链接的回调函数和处理读写事件的回调函数
 * 5、线程数量
 */
class ChatServer
{
public:
    ChatServer(EventLoop *loop,               // 事件循环
               const InetAddress &listenAddr, // IP + Port
               const string &nameArg)
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 给服务器注册用户连接的创建和 断开回调
        _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

        //设置服务器端的线程个数  1个io 3个worker
        _server.setThreadNum(4);
    }

    void start(){
        _server.start();
    }

private:
    // 专门处理用户连接的创建 和 断开
    void onConnection(const TcpConnectionPtr &conn)
    {
        if(conn->connected()){
            cout << conn->peerAddress().toIpPort()<< "-->" 
            << conn->localAddress().toIpPort() << "state:online" <<endl;
        }else{
            cout << conn->peerAddress().toIpPort()<< "-->" 
            << conn->localAddress().toIpPort() << "state:offline" <<endl;
            //_loop->quit();
            conn->shutdown();//close(fd)
        }

    };

    // 专门处理用户的读写事件的
    void onMessage(const TcpConnectionPtr &conn,//连接
                   Buffer *buffer,//缓冲区
                   Timestamp time)//接受的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << " time:" << time.toString() << endl;
        conn->send(buf + "hello \n");
    };

    TcpServer _server;
    EventLoop *_loop;
};

int main() {
    EventLoop loop;
    InetAddress addr("127.0.0.1" , 6000);
    ChatServer server(&loop , addr , "ChatServer");

    server.start();
    loop.loop();//epoll_wait 以阻塞的方式等待新客户的连接 ， 已连接用户的读写事件等等
    return 0;
}