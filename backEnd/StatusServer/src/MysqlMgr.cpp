/**
 *  FileName: MysqlMgr.cpp
 *  CreateTime: 2024/8/21 15:19
 *  Description: 
 *  Author: ACAね
*/
#include "../include/MysqlMgr.h"

int MysqlMgr::registerUser(const std::string &user_name, const std::string &email, const std::string &password) {
    return _dao.registerUser(user_name, email, password);
}

int MysqlMgr::checkEmailUserName(const std::string &user_name, const std::string &email) {
    return _dao.checkEmailUserName(user_name,email);
}

int MysqlMgr::updatePassword(const std::string &user_name, const std::string &password) {
    return _dao.updatePassword(user_name,password);
}

bool MysqlMgr::checkPassword(const std::string &user_name, const std::string &password, UserInfo &userInfo) {
    return _dao.checkPassword(user_name,password,userInfo);
}