#include "registerdialog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include "ui_registerdialog.h"
#include "httpMgr.h"

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);

    ui->lab_errTip->setProperty("state", "normal");
    repolish(ui->lab_errTip);
    ui->edit_pwd->setEchoMode(QLineEdit::Password);
    ui->edit_confirm->setEchoMode(QLineEdit::Password);

    initHttpHandlers();
    connect(HttpMgr::getInstance().get(),&HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);
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
        HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix+"/get_varifycode"), json_obj,
                                            ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    } else {
        showTip(tr("邮箱地址不正确"), true);
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
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
        qDebug() << "email is " << email;
    });
}
