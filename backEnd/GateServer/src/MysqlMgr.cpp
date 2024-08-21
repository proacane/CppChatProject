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
