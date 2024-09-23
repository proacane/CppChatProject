/**
 *  FileName: ChatServiceImpl.cpp
 *  CreateTime: 2024/9/7 15:22
 *  Description: 
 *  Author: ACAね
*/
#include "../include/ChatServiceImpl.h"
#include "../include/UserMgr.h"
#include "../include/CSession.h"
#include "../include/RedisMgr.h"
#include"../include/MysqlMgr.h"

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

bool ChatServiceImpl::getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> userInfo) {
    // 先在 redis 中查，查不到去数据库查，数据库查到了就写进redis
    std::string info_str = "";
    bool b_base = RedisMgr::getInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userInfo->uid = root["uid"].asInt();
        userInfo->name = root["name"].asString();
        userInfo->pwd = root["pwd"].asString();
        userInfo->email = root["email"].asString();
        userInfo->nick = root["nick"].asString();
        userInfo->desc = root["desc"].asString();
        userInfo->gender = root["gender"].asInt();
        userInfo->avatar = root["avatar"].asString();
        spdlog::info("user login uid is {}, name is {}, email is {}", userInfo->uid, userInfo->name, userInfo->email);
    } else {
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MysqlMgr::getInstance()->getUser(uid);
        if (user_info == nullptr) {
            return false;
        }
        userInfo = user_info;
        // 写入 redis
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["pwd"] = userInfo->pwd;
        redis_root["name"] = userInfo->name;
        redis_root["email"] = userInfo->email;
        redis_root["nick"] = userInfo->nick;
        redis_root["desc"] = userInfo->desc;
        redis_root["gender"] = userInfo->gender;
        redis_root["avatar"] = userInfo->avatar;
        RedisMgr::getInstance()->set(base_key, redis_root.toStyledString());
    }
    return true;
}

Status ChatServiceImpl::NotifyAuthFriend(::grpc::ServerContext *context, const ::message::AuthFriendReq *request,
                                         ::message::AuthFriendRsp *reply) {
    //查找用户是否在本服务器
    auto touid = request->touid();
    auto fromuid = request->fromuid();
    auto session = UserMgr::getInstance()->getSession(touid);
    Defer defer([request, reply]() {
        reply->set_error(ErrorCodes::Success);
        reply->set_fromuid(request->fromuid());
        reply->set_touid(request->touid());
    });
    //用户不在内存中则直接返回
    if (session == nullptr) {
        spdlog::warn("can't find server");
        return Status::OK;
    }
    //在内存中则直接发送通知对方
    Json::Value rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["fromuid"] = request->fromuid();
    rtvalue["touid"] = request->touid();
    std::string base_key = USER_BASE_INFO + std::to_string(fromuid);
    auto user_info = std::make_shared<UserInfo>();
    bool b_info = getBaseInfo(base_key, fromuid, user_info);
    if (b_info) {
        rtvalue["name"] = user_info->name;
        rtvalue["nick"] = user_info->nick;
        rtvalue["avatar"] = user_info->avatar;
        rtvalue["gender"] = user_info->gender;
    } else {
        rtvalue["error"] = ErrorCodes::UidInvalid;
    }
    std::string return_str = rtvalue.toStyledString();
    session->send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(::grpc::ServerContext *context, const ::message::TextChatMsgReq *request,
                                          ::message::TextChatMsgRsp *reply) {
    //查找用户是否在本服务器
    auto touid = request->touid();
    auto session = UserMgr::getInstance()->getSession(touid);
    reply->set_error(ErrorCodes::Success);
    //用户不在内存中则直接返回
    if (session == nullptr) {
        return Status::OK;
    }
    //在内存中则直接发送通知对方
    Json::Value  rtvalue;
    rtvalue["error"] = ErrorCodes::Success;
    rtvalue["fromuid"] = request->fromuid();
    rtvalue["touid"] = request->touid();
    //将聊天数据组织为数组
    Json::Value text_array;
    for (auto& msg : request->textmsgs()) {
        Json::Value element;
        element["content"] = msg.msgcontent();
        element["msgid"] = msg.msgid();
        text_array.append(element);
    }
    rtvalue["text_array"] = text_array;
    std::string return_str = rtvalue.toStyledString();
    session->send(return_str, ID_NOTIFY_TEXT_CHAT_MSG_REQ);
    return Status::OK;
}

ChatServiceImpl::ChatServiceImpl() {};
