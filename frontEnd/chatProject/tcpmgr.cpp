#include "tcpmgr.h"

#include "usermgr.h"

#include <QJsonDocument>

TcpMgr::TcpMgr(QObject* parent) :
    QObject{parent}, _host(""), _port(0), _b_rec_pending(false), _message_id(0), _message_len(0) {
    // 连接成功后的处理
    connect(&_socket, &QTcpSocket::connected, this, [&] {
        qDebug() << "Connected to chat server: ";
        // 连接成功后发送信号
        emit sig_connect_success(true);
    });

    // 读事件就绪的处理
    connect(&_socket, &QTcpSocket::readyRead, this, [&] {
        // 读取所有数据到缓冲区
        _buffer.append(_socket.readAll());

        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Version::Qt_6_5);
        // forever 就是无限循环
        forever {
            if (!_b_rec_pending) {
                // 上次的已经解析完了，或者是第一次解析
                // 如果 buffer 的大小小于4，连包头都没读取完，跳过
                if (_buffer.size() < static_cast<int>(sizeof(quint16) * 2)) {
                    return;
                }

                // 先解析包头数据
                stream >> _message_id >> _message_len;
                // 包头数据移除
                _buffer = _buffer.mid(sizeof(quint16) * 2);

                qDebug() << "Message id is " << _message_id << ", Message length is " << _message_len;
            }

            // 处理包体
            if (_buffer.size() < _message_len) {
                // 读取长度比数据长度小
                _b_rec_pending = true;
                return;
            }
            _b_rec_pending = false;
            // 获取包体数据
            QByteArray message_body = _buffer.mid(0, _message_len);
            // 移除包体
            _buffer = _buffer.mid(_message_len);

            qDebug() << "Receive message is :" ;
            qDebug().noquote() <<message_body;
            // 处理消息
            handleMsg(ReqId(_message_id), _message_len, std::move(message_body));
        }
    });

    // 处理异常
    connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this,
            [&](QAbstractSocket::SocketError socketError) {
                Q_UNUSED(socketError)
                qDebug() << "Error:" << _socket.errorString();
                emit sig_login_failed(socketError);
            });

    // 处理连接断开
    connect(&_socket, &QTcpSocket::disconnected, this, [&] { qDebug() << "Disconnected from server"; });

    // 连接发送信号用来发送数据
    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    // 注册消息
    initHandlers();
}

void TcpMgr::initHandlers() {
    _handlers.insert(ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data) {
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().quote()<<data;
        // 转换为 json
        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err;
            emit sig_login_failed(err);
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Login Failed, err is " << err;
            emit sig_login_failed(err);
            return;
        }

        UserMgr::getInstance()->setUid(json_obj["uid"].toInt());
        UserMgr::getInstance()->setUserName(json_obj["user_name"].toString());
        UserMgr::getInstance()->setToken(json_obj["token"].toString());
        // 切换到聊天窗口
        emit sig_swich_chatdlg();
    });
}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data) {
    auto find_iter = _handlers.find(id);
    if (find_iter == _handlers.end()) {
        // 没找到
        qDebug() << "Not found id [" << id << "] to handle";
        return;
    }
    // 进行处理
    find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si) {
    qDebug() << "receive tcp connect signal";
    // 尝试连接到服务器
    qDebug() << "Trying to connect to chat server, ip "<<si.Host<<", port "<<si.Port;
    _host = si.Host;
    _port = si.Port.toUInt();
    _socket.connectToHost(si.Host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QString data) {
    // 向服务器发送数据
    uint16_t id = reqId;
    QByteArray dataBytes = data.toUtf8();
    // 计算数据长度
    quint16 len = static_cast<quint16>(data.size());
    // 创建发送数组
    QByteArray send_data;
    QDataStream out(&send_data, QIODevice::WriteOnly);
    // 使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);
    out.setVersion(QDataStream::Version::Qt_6_5);
    // 写入 id 和长度
    out << id << len;
    send_data.append(dataBytes);
    qDebug()<<"slot send data to tcp server is "<<data;
    _socket.write(send_data);
}
