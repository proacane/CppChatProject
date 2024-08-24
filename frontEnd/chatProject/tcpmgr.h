#ifndef TCPMGR_H
#define TCPMGR_H
/*****************************************************************************
 * @file         tcpmgr.h
 * @brief
 * @description  Tcp 管理类，进行 Tcp 的连接
 * @author       ACA
 * @date         2024/08/24
 * @history
 *****************************************************************************/
#include <QObject>
#include <QTcpSocket>

#include "global.h"
#include "singleton.h"

class TcpMgr : public QObject, public SingleTon<TcpMgr>,public std::enable_shared_from_this<TcpMgr> {
    Q_OBJECT
    friend class SingleTon<TcpMgr>;
  public:
    ~TcpMgr();
private:
    explicit TcpMgr(QObject* parent = nullptr);
    void initHandlers();
    void handleMsg(ReqId id,int len,QByteArray data);
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    // 本次消息是否未接受完，true表示未接收完
    bool _b_rec_pending;
    // 消息 id
    quint16 _message_id;
    // 消息长度
    quint16 _message_len;
    // 存储请求处理函数
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;

public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId reqId, QString data);
  signals:
    void sig_connect_success(bool bsuccess);
    void sig_send_data(ReqId reqId, QString data);
    void sig_swich_chatdlg();
    void sig_login_failed(int err);
};

#endif  // TCPMGR_H
