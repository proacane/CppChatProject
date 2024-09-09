/**
 *  FileName: const.h
 *  CreateTime: 2024/8/13 14:36
 *  Description: 复用的头文件、常量
 *  Author: ACAね
*/
#ifndef GATESERVER_CONST_H
#define GATESERVER_CONST_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <functional>
#include <map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <utility>
#include <spdlog/spdlog.h>

#define MAX_LENGTH (1024*2)
#define HEAD_TOTAL_LEN 4
#define HEAD_ID_LEN 2
#define HEAD_DATA_LEN 2
#define MAX_REC_QUE  10000
#define MAX_SEND_QUE 1000
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

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

class Defer {
public:
    explicit Defer(std::function<void()> func) : _func(std::move(func)) {};

    ~Defer() {
        _func();
    }

private:
    std::function<void()> _func;
};

enum MSG_IDS {
    MSG_CHAT_LOGIN = 1005, //用户登陆
    MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
    ID_SEARCH_USER_REQ = 1007, //用户搜索请求
    ID_SEARCH_USER_RSP = 1008, //搜索用户回包
    ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
    ID_ADD_FRIEND_RSP  = 1010, //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};

// 用户连接到的ChatServer
#define USERIPPREFIX "uid_"
#define USERTOKENPREFIX "utoken_"
#define IPCOUNTPREFIX "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT "logincount"
#define NAME_INFO  "nameinfo_"
#endif //GATESERVER_CONST_H
