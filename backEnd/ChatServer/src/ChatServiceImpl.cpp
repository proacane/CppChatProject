/**
 *  FileName: ChatServiceImpl.cpp
 *  CreateTime: 2024/9/7 15:22
 *  Description: 
 *  Author: ACAね
*/
#include "../include/ChatServiceImpl.h"
#include "../include/UserMgr.h"
#include "../include/CSession.h"

ChatServiceImpl::~ChatServiceImpl() = default;

// TODO 后续实现
Status ChatServiceImpl::NotifyAddFriend(::grpc::ServerContext *context, const ::message::AddFriendReq *request,
                                        ::message::AddFriendRsp *response) {
    // 查看该用户是否在本服务器
    auto to_uid = request->touid();
    auto session = UserMgr::getInstance()->getSession(to_uid);
    Defer defer([request, response]() {
        response->set_error(ErrorCodes::Success);
        response->set_applyuid(request->applyuid());
        response->set_touid(request->touid());
    });
    // 不在就直接返回
    if (session == nullptr) {
        spdlog::warn("can't find server");
        return Status::OK;
    }

    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    return_value["applyuid"] = request->applyuid();
    return_value["name"] = request->name();
    return_value["desc"] = request->desc();
    return_value["avatar"] = request->avatar();
    return_value["gender"] = request->gender();
    return_value["nick"] = request->nick();
    std::string return_str = return_value.toStyledString();
    // 发送给客户端
    session->send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
    return Status::OK;
}

Status ChatServiceImpl::RplyAddFriend(::grpc::ServerContext *context, const ::message::RplyFriendReq *request,
                                      ::message::RplyFriendRsp *response) {
    return Service::RplyAddFriend(context, request, response);
}

Status ChatServiceImpl::SendChatMsg(::grpc::ServerContext *context, const ::message::SendChatMsgReq *request,
                                    ::message::SendChatMsgRsp *response) {
    return Service::SendChatMsg(context, request, response);
}

Status ChatServiceImpl::NotifyAuthFriend(::grpc::ServerContext *context, const ::message::AuthFriendReq *request,
                                         ::message::AuthFriendRsp *response) {
    return Service::NotifyAuthFriend(context, request, response);
}

Status ChatServiceImpl::NotifyTextChatMsg(::grpc::ServerContext *context, const ::message::TextChatMsgReq *request,
                                          ::message::TextChatMsgRsp *response) {
    return Service::NotifyTextChatMsg(context, request, response);
}

ChatServiceImpl::ChatServiceImpl() {};
