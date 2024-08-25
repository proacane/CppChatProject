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
    reader.parse(msg_data,root);
    auto uid = root["uid"].asInt();
    spdlog::info("User login uid is {}, token is {}",uid,root["token"].asString());

    // TODO 从状态服务器验证 token 和 uid
//    auto rsp = StatusGrpcClient::getInstance()->

    std::string return_str = root.toStyledString();
    session->send(return_str, msg_id);
}
