/**
 * @FileName MsgNode.h
 * @CreateTime 2024/7/29 12:54
 * @Author ACA
 * @Description 进行发送与接收的消息结点
 */
#ifndef LOGICSERVER_MSGNODE_H
#define LOGICSERVER_MSGNODE_H

#include <boost/asio.hpp>
#include <iostream>
#include "const.h"

/**
 * 基类结点
 */
class MsgNode {
public:
    MsgNode(short max_len) : _total_len(max_len), _cur_len(0) {
        _data = new char[_total_len + 1]();
        _data[_total_len] = '\0';
    }

    ~MsgNode() {
        std::cout << "MsgNode destructor" << std::endl;
        delete[] _data;
    }

    /**
     * 清除结点内容
     */
    void clear() {
        memset(_data, 0, _total_len);
        _cur_len = 0;
    }

    // 消息首地址
    char *_data;
    short _cur_len;
    short _total_len;
};

/**
 * 发送结点，采用 tlv 格式
 */
class SendNode : public MsgNode {
public:
    /**
     *
     * @param msg 消息内容
     * @param max_len 消息长度
     * @param msg_id 消息 id
     */
    SendNode(const char *msg, short max_len, short msg_id);

    short getId() const {
        return _msg_id;
    }

private:
    short _msg_id;
};

/**
 * 接收结点，采用 tlv 格式
 */
class RecNode : public MsgNode {
public:
    RecNode(short max_len, short msg_id);

    short getId() const {
        return _msg_id;
    }

private:
    short _msg_id;
};

#endif //LOGICSERVER_MSGNODE_H
