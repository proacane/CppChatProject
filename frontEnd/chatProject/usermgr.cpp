#include "usermgr.h"


void UserMgr::setUserName(QString name)
{
    _user_name = std::move(name);
}

void UserMgr::setUid(int uid)
{
    _uid = uid;
}

void UserMgr::setToken(QString token)
{
    _token = std::move(token);
}
