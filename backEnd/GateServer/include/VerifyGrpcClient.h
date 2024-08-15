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

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class VerifyGrpcClient : public SingleTon<VerifyGrpcClient> {
    friend class SingleTon<VerifyGrpcClient>;
public:
    // 获取验证码
    GetVerifyRsp getVerifyCode(std::string email);
private:
    VerifyGrpcClient();
    // 用于通信
    std::unique_ptr<VerifyService::Stub> _stub;
};
#endif //GATESERVER_VERIFYGRPCCLIENT_H
