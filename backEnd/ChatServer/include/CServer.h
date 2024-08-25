/**
 *  FileName: CServer.h
 *  CreateTime: 2024/8/24 14:07
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_CSERVER_H
#define CHATSERVER_CSERVER_H

#include <boost/asio.hpp>
#include <map>

using tcp = boost::asio::ip::tcp;
class CSession;
class CServer {
public:
    CServer(boost::asio::io_context &ioContext, short port);

    ~CServer();

    void clearSession(std::string uuid);

private:
    void startAccept();

    void handleAccept(std::shared_ptr<CSession> CSession, const boost::system::error_code &error);

    boost::asio::io_context &_io_context;
    short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
};

#endif //CHATSERVER_CSERVER_H
