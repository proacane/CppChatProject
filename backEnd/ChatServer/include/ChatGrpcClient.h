/**
 *  FileName: ChatGrpcClient.h
 *  CreateTime: 2024/9/7 13:43
 *  Description: chatServer 之间进行通信的客户端
 *  Author: ACAね
*/
#ifndef CHATSERVER_CHATGRPCCLIENT_H
#define CHATSERVER_CHATGRPCCLIENT_H

#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h>
#include "../proto/message.grpc.pb.h"
#include "../proto/message.pb.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <queue>
#include "SingleTon.h"
#include "data.h"
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;
using message::AuthFriendReq;
using message::AuthFriendRsp;
using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConnectionPool {
public:
    ChatConnectionPool(size_t pool_size, std::string host, std::string port);

    ~ChatConnectionPool();

    std::unique_ptr<ChatService::Stub> getConnection();

    void returnConnection(std::unique_ptr<ChatService::Stub>);

    void close();

private:
    size_t _pool_size;
    // 是否关闭该池子
    std::atomic<bool> _b_stop;
    std::string _host;
    std::string _port;
    // 保存连接
    std::queue<std::unique_ptr<ChatService::Stub>> _connections;
    std::mutex _mutex;
    std::condition_variable _cond;
};

class ChatGrpcClient : public SingleTon<ChatGrpcClient> {
    friend class SingleTon<ChatGrpcClient>;
public:
    ChatGrpcClient(const ChatGrpcClient&) = delete;
    ChatGrpcClient& operator=(ChatGrpcClient&) = delete;
    // 通知对方添加好友
    AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);
    // 通知对方我是否同意好友添加
    AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
    // 获取用户信息
    bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    // 传递文本信息
    TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);
private:
    ChatGrpcClient();
    // k:server 名字
    std::unordered_map<std::string,std::unique_ptr<ChatConnectionPool>> _pools;
};

#endif //CHATSERVER_CHATGRPCCLIENT_H
