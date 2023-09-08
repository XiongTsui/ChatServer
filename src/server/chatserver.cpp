#include "chatserver.hpp"
#include "json.hpp"
#include <functional>
#include <string>
#include "chatservice.hpp"

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) 
                       : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 给服务器注册用户连接的创建和 断开回调
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

    // 给服务器注册用户读写事件回调
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置服务器端的线程个数  1个io 3个worker
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 连接 或者 断开的回调
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开链接
    if (!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

//读写事件的回调
void  ChatServer::onMessage(const TcpConnectionPtr &conn,
                Buffer *buffer,
                Timestamp time)
{ 
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);

    //目的：完全解耦网络模块的代码和业务模块的代码
    //js["msgid"] -->  业务handler --》 conn  js  time
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //回调消息绑定好的事务处理器
    msgHandler(conn , js , time);
}

