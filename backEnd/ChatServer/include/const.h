/**
 *  FileName: const.h
 *  CreateTime: 2024/8/24 14:37
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_CONST_H
#define CHATSERVER_CONST_H
#include <functional>
#define MAX_LENGTH (1024*2)
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_REC_QUE  10000
#define MAX_SEND_QUE 1000

enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
    VerifyExpired = 1003,// 验证码过期
    VerifyCodeErr = 1004, //验证码错误
    UserExist = 1005,       //用户已经存在
    PasswordErr = 1006, // 密码校验错误
    UserEmailNotMatch = 1007, // 用户名与邮箱不匹配
    PasswordUpdateFail = 1008, // 更新密码失败
    PasswordSame = 1009, // 重置密码重复
    UserPasswordError = 1010,// 密码错误或用户不存在
    TokenInvalid = 1011,   //Token失效
    UidInvalid = 1012,  //uid无效
};

class Defer{
public:
    explicit Defer(std::function<void()> func):_func(std::move(func)){};
    ~Defer(){
        _func();
    };
private:
    std::function<void()> _func;
};


enum MSG_IDS {
    MSG_CHAT_LOGIN = 1005, //用户登陆
    MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
};

#endif //CHATSERVER_CONST_H
