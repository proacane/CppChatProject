/**
 *  FileName: LogicSystem.h
 *  CreateTime: 2024/8/24 15:47
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_LOGICSYSTEM_H
#define CHATSERVER_LOGICSYSTEM_H

#include <functional>
#include "SingleTon.h"
#include <thread>
#include <mutex>
#include <queue>
#include <map>
#include <json/value.h>
#include "MysqlMgr.h"

class CSession;

class LogicNode;

typedef std::function<void(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data)> funCallBack;

class LogicSystem : public SingleTon<LogicSystem> {
    friend class SingleTon<LogicSystem>;

public:
    ~LogicSystem() override;

    void postMsgQue(std::shared_ptr<LogicNode> msg);

    LogicSystem(const LogicSystem &) = delete;

    LogicSystem &operator=(const LogicSystem &) = delete;

private:
    LogicSystem();

    // 处理消息
    void dealMsg();

    // 注册回调函数
    void registerCallBacks();

    // 处理登录请求
    void loginHandler(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data);

    // 处理查询用户请求
    void searchInfo(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);
    // 处理添加好友的请求
    void addFriendApply(std::shared_ptr<CSession> session, const short &msg_id, const std::string &msg_data);

    std::thread _worker_thread;
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::condition_variable _consume;
    bool _b_stop;
    std::map<short, funCallBack> _fun_callbacks;

    bool getBaseInfo(std::string basicString, int uid, std::shared_ptr<UserInfo> sharedPtr);

    // 判断字符串是否为纯数字
    bool isPureDigit(const std::string &s);

    void getUserById(const std::string &str, Json::Value& value);

    void getUserByName(const std::string& name, Json::Value& value);
};

#endif //CHATSERVER_LOGICSYSTEM_H
