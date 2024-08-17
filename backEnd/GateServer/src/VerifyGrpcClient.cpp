/**
 *  FileName: VerifyGrpcClient.cpp
 *  CreateTime: 2024/8/15 16:38
 *  Description: 
 *  Author: ACAね
*/

#include <memory>

#include "../include/VerifyGrpcClient.h"
#include "../include/ConfigMgr.h"

GetVerifyRsp VerifyGrpcClient::getVerifyCode(std::string email) {
    ClientContext context;
    GetVerifyRsp reply;
    GetVerifyReq request;
    request.set_email(email);
    auto stub = _pool->getConnection();
    Status status = stub->GetVerifyCode(&context, request, &reply);
    if (status.ok()) {
        reply.set_error(ErrorCodes::Success);
    } else {
        reply.set_error(ErrorCodes::RPCFailed);
    }
    _pool->returnConnection(std::move(stub));
    return reply;
}

VerifyGrpcClient::VerifyGrpcClient() {
    auto& gCfgMgr = ConfigMgr::getInstance();
    std::string host = gCfgMgr["VerifyServer"]["Host"];
    std::string port = gCfgMgr["VerifyServer"]["Port"];
    _pool = std::make_unique<RPConPool>(5, host, port);
}

RPConPool::RPConPool(size_t poolSize, std::string host, std::string port) : _pool_size(poolSize), _host(host),
                                                                            _port(port), _b_stop(false) {
    // 初始化连接池，创建指定数量的 gRPC 连接
    for (std::size_t i = 0; i < poolSize; i++) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                                                               grpc::InsecureChannelCredentials());
        _connections.push(VerifyService::NewStub(channel));
    }
}

RPConPool::~RPConPool() {
    std::lock_guard<std::mutex> lock(_mutex);
    // 标记连接池停止
    close();
    while(!_connections.empty()){
        // 关闭连接
        _connections.pop();
    }
}

void RPConPool::close() {
    _b_stop = true;
    // 通知所有等待获取连接的线程
    _cond.notify_all();
}

std::unique_ptr<VerifyService::Stub> RPConPool::getConnection() {
    std::unique_lock<std::mutex> lock(_mutex);
    // 返回 false 会挂起，等待唤醒
    _cond.wait(lock,[this]{
        if(_b_stop){
            return true;
        }
        return !_connections.empty();
    });
    if(_b_stop){
        return nullptr;
    }
    // 获取连接
    auto context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void RPConPool::returnConnection(std::unique_ptr<VerifyService::Stub> context) {
    std::lock_guard<std::mutex> lock(_mutex);
    if(_b_stop){
        return;
    }
    _connections.push(std::move(context));
    // 唤醒正在等待连接的线程
    _cond.notify_one();
}
