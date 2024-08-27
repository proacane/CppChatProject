/**
 *  FileName: LogicSystem.cpp
 *  CreateTime: 2024/8/24 15:47
 *  Description: 
 *  Author: ACAね
*/
#include "../include/LogicSystem.h"
#include "../include/CSession.h"
#include <spdlog/spdlog.h>
#include <json/reader.h>
#include <json/value.h>
#include "../include/StatusGrpcClient.h"


LogicSystem::LogicSystem() : _b_stop(false) {
    registerCallBacks();
    // 逻辑线程进行消息处理
    _worker_thread = std::thread(&LogicSystem::dealMsg, this);
}

LogicSystem::~LogicSystem() {
    _b_stop = true;
    _consume.notify_one();
    _worker_thread.join();
}

void LogicSystem::postMsgQue(std::shared_ptr<LogicNode> msg) {
    std::unique_lock<std::mutex> lock(_mutex);
    _msg_que.push(msg);
    // 0变到1开启工作线程
    if (_msg_que.size() == 1) {
        lock.unlock();
        _consume.notify_one();
    }
}

void LogicSystem::dealMsg() {
    for (;;) {
        std::unique_lock<std::mutex> lock(_mutex);
        while (_msg_que.empty() && !_b_stop) {
            _consume.wait(lock);
        }
        // 要关闭了
        if (_b_stop) {
            while (!_msg_que.empty()) {
                auto msg_node = _msg_que.front();
                spdlog::info("LogicSystem received msg id is {}", msg_node->_recnode->getId());
                auto call_back_iter = _fun_callbacks.find(msg_node->_recnode->getId());
                if (call_back_iter == _fun_callbacks.end()) {
                    spdlog::warn("msg id is {}, has no handler", msg_node->_recnode->getId());
                    _msg_que.pop();
                    continue;
                }
                // 调用请求 id 对应的函数
                call_back_iter->second(msg_node->_session, msg_node->_recnode->getId(),
                                       std::string(msg_node->_recnode->_data, msg_node->_recnode->_cur_len));
                _msg_que.pop();
            }
        }
        // 队列有数据
        auto msg_node = _msg_que.front();
        spdlog::info("LogicSystem received msg id is {}", msg_node->_recnode->getId());
        auto call_back_iter = _fun_callbacks.find(msg_node->_recnode->getId());
        if (call_back_iter == _fun_callbacks.end()) {
            spdlog::warn("msg id is {}, has no handler", msg_node->_recnode->getId());
            _msg_que.pop();
            continue;
        }
        // 调用请求 id 对应的函数
        call_back_iter->second(msg_node->_session, msg_node->_recnode->getId(),
                               std::string(msg_node->_recnode->_data, msg_node->_recnode->_cur_len));
        _msg_que.pop();
    }
}

void LogicSystem::registerCallBacks() {
    _fun_callbacks[MSG_CHAT_LOGIN] = std::bind(&LogicSystem::loginHandler, this, std::placeholders::_1,
                                               std::placeholders::_2, std::placeholders::_3);
}

void LogicSystem::loginHandler(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto token = root["token"].asString();
    spdlog::info("User Login uid is {}, token is {}", uid, token);

    // 从状态服务器验证 token 和 uid
    // TODO 不知道为什么发送不到 StatusServer
    auto rsp = StatusGrpcClient::getInstance()->Login(uid, token);
    Json::Value return_value;

    Defer defer([this, &return_value, session]() {
        // 在 loginhandler 执行结束后执行
        std::string return_str = return_value.toStyledString();
        session->send(return_str, MSG_CHAT_LOGIN_RSP);
    });

    return_value["error"] = rsp.error();
    if (rsp.error() != ErrorCodes::Success) {
        spdlog::warn("Token or id verify failed, error is {}, uid is {}",rsp.error(),rsp.uid());
        return;
    }
    // 在内存中查询用户信息
    auto find_iter = _users.find(uid);
    std::shared_ptr<UserInfo> user_info = nullptr;
    if (find_iter == _users.end()) {
        // 没找到就去查数据库
        user_info = MysqlMgr::getInstance()->getUser(uid);
        if (user_info == nullptr) {
            return_value["error"] = ErrorCodes::UidInvalid;
            return;
        }
        _users[uid] = user_info;
    } else {
        user_info = find_iter->second;
    }
    // 返回给客户端
    return_value["uid"] = uid;
    return_value["token"] = token;
    return_value["user_name"] = user_info->user_name;
}
