/**
 *  FileName: StatusGrpcClient .cpp
 *  CreateTime: 2024/8/23 15:16
 *  Description: 
 *  Author: ACAね
*/
#include <memory>
#include <spdlog/spdlog.h>

#include "../../ChatServer2/include/StatusGrpcClient.h"
#include "../include/ConfigMgr.h"

StatusGrpcClient::~StatusGrpcClient() = default;

GetChatServerRsp StatusGrpcClient::getChatServer(int uid) {
    ClientContext context;
    GetChatServerRsp reply;
    GetChatServerReq request;
    request.set_uid(uid);
    auto stub = pool_->getConnection();
    Status status = stub->GetChatServer(&context, request, &reply);
    Defer defer([&stub, this]() {
        pool_->returnConnection(std::move(stub));
    });
    if (status.ok()) {
        return reply;
    } else {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

StatusGrpcClient::StatusGrpcClient() {
    auto &gCfgMgr = ConfigMgr::getInstance();
    std::string host = gCfgMgr["StatusServer"]["Host"];
    std::string port = gCfgMgr["StatusServer"]["Port"];
//    spdlog::info("grpc server host is {}, port is {}", host, port);
    pool_ = std::make_unique<StatusConPool>(5, host, port);
}

LoginRsp StatusGrpcClient::Login(int uid, std::string token) {
    ClientContext context;
    LoginRsp reply;
    LoginReq request;
    request.set_uid(uid);
    request.set_token(token);

    auto stub = pool_->getConnection();
    if (!stub) {
        spdlog::error("Get stub failed");
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }

    Status status = stub->Login(&context, request, &reply);
    Defer defer([&stub, this]() {
        pool_->returnConnection(std::move(stub));
    });

    if (status.ok()) {
        return reply;
    } else {
        int err = (int) status.error_code();
        spdlog::error("grpc call failed in Login: {}, {}", err, status.error_details());
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

StatusConPool::StatusConPool(size_t pool_size, std::string host, std::string port) : _poolSize(pool_size), _host(host),
                                                                                     _port(port), _b_stop(false) {
    for (size_t i = 0; i < _poolSize; i++) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                               grpc::InsecureChannelCredentials());

        _connections.push(StatusService::NewStub(channel));
    }
    spdlog::info("Status connection pool total has {} connections", _connections.size());
}

StatusConPool::~StatusConPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    close();
    while (!_connections.empty()) {
        // 关闭连接
        _connections.pop();
    }
}

std::unique_ptr<StatusService::Stub> StatusConPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] {
        if (_b_stop) {
            return true;
        }
        return !_connections.empty();
    });
    if (_b_stop) {
        return nullptr;
    }
    auto connection = std::move(_connections.front());
    _connections.pop();

    return connection;
}

void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> connection) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_b_stop) {
        return;
    }
    _connections.push(std::move(connection));
    // 唤醒正在等待连接的线程
    _cond.notify_one();
}

void StatusConPool::close() {
    _b_stop = true;
    _cond.notify_all();
}
