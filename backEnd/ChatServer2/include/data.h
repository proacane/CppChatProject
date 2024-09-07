/**
 *  FileName: data.h
 *  CreateTime: 2024/9/7 14:39
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_DATA_H
#define CHATSERVER_DATA_H
#include <string>
struct UserInfo {
    UserInfo(): name(""), pwd(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""), back("") {}
    // 用户名？
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
    // 昵称
    std::string nick;
    // 描述
    std::string desc;
    int sex;
    // 头像
    std::string icon;
    // 备注
    std::string back;
};

struct ApplyInfo {
    ApplyInfo(int uid, std::string name, std::string desc,
              std::string icon, std::string nick, int sex, int status)
            :_uid(uid),_name(name),_desc(desc),
             _icon(icon),_nick(nick),_sex(sex),_status(status){}

    int _uid;
    std::string _name;
    // 描述
    std::string _desc;
    // 头像
    std::string _icon;
    // 昵称
    std::string _nick;
    int _sex;
    // ？？
    int _status;
};
#endif //CHATSERVER_DATA_H
