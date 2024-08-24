#include "httpmgr.h"

HttpMgr::~HttpMgr() {
    qDebug() << "HttpMgr destructor";
}

void HttpMgr::postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod) {
    // 创建一个 post 请求，设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    // 通过 url 构建请求
    QNetworkRequest request(url);
    // 设置请求体类型和请求体长度
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 发送请求并处理响应
    auto self = shared_from_this();
    QNetworkReply* reply = _manager.post(request, data);
    // 设置信号和槽，等待发送完成异步执行
    connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod] {
        if (reply->error() != QNetworkReply::NoError) {
            // 有错误
            qDebug() << reply->errorString();
            // 发送信号，表示响应处理完毕
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }
        // 读回请求
        QString res = reply->readAll();
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

HttpMgr::HttpMgr() {
    qDebug() << "HttpMgr constructor";
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod) {
    // 发送信号通知指定模块http响应结束
    if (mod == Modules::REGISTERMOD) {
        emit sig_reg_mod_finish(id, res, err);
    } else if (mod == Modules::RESETMOD) {
        emit sig_reset_mod_finish(id, res, err);
    } else if (mod == Modules::LOGINMOD) {
        emit sig_login_mod_finish(id, res, err);
    }
}
