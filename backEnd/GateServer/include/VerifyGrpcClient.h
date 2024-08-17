/**
 *  FileName: VerifyGrpcClient.h
 *  CreateTime: 2024/8/15 16:38
 *  Description: 验证码获取层
 *  Author: ACAね
*/
#ifndef GATESERVER_VERIFYGRPCCLIENT_H
#define GATESERVER_VERIFYGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include "../proto/message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include <atomic>
#include <queue>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port);

    ~RPConPool();
    /**
     * 关闭连接池，停止连接的分配
     */
    void close();
    /**
     * 从连接池中获取一个连接
     * @return 连接
     */
    std::unique_ptr<VerifyService::Stub> getConnection();
    /**
     * 将使用完的连接归还到连接池
     * @param context 要归还的连接
     */
    void returnConnection(std::unique_ptr<VerifyService::Stub> context);

private:
    // 用于指示连接池是否停止
    std::atomic<bool> _b_stop;
    size_t _pool_size;
    // gRPC服务器的主机名
    std::string _host;
    std::string _port;
    // 连接队列
    std::queue<std::unique_ptr<VerifyService::Stub>> _connections;
    std::mutex _mutex;
    std::condition_variable _cond;
};

class VerifyGrpcClient : public SingleTon<VerifyGrpcClient> {
    friend class SingleTon<VerifyGrpcClient>;

public:
    // 获取验证码
    GetVerifyRsp getVerifyCode(std::string email);

private:
    VerifyGrpcClient();
    // 连接池对象
    std::unique_ptr<RPConPool> _pool;

    // 用于通信
//    std::unique_ptr<VerifyService::Stub> _stub;
};

#endif //GATESERVER_VERIFYGRPCCLIENT_H
