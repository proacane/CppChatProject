#pragma once
#ifndef GLOBAL_H
#    define GLOBAL_H
#    include <QJsonObject>
#    include <QStyle>
#    include <QWidget>
#    include <functional>

extern std::function<void(QWidget*)> repolish;

enum ReqId {
    ID_GET_VARIFY_CODE = 1001,  // 获取验证码
    ID_REG_USER = 1002,         // 注册用户
    ID_RESET_PWD = 1003,        // 重置密码
    ID_LOGIN_USER = 1004,        // 登录
    ID_CHAT_LOGIN = 1005, //登录聊天服务器
    ID_CHAT_LOGIN_RSP = 1006, //登录聊天服务器回包
};

enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,  // Json解析失败
    ERR_NETWORK = 2,
    UserExist = 1005,          // 用户已经存在
    UserEmailNotMatch = 1007,  // 用户名或密码不匹配
    PasswordSame = 1009,       // 重置密码重复
    VerifyExpired = 1003,      // 验证码过期
    UserPasswordError = 1010,// 密码错误或用户不存在
};

enum Modules {
    // 注册模块
    REGISTERMOD = 0,
    // 重置密码模块
    RESETMOD = 1,
    LOGINMOD = 2
};

// 提示代码
enum TipErr {
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6

};

// 标签是否被选中
enum ClickLbState { Normal = 0, Selected = 1 };
extern QString gate_url_prefix;

extern QString hashPassword(const QString& password);

static QRegularExpression email_regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");

// ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
static QRegularExpression password_regex("^[A-Za-z0-9@#$%^&+!=.]{8,16}$");


struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};
#endif  // GLOBAL_H
