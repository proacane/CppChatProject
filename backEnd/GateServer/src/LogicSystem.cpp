/**
 *  FileName: LogicSystem.cpp
 *  CreateTime: 2024/8/13 15:28
 *  Description: 
 *  Author: ACAね
*/
#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"

void LogicSystem::registerGet(std::string url, httpHandler handler) {
    _get_handlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem() {
    registerGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req\r\n";
        int i = 0;
        for (const auto &item: connection->_get_params) {
            i++;
            beast::ostream(connection->_response.body()) << "param " << i << " key is " << item.first;
            beast::ostream(connection->_response.body()) << ", " << i << " value is " << item.second << std::endl;
        }
    });
    // 处理验证码
    registerPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        // 请求转换为 string
        auto body_str = beast::buffers_to_string(connection->_request.body().data());
        std::cout << "Receive body is " << body_str << std::endl;
        // 响应类型为 json
        connection->_response.set(http::field::content_type, "text/json");
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;
        // 请求解析为json
        bool parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            // 解析失败
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return;
        }

        auto email = src_root["email"].asString();
        std::cout << "email is " << email << std::endl;
        root["error"] = ErrorCodes::Success;
        root["email"] = src_root["email"];
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
    });
}

bool LogicSystem::handleGet(std::string url, std::shared_ptr<HttpConnection> connection) {
    if (_get_handlers.find(url) == _get_handlers.end()) {
        // 该路由未注册
        return false;
    }
    _get_handlers[url](connection);
    return true;
}

LogicSystem::~LogicSystem() {

}

void LogicSystem::registerPost(std::string url, httpHandler handler) {
    _post_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::handlePost(std::string url, std::shared_ptr<HttpConnection> connection) {
    if(_post_handlers.find(url) == _post_handlers.end()){
        return false;
    }
    _post_handlers[url](connection);
    return true;
}
