/**
 *  FileName: CServer.h
 *  CreateTime: 2024/8/13 14:15
 *  Description: 绑定和监听连接
 *  Author: ACAね
*/
#ifndef GATESERVER_CSERVER_H
#define GATESERVER_CSERVER_H
#include <memory>
#include "const.h"
#include "HttpConnection.h"

class CServer : public std::enable_shared_from_this<CServer> {
public:
    CServer(net::io_context &ioc, unsigned short port_num);

    void start();

private:
//    tcp::socket _socket;
    net::io_context &_ioc;
    tcp::acceptor _acceptor;
};

#endif //GATESERVER_CSERVER_H
