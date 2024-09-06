#include "usermgr.h"

void UserMgr::setUserName(QString name) {
    _user_name = std::move(name);
}

void UserMgr::setUid(int uid) {
    _uid = uid;
}

void UserMgr::setToken(QString token) {
    _token = std::move(token);
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::getApplyList() {
    return _apply_list;
}
