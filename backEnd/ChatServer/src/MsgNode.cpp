
#include "../include/MsgNode.h"

SendNode::SendNode(const char *msg, short max_len, short msg_id) : MsgNode(max_len + HEAD_TOTAL_LEN), _msg_id(msg_id) {
    // 先处理消息头
    // 先发送 id，转换为网络字节序
    short msg_id_network = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    memcpy(_data, &msg_id_network, HEAD_ID_LEN);
    // 再发送消息长度
    short msg_len_network = boost::asio::detail::socket_ops::host_to_network_short(max_len);
    memcpy(_data+ HEAD_ID_LEN,&msg_len_network,HEAD_DATA_LEN);
    // 最后处理消息内容
    memcpy(_data + HEAD_TOTAL_LEN, msg, max_len);
}

RecNode::RecNode(short max_len, short msg_id) : MsgNode(max_len), _msg_id(msg_id) {
}
