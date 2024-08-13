/**
 *  FileName: LogicSystem.cpp
 *  CreateTime: 2024/8/13 15:28
 *  Description: 
 *  Author: ACAね
*/
#include "../include/LogicSystem.h"
#include "../include/HttpConnection.h"
void LogicSystem::registerGet(std::string url, httpHandler handler) {
    _get_handlers.insert(std::make_pair(url,handler));
}

LogicSystem::LogicSystem() {
    registerGet("/get_test",[](std::shared_ptr<HttpConnection> connection){
        beast::ostream(connection->_response.body())<<"receive get_test req";
    });
}

bool LogicSystem::handleGet(std::string url, std::shared_ptr<HttpConnection> connection) {
    if(_get_handlers.find(url) == _get_handlers.end()){
        // 该路由未注册
        return false;
    }
    _get_handlers[url](connection);
    return true;
}

LogicSystem::~LogicSystem() {

}
