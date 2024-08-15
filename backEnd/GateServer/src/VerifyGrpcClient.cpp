/**
 *  FileName: VerifyGrpcClient.cpp
 *  CreateTime: 2024/8/15 16:38
 *  Description: 
 *  Author: ACAね
*/

#include "../include/VerifyGrpcClient.h"
#include "../include/ConfigMgr.h"

GetVerifyRsp VerifyGrpcClient::getVerifyCode(std::string email) {
    ClientContext context;
    GetVerifyRsp reply;
    GetVerifyReq request;
    request.set_email(email);
    Status status = _stub->GetVerifyCode(&context, request, &reply);
    if (status.ok()) {
        reply.set_error(ErrorCodes::Success);
        return reply;
    } else {
        reply.set_error(ErrorCodes::RPCFailed);
        return reply;
    }
}

VerifyGrpcClient::VerifyGrpcClient() {
    ConfigMgr gCfgMgr;
    std::string gate_port_str = gCfgMgr["VerifyServer"]["Port"];
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:" + gate_port_str,
                                                           grpc::InsecureChannelCredentials());
    _stub = VerifyService::NewStub(channel);
}
