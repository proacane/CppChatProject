/**
 *  FileName: ChatGrpcClient.cpp
 *  CreateTime: 2024/9/7 13:43
 *  Description: 
 *  Author: ACAね
*/
#include <spdlog/spdlog.h>
#include "../include/ChatGrpcClient.h"
#include "../include/RedisMgr.h"
#include "../include/MysqlMgr.h"
#include "../include/UserMgr.h"
ChatConnectionPool::ChatConnectionPool(size_t pool_size, std::string host, std::string port) : _pool_size(pool_size),
                                                                                               _host(host), _port(port),
                                                                                               _b_stop(false) {
    for (size_t i = 0; i < _pool_size; i++) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                               grpc::InsecureChannelCredentials());

        _connections.push(ChatService::NewStub(channel));
    }
    spdlog::info("ChatServer connection pool total has {} connections", _connections.size());
}

ChatConnectionPool::~ChatConnectionPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    close();
    while (!_connections.empty()) {
        // 关闭连接
        _connections.pop();
    }
}

std::unique_ptr<ChatService::Stub> ChatConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this]() {
        if (_b_stop) {
            return true;
        }
        // 池子不为空就唤醒
        return !_connections.empty();
    });
    if (_b_stop) {
        // 池子已经关闭了
        return nullptr;
    }
    auto connection = std::move(_connections.front());
    _connections.pop();
    return connection;
}

void ChatConnectionPool::returnConnection(std::unique_ptr<ChatService::Stub> connection) {
    if (_b_stop) {
        return;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    _connections.push(std::move(connection));
    // 唤醒等待连接的线程
    _cond.notify_one();
}

void ChatConnectionPool::close() {
    _b_stop = true;
    _cond.notify_all();
}

// -----------------------------------------------------------------
ChatGrpcClient::ChatGrpcClient() {
    auto &cfg = ConfigMgr::getInstance();
    auto server_list = cfg["PeerServer"]["Servers"];
    spdlog::info("Peer server list is {}", server_list);
    std::vector<std::string> words;
    std::stringstream ss(server_list);
    std::string word;
    while (std::getline(ss, word, ',')) {
        words.push_back(word);
    }
    for (auto &w: words) {
        if (cfg[w]["Name"].empty()) {
            continue;
        }
        // 建立连接池
        _pools[cfg[w]["Name"]] = std::make_unique<ChatConnectionPool>(5, cfg[w]["Host"], cfg[w]["Port"]);
    }
}

// TODO 后续完善
AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq &req) {
    return AddFriendRsp();
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq &req) {
    return AuthFriendRsp();
}

bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo) {
    return false;
}

TextChatMsgRsp
ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq &req, const Json::Value &rtvalue) {
    return TextChatMsgRsp();
}
