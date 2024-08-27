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
#include "MysqlMgr.h"
class CSession;

class LogicNode;

typedef std::function<void(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data)> funCallBack;

class LogicSystem : public SingleTon<LogicSystem> {
    friend class SingleTon<LogicSystem>;

public:
    ~LogicSystem();

    void postMsgQue(std::shared_ptr<LogicNode> msg);

    LogicSystem(const LogicSystem &) = delete;

    LogicSystem &operator=(const LogicSystem &) = delete;

private:
    LogicSystem();

    // 处理消息
    void dealMsg();

    // 注册回调函数
    void registerCallBacks();

    void loginHandler(std::shared_ptr<CSession>, const short &msg_id, const std::string &msg_data);
    std::thread _worker_thread;
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::condition_variable _consume;
    bool _b_stop;
    std::map<short, funCallBack> _fun_callbacks;
    std::unordered_map<int, std::shared_ptr<UserInfo>> _users;
};

#endif //CHATSERVER_LOGICSYSTEM_H
