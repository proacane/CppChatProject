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
#include "../include/ChatGrpcClient.h"

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
    _fun_callbacks[ID_SEARCH_USER_REQ] = std::bind(&LogicSystem::searchInfo, this, std::placeholders::_1,
                                                   std::placeholders::_2, std::placeholders::_3);
    _fun_callbacks[ID_ADD_FRIEND_REQ] = std::bind(&LogicSystem::addFriendApply, this, std::placeholders::_1,
                                                  std::placeholders::_2, std::placeholders::_3);
    _fun_callbacks[ID_AUTH_FRIEND_REQ] = std::bind(&LogicSystem::authFriendApply,this,std::placeholders::_1,
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

    // 在redis中查询用户信息
    std::string base_key = USER_BASE_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool b_base = getBaseInfo(base_key, uid, user_info);
    if (!b_base) {
        return_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    return_value["uid"] = uid;
    return_value["token"] = token;
    return_value["name"] = user_info->name;
    return_value["error"] = ErrorCodes::Success;
    return_value["gender"] = user_info->gender;
    return_value["avatar"] = user_info->avatar;
    return_value["nick"] = user_info->nick;
    // TODO 从数据库获取自己发出的好友申请与接收到的申请列表
    // 从数据库获取申请列表，暂时显示自己接收到的
    std::vector<std::shared_ptr<ApplyInfo>> apply_list;
    // 最多显示 20 条
    auto b_apply = MysqlMgr::getInstance()->getFriendApplyList(uid,20,apply_list);
    if(b_apply){
        for (auto & apply : apply_list) {
            Json::Value obj;
            obj["name"] = apply->_name;
            obj["uid"] = apply->_uid;
            obj["avatar"] = apply->_avatar;
            obj["nick"] = apply->_nick;
            obj["gender"] = apply->_gender;
            obj["desc"] = apply->_desc;
            obj["status"] = apply->_status;
            return_value["apply_list"].append(obj);
        }
    }else{
        spdlog::error("Some error occurred when query friend apply list");
    }

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
    // uid和session绑定管理,方便以后踢人操作
    UserMgr::getInstance()->setUserSession(uid, session);

}

bool LogicSystem::getBaseInfo(const std::string& base_key, int uid, std::shared_ptr<UserInfo> userInfo) {
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

void LogicSystem::searchInfo(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto request_str = root["searchInfo"].asString();
    spdlog::info("User search info is {}", request_str);

    Json::Value return_value;
    Defer defer([this, &return_value, &session] {
        std::string return_str = return_value.toStyledString();
        // 发送回客户端
        session->send(return_str, ID_SEARCH_USER_RSP);
    });

    // 判断是否为纯数字，纯数字就是 uid，否则就是用户名
    bool b_digit = isPureDigit(request_str);
    if (b_digit) {
        // 根据 uid 查询用户
        getUserById(request_str, return_value);
    } else {
        // 根据用户名查询用户
        getUserByName(request_str, return_value);
    }
}

bool LogicSystem::isPureDigit(const std::string &str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void LogicSystem::getUserById(const std::string &str, Json::Value &return_value) {
    return_value["error"] = ErrorCodes::Success;
    // 先在 redis 里查
    std::string base_key = USER_BASE_INFO + str;
    std::string info_str = "";
    bool b_base = RedisMgr::getInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto name = root["name"].asString();
        auto pwd = root["pwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto gender = root["gender"].asInt();
        auto avatar = root["avatar"].asString();
        spdlog::info("Search user info: \nuid : {},\nname : {},\npwd : {},\nemail : {},\navatar : {}\n", uid, name, pwd,
                     email, avatar);

        return_value["uid"] = uid;
        return_value["pwd"] = pwd;
        return_value["name"] = name;
        return_value["email"] = email;
        return_value["nick"] = nick;
        return_value["desc"] = desc;
        return_value["gender"] = gender;
        return_value["avatar"] = avatar;
        return;
    }
    // 在 mysql 中查
    auto uid = std::stoi(str);
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MysqlMgr::getInstance()->getUser(uid);
    if (user_info == nullptr) {
        return_value["error"] = ErrorCodes::UidInvalid;
        spdlog::info("There's no such a user");
        return;
    }
    // 查到了就添到 redis
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["pwd"] = user_info->pwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["gender"] = user_info->gender;
    redis_root["avatar"] = user_info->avatar;

    RedisMgr::getInstance()->set(base_key, redis_root.toStyledString());

    return_value["uid"] = user_info->uid;
    return_value["pwd"] = user_info->pwd;
    return_value["name"] = user_info->name;
    return_value["email"] = user_info->email;
    return_value["nick"] = user_info->nick;
    return_value["desc"] = user_info->desc;
    return_value["gender"] = user_info->gender;
    return_value["avatar"] = user_info->avatar;
}

void LogicSystem::getUserByName(const std::string &name, Json::Value &return_value) {
    return_value["error"] = ErrorCodes::Success;
    std::string base_key = NAME_INFO + name;
    std::string info_str = "";
    bool b_base = RedisMgr::getInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto r_name = root["name"].asString();
        auto pwd = root["pwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto gender = root["gender"].asInt();
        auto avatar = root["avatar"].asString();
        spdlog::info("Search user info: \nuid : {},\nname : {},\npwd : {},\nemail : {},\navatar : {}\n", uid, r_name,
                     pwd,
                     email, avatar);

        return_value["uid"] = uid;
        return_value["pwd"] = pwd;
        return_value["name"] = r_name;
        return_value["email"] = email;
        return_value["nick"] = nick;
        return_value["desc"] = desc;
        return_value["gender"] = gender;
        return_value["avatar"] = avatar;
        return;
    }
    // 在 mysql 查
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MysqlMgr::getInstance()->getUser(name);
    if (user_info == nullptr) {
        // TODO 更新错误码
        return_value["error"] = ErrorCodes::UidInvalid;
        spdlog::info("There's no such a user");
        return;
    }
// 查到了就添到 redis
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["pwd"] = user_info->pwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["gender"] = user_info->gender;
    redis_root["avatar"] = user_info->avatar;

    RedisMgr::getInstance()->set(base_key, redis_root.toStyledString());

    return_value["uid"] = user_info->uid;
    return_value["pwd"] = user_info->pwd;
    return_value["name"] = user_info->name;
    return_value["email"] = user_info->email;
    return_value["nick"] = user_info->nick;
    return_value["desc"] = user_info->desc;
    return_value["gender"] = user_info->gender;
    return_value["avatar"] = user_info->avatar;
}

void LogicSystem::addFriendApply(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto applyname = root["applyname"].asString();
    auto bakname = root["bakname"].asString();
    auto touid = root["touid"].asInt();
    spdlog::info("uid: {}, apply name: {}, backName: {}, to uid: {}",uid,applyname,bakname,touid);

    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;
    // 发送回申请的客户端
    Defer defer([this, &return_value, session]() {
        std::string return_str = return_value.toStyledString();
        session->send(return_str, ID_ADD_FRIEND_RSP);
    });

    // 更新数据库
    bool up_database = MysqlMgr::getInstance()->addFriendApply(uid, touid);
    if(!up_database){
        spdlog::error("Error occurred in addFriendApply while updating database");
        return_value["error"] = ErrorCodes::ApplyFriendFail;
        return;
    }

    // 在 redis 中查 touid 所在的服务器
    auto to_str = std::to_string(touid);
    auto to_ip_key = USERIPPREFIX + to_str;
    std::string to_ip_value = "";
    bool b_ip = RedisMgr::getInstance()->get(to_ip_key,to_ip_value);
    if(!b_ip){
        spdlog::error("Error occurred in addFriendApply while querying redis");
        return_value["error"] = ErrorCodes::ApplyFriendFail;
        return;
    }

    // 获取自己的服务器名称
    auto& cfg = ConfigMgr::getInstance();
    auto self_name = cfg["SelfServer"]["Name"];

    std::string base_key = USER_BASE_INFO + std::to_string(uid);
    auto apply_info = std::make_shared<UserInfo>();
    bool b_info = getBaseInfo(base_key,uid,apply_info);

    // 处于一个服务器，直接发送就行
    if(to_ip_value == self_name){
        spdlog::info("They are in the same server");
        auto s = UserMgr::getInstance()->getSession(touid);
        if(s){
            // 缓存在了内存中就直接发送
            return_value["error"] = ErrorCodes::Success;
            return_value["applyuid"] = uid;
            return_value["name"] = applyname;
            return_value["desc"] = "";
            if(b_info){
                return_value["avatar"] = apply_info->avatar;
                return_value["gender"] = apply_info->gender;
                return_value["nick"] = apply_info->nick;
            }
        }else{
            return_value["error"] = ErrorCodes::ApplyFriendFail;
            spdlog::error("user is not online");
        }
        std::string return_str = return_value.toStyledString();
        // 发送到被申请的客户端
        s->send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
        return;
    }

    // 不在一个服务器，调用 grpc 通信
    AddFriendReq add_req;
    add_req.set_applyuid(uid);
    add_req.set_name(applyname);
    add_req.set_touid(touid);
    add_req.set_desc("");
    if(b_info){
        add_req.set_avatar(apply_info->avatar) ;
        add_req.set_gender(apply_info->gender) ;
        add_req.set_nick(apply_info->nick) ;
    }

    ChatGrpcClient::getInstance()->NotifyAddFriend(to_ip_value,add_req);
}

void LogicSystem::authFriendApply(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data) {
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto from_uid = root["fromuid"].asInt();
    auto to_uid = root["touid"].asInt();
    auto back_name = root["back"].asString();
    spdlog::info("uid {} has authed uid: {} friend apply, back name is {}",from_uid,to_uid,back_name);

    Json::Value return_value;
    return_value["error"] = ErrorCodes::Success;

    auto user_info = std::make_shared<UserInfo>();
    std::string base_key = USER_BASE_INFO + std::to_string(to_uid);
    // 查询被同意的用户信息
    bool b_info = getBaseInfo(base_key,to_uid,user_info);
    if(b_info){
        return_value["name"] = user_info->name;
        return_value["email"] = user_info->email;
        return_value["nick"] = user_info->nick;
        return_value["gender"] = user_info->gender;
        return_value["avatar"] = user_info->avatar;
        return_value["uid"] = user_info->uid;
    }else{
        return_value["error"] = ErrorCodes::UidInvalid;
        spdlog::warn("Can't find user date where uid is {}",to_uid);
        return;
    }
    // 这是返回给认证同意的客户端
    Defer defer([this,&return_value,session](){
        std::string return_str = return_value.toStyledString();
        session->send(return_str,ID_AUTH_FRIEND_RSP);
    });

    // 先更新申请状态为 1
   bool b_update =  MysqlMgr::getInstance()->authFriendApply(from_uid,to_uid);
   if(!b_update){
       return_value["error"] = ErrorCodes::UidInvalid;
       spdlog::warn("Can't update status when authed friend apply ");
       return;
   }
    b_update= MysqlMgr::getInstance()->addFriend(from_uid,to_uid,back_name);
    if(!b_update){
        return_value["error"] = ErrorCodes::UidInvalid;
        spdlog::warn("Can't add friend");
        return;
    }

    // 查看对方所处的服务器
    //查询redis 查找touid对应的server ip
    auto to_str = std::to_string(to_uid);
    auto to_ip_key = USERIPPREFIX + to_str;
    std::string to_ip_value = "";
    bool b_ip = RedisMgr::getInstance()->get(to_ip_key, to_ip_value);
    if (!b_ip) {
        return;
    }

    auto& cfg = ConfigMgr::getInstance();
    auto self_name = cfg["SelfServer"]["Name"];
    // 下面是发给被同意的客户端
    if(to_ip_value == self_name){
        // 处于同一服务器
        auto s = UserMgr::getInstance()->getSession(to_uid);
        if(s){
            //在内存中则直接发送通知对方
            Json::Value  notify;
            notify["error"] = ErrorCodes::Success;
            notify["fromuid"] = from_uid;
            notify["touid"] = to_uid;
            std::string base_key2 = USER_BASE_INFO + std::to_string(from_uid);
            auto user_info2 = std::make_shared<UserInfo>();
            bool b_info2 = getBaseInfo(base_key2, from_uid, user_info2);
            if (b_info2) {
                notify["name"] = user_info->name;
                notify["nick"] = user_info->nick;
                notify["avatar"] = user_info->avatar;
                notify["gender"] = user_info->gender;
            }
            else {
                notify["error"] = ErrorCodes::UidInvalid;
            }

            std::string return_str = notify.toStyledString();
            s->send(return_str, ID_NOTIFY_AUTH_FRIEND_REQ);
        }
        return;
    }

    // 不在同一服务器就 grpc 通知
    AuthFriendReq auth_req;
    auth_req.set_fromuid(from_uid);
    auth_req.set_touid(to_uid);

    //发送通知
    ChatGrpcClient::getInstance()->NotifyAuthFriend(to_ip_value, auth_req);
}
