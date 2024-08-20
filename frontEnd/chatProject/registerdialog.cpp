#include "registerdialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

#include "httpMgr.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);

    ui->lab_errTip->setProperty("state", "normal");
    repolish(ui->lab_errTip);
    ui->edit_pwd->setEchoMode(QLineEdit::Password);
    ui->edit_confirm->setEchoMode(QLineEdit::Password);

    initHttpHandlers();
    connect(HttpMgr::getInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
}

RegisterDialog::~RegisterDialog() {
    qDebug() << "RegisterDialog destructor";
    delete ui;
}

void RegisterDialog::on_btn_getCode_clicked() {
    // 获取邮箱信息
    auto email = ui->edit_email->text();
    // 正则表达式验证邮箱是否合法
    static QRegularExpression email_regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    auto email_match = email_regex.match(email).hasMatch();
    if (email_match) {
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_verifycode"), json_obj,
                                            ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    } else {
        showTip(tr("请输入正确的邮箱"), true);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), true);
        return;
    }
    // 对响应的 json 进行解析
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // 解析失败
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), true);
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 处理逻辑
    _handlers[id](jsonObj);
    return;
}

void RegisterDialog::showTip(const QString& tip, bool is_error) {
    if (is_error) {
        ui->lab_errTip->setProperty("state", "err");
    } else {
        ui->lab_errTip->setProperty("state", "normal");
    }

    ui->lab_errTip->setText(tip);
    repolish(ui->lab_errTip);
}

void RegisterDialog::initHttpHandlers() {
    // 获取验证码
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), false);
        qDebug() << "VerifyCode send succeed, email is " << email;
    });
    // 注册请求
    _handlers.insert(ReqId::ID_REG_USER,[this](QJsonObject json_obj){
        int error = json_obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        auto email = json_obj["email"].toString();
        showTip(tr("用户注册成功"), false);
        qDebug()<< "Register succeed, email is " << email ;
    });
}

void RegisterDialog::on_btn_ok_clicked() {
    // TODO 加入校验逻辑
    if (ui->edit_user->text().isEmpty()) {
        showTip(tr("请输入用户名"), true);
        return;
    }
    if (ui->edit_email->text().isEmpty()) {
        showTip(tr("请输入正确的邮箱"), true);
        return;
    }
    if (ui->edit_pwd->text().isEmpty()) {
        showTip(tr("请输入密码"), true);
        return;
    }
    if (ui->edit_confirm->text() != ui->edit_pwd->text()) {
        showTip(tr("两次密码输入不一致"), true);
        return;
    }
    if (ui->edit_verify->text().isEmpty()) {
        showTip(tr("请输入验证码"), true);
        return;
    }

    // 发送 http post 请求
    QJsonObject json_obj;
    json_obj["user"] = ui->edit_user->text();
    json_obj["email"] = ui->edit_email->text();
    json_obj["password"] = ui->edit_pwd->text();
    json_obj["confirm"] = ui->edit_confirm->text();
    json_obj["verifycode"] = ui->edit_verify->text();
    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix+"/user_register"),json_obj,ReqId::ID_REG_USER,Modules::REGISTERMOD);
}
