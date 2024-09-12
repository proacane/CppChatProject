#include "tcpmgr.h"

#include <QJsonDocument>

#include "usermgr.h"

TcpMgr::TcpMgr(QObject* parent) :
    QObject{parent}, _host(""), _port(0), _b_rec_pending(false), _message_id(0), _message_len(0) {
    // 连接成功后的处理
    connect(&_socket, &QTcpSocket::connected, this, [&] {
        // qDebug() << "Connected to chat server: ";
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

            qDebug() << "Receive message is :";
            qDebug().noquote() << message_body;
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
        qDebug().noquote() << data;
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
            qDebug() << "Login Failed, err is Json Parse Err: " << err;
            emit sig_login_failed(err);
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Login Failed, err is " << err;
            emit sig_login_failed(err);
            return;
        }
        // 登录后缓存其它信息
        auto uid = json_obj["uid"].toInt();
        auto name = json_obj["name"].toString();
        auto nick = json_obj["nick"].toString();
        auto avatar = json_obj["avatar"].toString();
        auto gender = json_obj["gender"].toInt();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, avatar, gender);

        UserMgr::getInstance()->setUserInfo(user_info);
        UserMgr::getInstance()->setToken(json_obj["token"].toString());
        // 获取申请列表
        if(json_obj.contains("apply_list")){
            UserMgr::getInstance()->appendApplyList(json_obj["apply_list"].toArray());
        }
        // 切换到聊天窗口
        emit sig_swich_chatdlg();
    });

    // 查询用户
    _handlers.insert(ReqId::ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data) {
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().noquote() << data;

        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Search user Failed, err is Json Parse Err: " << err;
            emit sig_user_search_failed(err);
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Search user Failed, err is " << err;
            emit sig_user_search_failed(err);
            return;
        }

        // 创建用户信息
        auto search_info = std::make_shared<SearchInfo>(json_obj["uid"].toInt(), json_obj["name"].toString(),
                                                        json_obj["nick"].toString(), json_obj["desc"].toString(),
                                                        json_obj["gender"].toInt(), json_obj["avatar"].toString());
        // 通知 SearchList
        emit sig_user_search(search_info);
    });

    // TODO 发送好友请求后，返回给自己的响应
    _handlers.insert(ReqId::ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data) {
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().noquote() << data;

        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Apply friend Failed, err is Json Parse Err: " << err;
            emit sig_friend_apply_failed(err);
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Apply friend Failed, err is " << err;
            emit sig_friend_apply_failed(err);
            return;
        }
    });

    // 收到了好友请求
    _handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data) {
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().noquote() << data;

        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Receive Apply friend Failed, err is Json Parse Err: " << err;
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Receive Apply friend Failed, err is " << err;
            return;
        }

        int from_uid = json_obj["applyuid"].toInt();
        QString name = json_obj["name"].toString();
        QString desc = json_obj["desc"].toString();
        QString icon = json_obj["avatar"].toString();
        QString nick = json_obj["nick"].toString();
        int gender = json_obj["gender"].toInt();
        auto apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, gender);
        // 接收好友请求
        emit sig_friend_apply(apply_info);
    });

    // 同意好友请求后的处理
    _handlers.insert(ReqId::ID_AUTH_FRIEND_RSP,[this](ReqId id, int len, QByteArray data){
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().noquote() << data;

        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth friend Failed, err is Json Parse Err: " << err;
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth friend Failed, err is " << err;
            return;
        }

        auto name = json_obj["name"].toString();
        auto nick = json_obj["nick"].toString();
        auto avatar = json_obj["avatar"].toString();
        auto gender = json_obj["gender"].toInt();
        auto uid = json_obj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, avatar, gender);
        emit sig_auth_rsp(rsp);
    });
    // 接收到了对方同意好友的请求
    _handlers.insert(ReqId::ID_NOTIFY_AUTH_FRIEND_REQ,[this](ReqId id, int len, QByteArray data){
        qDebug() << "Handle id is " << id << ", data is ";
        qDebug().noquote() << data;

        QJsonDocument json_doc = QJsonDocument::fromJson(data);
        if (json_doc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json_obj = json_doc.object();
        // json 必须包含 error
        if (!json_obj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth friend Failed, err is Json Parse Err: " << err;
            return;
        }

        int err = json_obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth friend Failed, err is " << err;
            return;
        }

        int from_uid = json_obj["fromuid"].toInt();
        QString name = json_obj["name"].toString();
        QString nick = json_obj["nick"].toString();
        QString avatar = json_obj["avatar"].toString();
        int gender = json_obj["gender"].toInt();
        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, avatar, gender);
        emit sig_add_auth_friend(auth_info);
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
    qDebug() << "Trying to connect to chat server, ip " << si.Host << ", port " << si.Port;
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
    qDebug() << "slot send data to tcp server is: ";
    qDebug().noquote() << dataBytes;
    _socket.write(send_data);
}
