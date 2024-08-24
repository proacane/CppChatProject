#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include"global.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    Ui::LoginDialog *ui;
    int _uid;
    QString _token;
    // 初始化 http 请求处理函数
    void initHttpHandlers();
    // 存储对不同请求的处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    bool checkUserValid();
    bool checkPassValid();
    // 显示信息
    void showTip(const QString& tip,bool is_error);
    QMap<TipErr, QString> _tip_errs;
    void addTipErr(TipErr err,const QString& tips);
    void delTipErr(TipErr err);
    bool enableBtn(bool);
signals:
    // 跳转到注册界面
    void switch_to_register();
    // 跳转到忘记密码界面
    void switchReset();
    // 连接到聊天服务器
    void sig_connect_tcp(ServerInfo si);
private slots:
    // 忘记密码
    void slot_forget_password();
    void on_btn_login_clicked();
    // 处理 http响应
    void slot_login_mod_finish(ReqId id,QString res, ErrorCodes err);
    // 处理 tcp 连接成功
    void slot_tcp_connect_finish(bool success);
    // 处理 tcp 连接失败
    void slot_login_failed(int err);
};

#endif // LOGINDIALOG_H
