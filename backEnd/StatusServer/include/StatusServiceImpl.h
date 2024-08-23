/**
 *  FileName: StatusServiceImpl.h
 *  CreateTime: 2024/8/23 15:49
 *  Description: 
 *  Author: ACAね
*/
#ifndef STATUSSERVER_STATUSSERVICEIMPL_H
#define STATUSSERVER_STATUSSERVICEIMPL_H

#include <grpcpp/grpcpp.h>
#include "../proto/message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

struct ChatServer {
    std::string host;
    std::string port;
    std::string name;
    int connect_count;
};

class StatusServiceImpl final : public StatusService::Service {
public:
    StatusServiceImpl();

    Status GetChatServer(ServerContext *context, const GetChatServerReq *request,
                         GetChatServerRsp *reply) override;

private:
    void insertToken(int uid, std::string token);

    ChatServer getChatServer();

    std::unordered_map<std::string, ChatServer> _servers;
    std::mutex _server_mutex;

    std::unordered_map<int, std::string> _tokens;
    std::mutex _token_mutex;
};

#endif //STATUSSERVER_STATUSSERVICEIMPL_H
