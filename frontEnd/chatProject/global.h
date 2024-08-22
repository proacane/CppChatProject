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
};

enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,  // Json解析失败
    ERR_NETWORK = 2,
};

enum Modules {
    // 注册模块
    REGISTERMOD = 0,
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
enum ClickLbState{
    Normal = 0,
    Selected = 1
};
extern QString gate_url_prefix;

extern QString hashPassword(const QString& password);
#endif  // GLOBAL_H
