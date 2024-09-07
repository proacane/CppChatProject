/**
 *  FileName: CSession.h
 *  CreateTime: 2024/8/24 14:16
 *  Description: 
 *  Author: ACAね
*/
#ifndef CHATSERVER_CSESSION_H
#define CHATSERVER_CSESSION_H

#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"

class CServer;
class LogicSystem;
class CSession:public std::enable_shared_from_this<CSession> {
    using tcp = boost::asio::ip::tcp;
public:
    CSession(boost::asio::io_context &ioc, CServer *server);

    ~CSession();

    tcp::socket &getSocket();

    std::string &getSessionId();

    void setUserId(int uid);
    int getUserId() const ;
    void start();

    // 发送
    void send(char *msg, short max_length, short msg_id);

    void send(std::string msg, short msg_id);

    void close();
    /**
     * 读取指定长度的消息体
     * @param total_len
     */
    void asyncReadBody(int length);
    /**
     * 读取指定长度的包头
     * @param total_len
     */
    void asyncReadHead(int total_len);

private:
    tcp::socket _socket;
    std::string _session_id;
    int _user_id;
    // 缓存区
    char *_data[MAX_LENGTH]{};
    CServer *_server;
    bool _b_close;
    // 发送队列
    std::queue<std::shared_ptr<SendNode>> _send_queue;
    std::mutex _mutex;
    // 接收的消息体
    std::shared_ptr<RecNode> _rec_msg_node;
    // 头部是否解析完
    bool _b_head_parse;
    // 接收的头部信息
    std::shared_ptr<MsgNode> _rec_head_node;
    /**
     * 读完指定字节或出现异常才会回调
     * @param maxLength 读取长度
     * @param handler 回调函数
     */
    void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code &, std::size_t)> handler);

    /**
     * 读完指定长度或出现异常才会回调
     * @param read_len 读取字节起始位置
     * @param bytesTransferred 读取总长度
     * @param handler 回调函数
     */
    void asyncReadLen(std::size_t read_len, std::size_t bytesTransferred,
                      std::function<void(const boost::system::error_code &, std::size_t)> handler);


    void handleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self);
};

class LogicNode{
    friend  class LogicSystem;
public:
    LogicNode(std::shared_ptr<CSession>,std::shared_ptr<RecNode>);
private:
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecNode> _recnode;
};

#endif //CHATSERVER_CSESSION_H
