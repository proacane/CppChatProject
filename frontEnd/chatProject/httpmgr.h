#pragma once
#ifndef HTTPMGR_H
#define HTTPMGR_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QUrl>
#include <memory>

#include "global.h"
#include "singleton.h"
class HttpMgr : public QObject, public SingleTon<HttpMgr>, public std::enable_shared_from_this<HttpMgr> {
    Q_OBJECT
    friend class SingleTon<HttpMgr>;
  public:
    ~HttpMgr();
    /**
     * @brief PostHttpReq 发送 post 请求
     * @param url 请求地址
     * @param json 请求数据
     * @param req_id 请求 id
     * @param mod 请求模块（类型）
     */
    void postHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

  private:
    HttpMgr();
    QNetworkAccessManager _manager;
  signals:
    /**
     * @brief sig_http_finish
     * @param id 请求 id
     * @param res 请求结果
     * @param err 请求代码
     * @param mod 请求模块（类型）
     */
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    /**
     * @brief sig_reg_mod_finish 发送给注册模块的信号
     * @param id 请求 id
     * @param res 请求结果
     * @param err 请求代码
     */
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    /**
     * @brief sig_reset_mod_finish 发送给重置密码模块
     * @param id
     * @param res
     * @param err
     */
    void sig_reset_mod_finish(ReqId id,QString res,ErrorCodes err);

    /**
     * @brief sig_login_mod_finish 发送给登录模块
     * @param id
     * @param res
     * @param err
     */
    void sig_login_mod_finish(ReqId id,QString res,ErrorCodes err);
  private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
};

#endif  // HTTPMGR_H
