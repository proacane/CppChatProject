/**
 *  FileName: CServer.cpp
 *  CreateTime: 2024/8/13 14:16
 *  Description: 
 *  Author: ACAね
*/
#include <iostream>
#include "../include/CServer.h"

CServer::CServer(net::io_context &ioc, unsigned short port_num) : _ioc(ioc), _socket(ioc),
                                                                  _acceptor(ioc, tcp::endpoint(tcp::v4(), port_num)) {

}

void CServer::start() {
    auto self = shared_from_this();
    // 异步接收连接
    _acceptor.async_accept(_socket,[self](boost::system::error_code ec){
        try {
            if(ec){
                // 放弃该连接，继续监听
                self->start();
                return;
            }
            // 创建 HttpConnection 类管理新连接,将 _socket 内部数据转移给HttpConnection管理
            std::make_shared<HttpConnection>(std::move(self->_socket))->start();
            // 继续监听
            self->start();
        } catch (std::exception&e) {
            std::cerr<<"Exception in CServer::start,which is "<<e.what()<<std::endl;
            // 重新监听
            self->start();
        }
    });
}
