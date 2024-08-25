/**
 *  FileName: data.h
 *  CreateTime: 2024/8/25 10:29
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_DATA_H
#define CHATSERVER_DATA_H

#include <string>

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};
#endif //CHATSERVER_DATA_H
