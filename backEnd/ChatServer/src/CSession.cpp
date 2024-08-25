/**
 *  FileName: CSession.cpp
 *  CreateTime: 2024/8/24 14:16
 *  Description: 
 *  Author: ACAね
*/
#include "../include/CSession.h"
#include "../include/CServer.h"
#include <spdlog/spdlog.h>
#include "../include/LogicSystem.h"

CSession::CSession(boost::asio::io_context &ioc, CServer *server) : _socket(ioc), _b_close(false), _b_head_parse(false),
                                                                    _server(server) {
    memset(_data, 0, MAX_LENGTH);
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    _uuid = boost::uuids::to_string(a_uuid);
    _rec_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession() {
    spdlog::info("CSession destructor");
}

tcp::socket &CSession::getSocket() {
    return _socket;
}

std::string &CSession::getUUID() {
    return _uuid;
}

void CSession::start() {
    // 先读包头
    asyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::asyncReadHead(int total_len) {
    auto self = shared_from_this();
    asyncReadFull(HEAD_TOTAL_LEN, [this, self](const boost::system::error_code &ec, std::size_t bytesTransferred) {
        try {
            if (ec) {
                spdlog::warn("Handle Read head failed, error is {}", ec.what());
                close();
                _server->clearSession(_uuid);
                return;
            }
            if (bytesTransferred < HEAD_TOTAL_LEN) {
                spdlog::info("Read head length does not match, [{}] bytes read, need to read [{}] bytes",
                             bytesTransferred, HEAD_TOTAL_LEN);
                close();
                _server->clearSession(_uuid);
                return;
            }

            // 解析包头
            _rec_head_node->clear();
            memcpy(_rec_head_node->_data, _data, HEAD_TOTAL_LEN);
            // 获取消息id
            short msg_id = 0;
            memcpy(&msg_id, _rec_head_node->_data, HEAD_ID_LEN);
            // 网络字节序转换为本地字节序
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id); // NOLINT(*-narrowing-conversions)
            spdlog::info("msg id is {}", msg_id);
            // 获取消息长度
            short msg_len = 0;
            memcpy(&msg_len, _rec_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(
                    msg_len); // NOLINT(*-narrowing-conversions)
            spdlog::info("msg len is {}", msg_len);
            // 判断消息长度是否合法
            if (msg_len > MAX_LENGTH) {
                spdlog::warn("Invalid data length");
                close();
                _server->clearSession(_uuid);
                return;
            }

            // 构造消息体，开始读取
            _rec_msg_node = std::make_shared<RecNode>(msg_len, msg_id);
            asyncReadBody(msg_len);
        } catch (const std::exception &e) {
            spdlog::error("Exception: {}", e.what());
            close();
            _server->clearSession(_uuid);
        }
    });
}

void CSession::asyncReadFull(std::size_t maxLength,
                             std::function<void(const boost::system::error_code &, std::size_t)> handler) {
    memset(_data, 0, MAX_LENGTH);
    asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len,
                            std::function<void(const boost::system::error_code &, std::size_t)> handler) {
    auto self = shared_from_this();
    _socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len),
                            [read_len, total_len, handler, self](const boost::system::error_code &ec,
                                                                 std::size_t bytesTransferred) {
                                if (ec) {
                                    handler(ec, read_len + bytesTransferred);
                                    return;
                                }
                                // 读取字节数够了就回调
                                if (read_len + bytesTransferred >= total_len) {
                                    handler(ec, read_len + bytesTransferred);
                                    return;
                                }
                                // 读的字节数不够就继续读
                                self->asyncReadLen(read_len + bytesTransferred, total_len, handler);
                            });
}


void CSession::close() {
    _socket.close();
    _b_close = true;
}

void CSession::asyncReadBody(int total_len) {
    auto self = shared_from_this();
    asyncReadFull(total_len,
                  [self, this, total_len](const boost::system::error_code &ec, std::size_t bytesTransferred) {
                      try {
                          if (ec) {
                              spdlog::warn("Handle read body failed, error is {}", ec.what());
                              close();
                              _server->clearSession(_uuid);
                              return;
                          }
                          if (bytesTransferred < total_len) {
                              spdlog::info("Read body length does not match, [{}] bytes read, need to read [{}] bytes",
                                           bytesTransferred, total_len);
                              close();
                              _server->clearSession(_uuid);
                              return;
                          }

                          // 给包体赋值
                          memcpy(_rec_msg_node->_data, _data, bytesTransferred);
                          _rec_msg_node->_cur_len += bytesTransferred;
                          _rec_msg_node->_data[_rec_msg_node->_total_len] = '\0';
                          spdlog::info("Receive data is {}", _rec_msg_node->_data);

                          // 投递到消息队列进行处理
                          LogicSystem::getInstance()->postMsgQue(std::make_shared<LogicNode>(self, _rec_msg_node));
                          // 继续监听下一个消息
                          asyncReadHead(HEAD_TOTAL_LEN);
                      } catch (const std::exception &e) {
                          spdlog::error("Exception: {}", e.what());
                          close();
                          _server->clearSession(_uuid);
                      }
                  });
}

void CSession::send(char *msg, short max_length, short msg_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    int send_size = _send_queue.size();
    if (send_size > MAX_SEND_QUE) {
        spdlog::info("Session {} send queue is fulled, size is {}", _uuid, send_size);
        return;
    }

    _send_queue.push(std::make_shared<SendNode>(msg, max_length, msg_id));
    if (send_size > 0) {
        // 接着发之前的
        return;
    }
    auto &msg_node = _send_queue.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
                             std::bind(&CSession::handleWrite, this, std::placeholders::_1, shared_from_this()));
}

void CSession::send(std::string msg, short msg_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    int send_size = _send_queue.size();
    if (send_size > MAX_SEND_QUE) {
        spdlog::info("Session {} send queue is fulled, size is {}", _uuid, send_size);
        return;
    }

     _send_queue.push(std::make_shared<SendNode>(msg.c_str(), msg.size(), msg_id));
    if (send_size > 0) {
        // 接着发之前的
        return;
    }
    auto &msg_node = _send_queue.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
                             std::bind(&CSession::handleWrite, this, std::placeholders::_1, shared_from_this()));
}

void CSession::handleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self) {
    try {
        if(error){
            spdlog::warn("Handle write failed, error is {}",error.what());
            close();
            _server->clearSession(_uuid);
        }
        std::lock_guard<std::mutex> lock(_mutex);
        _send_queue.pop();
        if(!_send_queue.empty()){
            auto &msg_node = _send_queue.front();
            boost::asio::async_write(_socket, boost::asio::buffer(msg_node->_data, msg_node->_total_len),
                                     std::bind(&CSession::handleWrite, this, std::placeholders::_1, shared_from_this()));
        }
    } catch (const std::exception &e) {
        spdlog::error("Exception is {}",e.what());
        close();
        _server->clearSession(_uuid);
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecNode> recNode) : _session(session),
                                                                                            _recnode(recNode) {

}
