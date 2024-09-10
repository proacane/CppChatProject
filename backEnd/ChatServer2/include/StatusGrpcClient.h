/**
 *  FileName: StatusGrpcClient .h
 *  CreateTime: 2024/8/23 15:15
 *  Description: 与状态服务进行通信
 *  Author: ACAね
*/
#ifndef GATESERVER_STATUSGRPCCLIENT_H
#define GATESERVER_STATUSGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "../proto/message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include <atomic>
#include <queue>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginRsp;
using message::LoginReq;
using message::AddFriendReq;


class StatusConPool {
public:
    StatusConPool(size_t pool_size, std::string host, std::string port);

    ~StatusConPool();

    std::unique_ptr<StatusService::Stub> getConnection();

    void returnConnection(std::unique_ptr<StatusService::Stub> connection);

    void close();

private:
    std::atomic<bool> _b_stop;
    size_t _poolSize;
    std::string _host;
    std::string _port;
    std::queue<std::unique_ptr<StatusService::Stub>> _connections;
    std::mutex _mutex;
    std::condition_variable _cond;
};

class StatusGrpcClient : public SingleTon<StatusGrpcClient> {
    friend class SingleTon<StatusGrpcClient>;

public:
    ~StatusGrpcClient() override;

    GetChatServerRsp getChatServer(int uid);
    StatusGrpcClient(const StatusGrpcClient&) = delete;
    StatusGrpcClient& operator=(const StatusGrpcClient&) = delete;
    LoginRsp Login(int uid, std::string token);
private:
    StatusGrpcClient();
    std::unique_ptr<StatusConPool> pool_;
};

#endif //GATESERVER_STATUSGRPCCLIENT_H
