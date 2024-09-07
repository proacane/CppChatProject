/**
 *  FileName: StatusServiceImpl.cpp
 *  CreateTime: 2024/8/23 15:49
 *  Description: 
 *  Author: ACAね
*/
#include "../include/StatusServiceImpl.h"
#include "../include/const.h"
#include"../include/ConfigMgr.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <utility>
#include "../include/RedisMgr.h"

std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}

StatusServiceImpl::StatusServiceImpl() {
    auto &cfg = ConfigMgr::getInstance();
    auto server_list = cfg["chatServers"]["Name"];
    std::vector<std::string> words;
    std::stringstream ss(server_list);
    std::string word;
    while (std::getline(ss, word, ',')) {
        words.push_back(word);
    }

    ChatServer server;
    for (auto &w: words) {
        if (cfg[w]["Name"].empty()) {
            continue;
        }
        server.name = cfg[w]["Name"];
        server.host = cfg[w]["Host"];
        server.port = cfg[w]["Port"];
        server.connect_count = 0;
        _servers[server.name] = server;
    }
    spdlog::info("Init {} chat servers", _servers.size());
}

Status
StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply) {
    spdlog::info("Status server received from uid {}", request->uid());
    const auto &server = getChatServer();
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    spdlog::info("[{},{}]", request->uid(), reply->token());
    insertToken(request->uid(), reply->token());
    return Status::OK;
}

ChatServer StatusServiceImpl::getChatServer() {
    std::lock_guard<std::mutex> lock(_server_mutex);
//     获取连接数最少的 server
    auto minServer = _servers.begin()->second;
    auto count_str = RedisMgr::getInstance()->hGet(LOGIN_COUNT, minServer.name);
    if (count_str.empty()) {
        //不存在则默认设置为最大
        minServer.connect_count = INT_MAX;
    } else {
        minServer.connect_count = std::stoi(count_str);
    }
    for (auto &server: _servers) {
        if (server.second.name == minServer.name) {
            continue;
        }
        auto count_str = RedisMgr::getInstance()->hGet(LOGIN_COUNT, server.second.name);
        if (count_str.empty()) {
            server.second.connect_count = INT_MAX;
        } else {
            server.second.connect_count = std::stoi(count_str);
        }
        if (server.second.connect_count < minServer.connect_count) {
            minServer = server.second;
        }
    }
    return minServer;
}

void StatusServiceImpl::insertToken(int uid, std::string token) {
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    RedisMgr::getInstance()->set(token_key,token);
}

Status StatusServiceImpl::Login(ServerContext *context, const LoginReq *request,
                                LoginRsp *response) {
    auto uid = request->uid();
    auto token = request->token();
    spdlog::info("uid is {}, token is {}", uid, token);
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisMgr::getInstance()->get(token_key,token_value);
    if(!success){
        response->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }
    if(token_value != token){
        response->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }

    response->set_error(ErrorCodes::Success);
    response->set_uid(uid);
    response->set_token(token);
    return Status::OK;
}
