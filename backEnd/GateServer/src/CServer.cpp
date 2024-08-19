/**
 *  FileName: CServer.cpp
 *  CreateTime: 2024/8/13 14:16
 *  Description: 
 *  Author: ACAね
*/
#include <iostream>
#include "../include/CServer.h"
#include "../include/AsioIOServicePool.h"
#include<spdlog/spdlog.h>
CServer::CServer(net::io_context &ioc, unsigned short port_num) : _ioc(ioc),
                                                                  _acceptor(ioc, tcp::endpoint(tcp::v4(), port_num)) {
    spdlog::info( "GateServer started, listen on port: {}",port_num) ;
}

void CServer::start() {
    auto self = shared_from_this();
    auto &io_context = AsioIOServicePool::getInstance()->getIOService();
    // 创建新连接
    auto new_con = std::make_shared<HttpConnection>(io_context);
    // 异步接收连接
    _acceptor.async_accept(new_con->getSocket(), [self, new_con](boost::system::error_code ec) {
        try {
            if (ec) {
                // 放弃该连接，继续监听
                self->start();
                return;
            }
            // 处理连接
            new_con->start();
            // 继续监听
            self->start();
        } catch (std::exception &e) {
            spdlog::error( "Exception in CServer::start,which is {}",e.what() );
            // 重新监听
            self->start();
        }
    });
}
