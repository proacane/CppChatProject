/**
 *  FileName: CServer.cpp
 *  CreateTime: 2024/8/24 14:07
 *  Description: 
 *  Author: ACAね
*/
#include "../include/CServer.h"
#include <spdlog/spdlog.h>
#include "../include/CSession.h"
#include "../include/AsioIOServicePool.h"
#include "../include/UserMgr.h"
CServer::CServer(boost::asio::io_context &ioContext, short port) : _io_context(ioContext), _port(port),
                                                                   _acceptor(_io_context,
                                                                             tcp::endpoint(tcp::v4(), port)) {
    spdlog::info("Chat Server start success, listening on port {}", port);
    startAccept();
}

void CServer::startAccept() {
    auto &io_context = AsioIOServicePool::getInstance()->getIOService();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    _acceptor.async_accept(new_session->getSocket(),
                           std::bind(&CServer::handleAccept, this, new_session, std::placeholders::_1));
}

void CServer::handleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &error) {
    if(!error){
        new_session->start();
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.insert(std::make_pair(new_session->getSessionId(), new_session));
    }else{
        spdlog::warn("session accept failed, error is {}",error.what());
    }
    startAccept();
}

CServer::~CServer() {
    spdlog::info("CServer destructor");
}

void CServer::clearSession(const std::string session_id) {
    if (_sessions.find(session_id) != _sessions.end()) {
        // 移除用户和session的关联
        UserMgr::getInstance()->rmvUserSession(_sessions[session_id]->getUserId());
    }
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.erase(session_id);
}
