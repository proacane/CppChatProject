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
};

class Defer{
public:
    explicit Defer(std::function<void()> func):_func(std::move(func)){};
    ~Defer(){
        _func();
    }
private:
    std::function<void()> _func;
};
#endif //GATESERVER_CONST_H
