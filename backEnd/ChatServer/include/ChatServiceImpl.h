/**
 *  FileName: ChatServiceImpl.h
 *  CreateTime: 2024/9/7 15:22
 *  Description: ChatServer 之间通信的服务端
 *  Author: ACAね
*/
#ifndef CHATSERVER_CHATSERVICEIMPL_H
#define CHATSERVER_CHATSERVICEIMPL_H

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

class ChatServiceImpl final : public ChatService::Service {
public:
    ~ChatServiceImpl() override;
    ChatServiceImpl();
    Status NotifyAddFriend(::grpc::ServerContext *context, const ::message::AddFriendReq *request,
                           ::message::AddFriendRsp *response) override;

    Status RplyAddFriend(::grpc::ServerContext *context, const ::message::RplyFriendReq *request,
                         ::message::RplyFriendRsp *response) override;

    Status SendChatMsg(::grpc::ServerContext *context, const ::message::SendChatMsgReq *request,
                       ::message::SendChatMsgRsp *response) override;

    Status NotifyAuthFriend(::grpc::ServerContext *context, const ::message::AuthFriendReq *request,
                            ::message::AuthFriendRsp *response) override;

    Status NotifyTextChatMsg(::grpc::ServerContext *context, const ::message::TextChatMsgReq *request,
                             ::message::TextChatMsgRsp *response) override;

private:
    bool getBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo> userInfo);
};

#endif //CHATSERVER_CHATSERVICEIMPL_H
