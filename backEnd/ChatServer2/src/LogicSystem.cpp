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
#include "../include/RedisMgr.h"
#include "../include/ConfigMgr.h"
#include "../include/UserMgr.h"

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


//    auto rsp = StatusGrpcClient::getInstance()->Login(uid, token);
    Json::Value return_value;
    Defer defer([this, &return_value, session]() {
        // 在 loginhandler 执行结束后执行
        std::string return_str = return_value.toStyledString();
        session->send(return_str, MSG_CHAT_LOGIN_RSP);
    });
    // 从redis中查询 token 和 uid
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisMgr::getInstance()->get(token_key, token_value);
    if (!success) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }
    if (token_value != token) {
        return_value["error"] = ErrorCodes::TokenInvalid;
        return;
    }

    // TODO 逻辑修改
    // 在redis中查询用户信息
    std::string base_key = USER_BASE_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool b_base = getBaseInfo(base_key, uid, user_info);
    if (!b_base) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }
    // 返回给客户端
    return_value["uid"] = uid;
//    return_value["password"] = user_info->pwd;
    return_value["token"] = token;
    return_value["name"] = user_info->name;
    return_value["error"] = ErrorCodes::Success;

    // TODO 从数据库获取申请列表
    // TODO 获取好友列表

    auto server_name = ConfigMgr::getInstance().getValue("SelfServer", "Name");
    //将登录数量增加
    auto rd_res = RedisMgr::getInstance()->hGet(LOGIN_COUNT, server_name);
    int count = 0;
    if (!rd_res.empty()) {
        count = std::stoi(rd_res);
    }
    count++;
    auto count_str = std::to_string(count);
    // 登陆数量写入 redis
    RedisMgr::getInstance()->hSet(LOGIN_COUNT, server_name, count_str);
    //session绑定用户uid
    session->setUserId(uid);
    //为用户设置登录ip server的名字
    std::string ipkey = USERIPPREFIX + uid_str;
    RedisMgr::getInstance()->set(ipkey, server_name);
    //uid和session绑定管理,方便以后踢人操作
    UserMgr::getInstance()->setUserSession(uid, session);
    
}

bool LogicSystem::getBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> userInfo) {
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
        userInfo->sex = root["sex"].asInt();
        userInfo->icon = root["icon"].asString();
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
        redis_root["sex"] = userInfo->sex;
        redis_root["icon"] = userInfo->icon;
        RedisMgr::getInstance()->set(base_key, redis_root.toStyledString());
    }
    return true;
}
