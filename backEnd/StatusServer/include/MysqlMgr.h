/**
 *  FileName: MysqlMgr.h
 *  CreateTime: 2024/8/21 15:18
 *  Description: 
 *  Author: ACAね
*/
#ifndef GATESERVER_MYSQLMGR_H
#define GATESERVER_MYSQLMGR_H

#include "SingleTon.h"
#include "MysqlDao.h"
class MysqlMgr : public SingleTon<MysqlMgr> {
    friend class SingleTon<MysqlMgr>;
public:
    int registerUser(const std::string& user_name, const std::string& email, const std::string& password);
    ~MysqlMgr() override =default;
    int checkEmailUserName(const std::string & user_name,const std::string & email);
    int updatePassword(const std::string&user_name,const std::string& password);
    bool checkPassword(const std::string &user_name, const std::string &password, UserInfo &userInfo);
private:
    MysqlMgr() = default;
    MysqlDao _dao;
};

#endif //GATESERVER_MYSQLMGR_H