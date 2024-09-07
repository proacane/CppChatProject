/**
 *  FileName: UserMgr.h
 *  CreateTime: 2024/9/7 14:49
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_USERMGR_H
#define CHATSERVER_USERMGR_H
#include "SingleTon.h"
#include <unordered_map>
#include <memory>
#include <mutex>
class CSession;

class UserMgr:public SingleTon<UserMgr>{
    friend class SingleTon<UserMgr>;
public:
    ~UserMgr() override;
    std::shared_ptr<CSession> getSession(int uid);
    void setUserSession(int uid, std::shared_ptr<CSession> session);
    void rmvUserSession(int uid);
private:
    UserMgr();
    std::mutex _session_mtx;
    std::unordered_map<int, std::shared_ptr<CSession>> _uid_to_session;
};

#endif //CHATSERVER_USERMGR_H
