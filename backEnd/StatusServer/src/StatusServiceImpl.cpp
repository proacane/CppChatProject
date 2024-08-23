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

std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}

StatusServiceImpl::StatusServiceImpl() {
    auto &cfg = ConfigMgr::getInstance();
    ChatServer server;
    server.port = cfg["ChatServer1"]["Port"];
    server.host = cfg["ChatServer1"]["Host"];
    server.name  = cfg["ChatServer1"]["Name"];
    server.connect_count = 0;
    _servers[server.name] = server;

    server.port = cfg["ChatServer2"]["Port"];
    server.host = cfg["ChatServer2"]["Host"];
    server.name = cfg["ChatServer2"]["Name"];
    server.connect_count = 0;
    _servers[server.name] = server;
}

Status
StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *reply) {
    spdlog::info("Status server received  ");
    const auto& server = getChatServer();
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    insertToken(request->uid(), reply->token());
    return Status::OK;
}

ChatServer StatusServiceImpl::getChatServer() {
    std::lock_guard<std::mutex> lock(_server_mutex);
    // 获取连接数最少的 server
    auto min_server = _servers.begin()->second;
    for (const auto &server: _servers) {
        if (server.second.connect_count < min_server.connect_count) {
            min_server = server.second;
        }
    }
    return min_server;
}

void StatusServiceImpl::insertToken(int uid, std::string token) {
    std::lock_guard<std::mutex> lock(_token_mutex);
    _tokens[uid] = std::move(token);
}
