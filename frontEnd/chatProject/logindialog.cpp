#include "logindialog.h"

#include "httpmgr.h"
#include "tcpmgr.h"
#include "ui_logindialog.h"
LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    connect(ui->btn_register, &QPushButton::clicked, this, &LoginDialog::switch_to_register);
    ui->edit_password->setEchoMode(QLineEdit::Password);

    ui->lab_forgetPwd->setState("normal", "hover", "", "selected", "selected_hover", "");
    ui->lab_forgetPwd->setCursor(Qt::PointingHandCursor);
    // 忘记密码跳转
    connect(ui->lab_forgetPwd, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_password);
    initHttpHandlers();
    // http 请求处理
    connect(HttpMgr::getInstance().get(), &HttpMgr::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
    // 连接tcp连接请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::getInstance().get(), &TcpMgr::slot_tcp_connect);
    // 连接tcp管理者发出的连接成功信号
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_connect_success, this, &LoginDialog::slot_tcp_connect_finish);
    // 连接tcp管理者发出的登陆失败信号
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_login_failed, this, &LoginDialog::slot_login_failed);
}

LoginDialog::~LoginDialog() {
    qDebug() << "LoginDialog destructor";
    delete ui;
}

void LoginDialog::initHttpHandlers() {
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject json_obj) {
        int error = json_obj["error"].toInt();
        if (error == ErrorCodes::UserPasswordError) {
            showTip(tr("用户名不存在或密码错误"), true);
            enableBtn(true);
            return;
        } else if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            enableBtn(true);
            return;
        }
        auto user = json_obj["user"].toString();
        // 发送信号通知tcpMgr发送长链接
        ServerInfo si;
        si.Uid = json_obj["uid"].toInt();
        si.Host = json_obj["host"].toString();
        si.Port = json_obj["port"].toString();
        si.Token = json_obj["token"].toString();
        _uid = si.Uid;
        _token = si.Token;
        qDebug() << "user is " << user << " uid is " << si.Uid << " host is " << si.Host << " Port is " << si.Port
                 << " Token is " << si.Token;

        emit sig_connect_tcp(si);
    });
}

void LoginDialog::slot_forget_password() {
    // qDebug() << "slot forget pwd";
    emit switchReset();
}

void LoginDialog::showTip(const QString& tip, bool is_error) {
    if (is_error) {
        ui->lab_errTip->setProperty("state", "err");
    } else {
        ui->lab_errTip->setProperty("state", "normal");
    }

    ui->lab_errTip->setText(tip);
    repolish(ui->lab_errTip);
}

bool LoginDialog::checkUserValid() {
    if (ui->edit_user->text().isEmpty()) {
        addTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    } else if (ui->edit_user->text().size() < 6 || ui->edit_user->text().size() > 16) {
        addTipErr(TipErr::TIP_USER_ERR, tr("用户名应在6-15位"));
        return false;
    }
    delTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool LoginDialog::checkPassValid() {
    auto pass = ui->edit_password->text();
    if (pass.length() < 8 || pass.length() > 17) {
        // 提示长度不准确
        addTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为8~16"));
        return false;
    }

    bool match = password_regex.match(pass).hasMatch();
    if (!match) {
        // 提示字符非法
        addTipErr(TipErr::TIP_PWD_ERR, tr("密码含有非法字符"));
        return false;
    }
    delTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

void LoginDialog::addTipErr(TipErr err, const QString& tips) {
    _tip_errs[err] = tips;
    showTip(tips, true);
}

void LoginDialog::delTipErr(TipErr err) {
    _tip_errs.remove(err);
    if (_tip_errs.empty()) {
        ui->lab_errTip->clear();
        return;
    }
    // 还有错误就继续显示
    showTip(_tip_errs.first(), true);
}

void LoginDialog::on_btn_login_clicked() {
    if (!checkUserValid() || !checkPassValid()) {
        return;
    }

    enableBtn(false);
    // 发送 post 请求
    auto user_name = ui->edit_user->text();
    auto password = ui->edit_password->text();
    QJsonObject json_obj;
    json_obj["user"] = user_name;
    json_obj["password"] = hashPassword(password);
    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_login"), json_obj, ReqId::ID_LOGIN_USER,
                                        Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), true);
        enableBtn(true);
        return;
    }
    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json解析错误
    if (jsonDoc.isNull()) {
        showTip(tr("json解析错误"), false);
        return;
    }
    // json解析错误
    if (!jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        return;
    }
    // 调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());
    return;
}

void LoginDialog::slot_tcp_connect_finish(bool success) {
    if (success) {
        showTip(tr("聊天服务连接成功，正在登录..."), false);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;
        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        // 发送tcp请求给chat server
        emit TcpMgr::getInstance() -> sig_send_data(ReqId::ID_CHAT_LOGIN, jsonString);
    } else {
        showTip(tr("网络异常"), false);
        enableBtn(true);
    }
}

void LoginDialog::slot_login_failed(int err) {
    QString result = QString("登录失败, err is %1").arg(err);
    showTip(result, true);
    enableBtn(true);
}

bool LoginDialog::enableBtn(bool enabled) {
    ui->btn_login->setEnabled(enabled);
    ui->btn_register->setEnabled(enabled);
    return true;
}
