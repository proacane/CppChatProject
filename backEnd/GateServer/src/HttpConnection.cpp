/**
 *  FileName: HttpConnection.cpp
 *  CreateTime: 2024/8/13 14:37
 *  Description: 
 *  Author: ACAね
*/
#include "../include/HttpConnection.h"
#include <iostream>
#include "../include/LogicSystem.h"

HttpConnection::HttpConnection(tcp::socket socket) : _socket(std::move(socket)) {

}

void HttpConnection::start() {
    auto self = shared_from_this();

    http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, std::size_t bytes_transferred) {
        try {
            if (ec) {
                std::cout << "http read err is " << ec.what() << std::endl;
                return;
            }
            // 处理读取的数据，同时检测超时
            self->handleRequest();
            self->checkDeadline();
        } catch (std::exception &e) {
            std::cerr << "Exception in HttpConnection::start,which is " << e.what() << std::endl;
        }
    });
}

void HttpConnection::checkDeadline() {
    auto self = shared_from_this();
    _deadline.async_wait([self](boost::system::error_code ec) {
        if (!ec) {
            // 超时了
            self->_socket.close();
        }
    });
}

void HttpConnection::handleRequest() {
    // 设置版本
    _response.version(_request.version());
    // 设置为短连接
    _response.keep_alive(false);

    if (_request.method() == http::verb::get) {
        // 使用逻辑队列进行处理
        bool success = LogicSystem::getInstance()->handleGet(_request.target(), shared_from_this());
        if (!success) {
            // 出问题了
            _response.result(http::status::not_found);
            // 回应类型为文本类型
            _response.set(http::field::content_type, "text/plain");
            // 向消息体中写数据
            beast::ostream(_response.body()) << "url not found\r\n";
            // 发送回去
            writeResponse();
            return;
        }
        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        writeResponse();
    }
}

void HttpConnection::writeResponse() {
    auto self = shared_from_this();
    // 设置长度
    _response.content_length(_response.body().size());
    http::async_write(_socket, _response, [self](beast::error_code ec, std::size_t) {
        // 关闭发送端
        self->_socket.shutdown(tcp::socket::shutdown_send, ec);
        // 取消定时器
        self->_deadline.cancel();
    });
}
