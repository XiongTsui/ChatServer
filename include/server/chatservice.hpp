#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>


using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "redis.hpp"
#include "groupmodel.hpp"
#include "usermodel.hpp"
#include "json.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
using json = nlohmann::json;

//处理事件方法的回调类型  
using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js , Timestamp)>;

class ChatService{
public:
    //获取单例对象的接口函数
    static ChatService* instance();
    //登入业务
    void login(const TcpConnectionPtr &conn , json &js , Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr &conn , json &js , Timestamp time);
    //一对一聊天业务
    void onechat(const TcpConnectionPtr &conn , json &js , Timestamp time);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    //用户退出
    void clientCloseException(const TcpConnectionPtr &conn);
    //服务器异常，业务重置方法
    void reset();
    //处理退出账户任务
    void loginout(const TcpConnectionPtr &conn , json &js , Timestamp time);
    //单点聊天
    void oneChat(const TcpConnectionPtr &conn , json &js , Timestamp time);
    //加好友
    void addFriend(const TcpConnectionPtr &conn , json &js , Timestamp time);
     // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    //从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int userid , string msg);
    
private:
    ChatService();
    //存储消息id 和 其对应的业务处理方法
    unordered_map<int , MsgHandler> _msgHandlerMap;
    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    //数据操作类对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    //定义一个redis对象
    Redis _redis;
};

#endif