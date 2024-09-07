/**
 *  FileName: ChatServiceImpl.cpp
 *  CreateTime: 2024/9/7 15:22
 *  Description: 
 *  Author: ACAね
*/
#include "../include/ChatServiceImpl.h"

ChatServiceImpl::~ChatServiceImpl() = default;
// TODO 后续实现
Status ChatServiceImpl::NotifyAddFriend(::grpc::ServerContext *context, const ::message::AddFriendReq *request,
                                        ::message::AddFriendRsp *response) {
    return Service::NotifyAddFriend(context, request, response);
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

ChatServiceImpl::ChatServiceImpl() {} ;
