/**
 *  FileName: HttpConnection.h
 *  CreateTime: 2024/8/13 14:37
 *  Description: 管理 Http 连接
 *  Author: ACAね
*/
#ifndef GATESERVER_HTTPCONNECTION_H
#define GATESERVER_HTTPCONNECTION_H

#include "const.h"

class LogicSystem;

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {
    friend class LogicSystem;

public:
    explicit HttpConnection(tcp::socket socket);
    explicit HttpConnection(boost::asio::io_context& ioc);
    void start();
    tcp::socket& getSocket();
private:

    // 超时检测
    void checkDeadline();

    // 回应
    void writeResponse();

    // 处理请求
    void handleRequest();

    // 解析get请求
    void preParseGetParam();
    tcp::socket _socket;
    // 接收缓冲区，用来接收数据
    beast::flat_buffer _buffer{8192};
    // 用来解析请求
    http::request<http::dynamic_body> _request;
    // 用来回应客户端
    http::response<http::dynamic_body> _response;
    // 定时器
    net::steady_timer _deadline{
            _socket.get_executor(), std::chrono::seconds(60)
    };
    // get url
    std::string _get_url;
    // get 参数
    std::unordered_map<std::string, std::string> _get_params;

};

#endif //GATESERVER_HTTPCONNECTION_H
