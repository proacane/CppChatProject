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
        // 解析路由
        preParseGetParam();
        // 使用逻辑队列进行处理
        bool success = LogicSystem::getInstance()->handleGet(_get_url, shared_from_this());
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
    } else if (_request.method() == http::verb::post) {
        // 直接投递
        bool success = LogicSystem::getInstance()->handlePost(_request.target(), shared_from_this());
        if (!success) {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "url not found\r\n";
            writeResponse();
            return;
        }
        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        writeResponse();
        return;
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


// 字符 转为16进制
unsigned char ToHex(unsigned char x) {
    return x > 9 ? x + 55 : x + 48;
}

// 16进制转10进制
unsigned char FromHex(unsigned char x) {
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else
        assert(0);
    return y;
}

// url 编码
std::string UrlEncode(const std::string &str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        // 判断是否仅有数字和字母构成
        if (isalnum((unsigned char) str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ') //为空字符
            strTemp += "+";
        else {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制
            strTemp += '%';
            strTemp += ToHex((unsigned char) str[i] >> 4);
            strTemp += ToHex((unsigned char) str[i] & 0x0F);
        }
    }
    return strTemp;
}

// url 解码
std::string UrlDecode(const std::string &str) {
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
            //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%') {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char) str[++i]);
            unsigned char low = FromHex((unsigned char) str[++i]);
            strTemp += high * 16 + low;
        } else strTemp += str[i];
    }
    return strTemp;
}

void HttpConnection::preParseGetParam() {
    // 提取 uri
    auto uri = _request.target();
    // 查找 ? 的位置，也就是参数的位置
    auto query_pos = uri.find('?');
    if (query_pos == std::string::npos) {
        // 没有参数
        _get_url = uri;
        return;
    }
    _get_url = uri.substr(0, query_pos);

    std::string query_string = uri.substr(query_pos + 1);
    // 参数的键值
    std::string key;
    std::string value;
    size_t pos = 0;

    // 查找参数
    while ((pos = query_string.find('&')) != std::string::npos) {
        // 获取参数
        auto pair = query_string.substr(0, pos);
        // key value 的分割点
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(pair.substr(0, eq_pos));
            value = UrlDecode(pair.substr(eq_pos + 1));
            _get_params[key] = value;
        }
        query_string.erase(0, pos + 1);
    }
    // 如果最后一个参数没有 & ，单独处理
    if (!query_string.empty()) {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos) {
            key = UrlDecode(query_string.substr(0, eq_pos));
            value = UrlDecode(query_string.substr(eq_pos + 1));
            _get_params[key] = value;
        }
    }
}