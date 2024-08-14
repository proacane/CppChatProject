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

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
};
#endif //GATESERVER_CONST_H
