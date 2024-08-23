#include "resetdialog.h"

#include <QRegularExpression>
#include<QThread>
#include "httpmgr.h"
#include "ui_resetdialog.h"

ResetDialog::ResetDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ResetDialog) {
    ui->setupUi(this);

    connect(ui->edit_user, &QLineEdit::editingFinished, this, [this]() { checkUserValid(); });
    connect(ui->edit_email, &QLineEdit::editingFinished, this, [this]() { checkEmailValid(); });
    connect(ui->edit_pwd, &QLineEdit::editingFinished, this, [this]() { checkPassValid(); });
    connect(ui->edit_verify, &QLineEdit::editingFinished, this, [this]() { checkVerifyValid(); });
    // 连接reset相关信号和注册处理回调
    initHttpHandlers();
    connect(HttpMgr::getInstance().get(), &HttpMgr::sig_reset_mod_finish, this, &ResetDialog::slotResetModFinish);
}

ResetDialog::~ResetDialog() {
    delete ui;
}

void ResetDialog::showTip(const QString& tip, bool is_error) {
    if (is_error) {
        ui->lab_errTip->setProperty("state", "err");
    } else {
        ui->lab_errTip->setProperty("state", "normal");
    }

    ui->lab_errTip->setText(tip);
    repolish(ui->lab_errTip);
}

bool ResetDialog::checkUserValid() {
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

bool ResetDialog::checkEmailValid() {
    // 验证邮箱的地址正则表达式
    auto email = ui->edit_email->text();
    bool match = email_regex.match(email).hasMatch();  // 执行正则表达式匹配
    if (!match) {
        // 提示邮箱不正确
        addTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    delTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool ResetDialog::checkPassValid() {
    auto pass = ui->edit_pwd->text();
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

bool ResetDialog::checkVerifyValid() {
    auto pass = ui->edit_verify->text();
    if (pass.isEmpty()) {
        addTipErr(TipErr::TIP_VARIFY_ERR, tr("请输入验证码"));
        return false;
    }
    delTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void ResetDialog::addTipErr(TipErr err, const QString& tips) {
    _tip_errs[err] = tips;
    showTip(tips, true);
}

void ResetDialog::delTipErr(TipErr err) {
    _tip_errs.remove(err);
    if (_tip_errs.empty()) {
        ui->lab_errTip->clear();
        return;
    }
    // 还有错误就继续显示
    showTip(_tip_errs.first(), true);
}

void ResetDialog::initHttpHandlers() {
    // 获取验证码请求
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject json_obj) {
        int error = json_obj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        // auto email = json_obj["email"].toString();
        showTip(tr("验证码已发送到邮箱，注意查收"), false);
        // qDebug()<< "email is " << email ;
    });

    // 重置密码请求
    _handlers.insert(ReqId::ID_RESET_PWD, [this](QJsonObject json_obj) {
        int error = json_obj["error"].toInt();
        if(error == ErrorCodes::PasswordSame){
            showTip(tr("新密码不能与原密码相同"), true);
            return;
        }else if(error == ErrorCodes::VerifyExpired){
            showTip(tr("验证码过期或失效"), true);
            return;
        }
        else if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        // auto email = json_obj["email"].toString();
        showTip(tr("重置成功,正在跳转登录界面"), false);
        // qDebug()<< "email is " << email ;
        qDebug() << "user uuid is " << json_obj["uuid"].toInt();
        QThread::sleep(3);
        emit sigSwitchLogin();
    });
}

void ResetDialog::on_btn_getCode_clicked() {
    // 获取邮箱信息
    auto email = ui->edit_email->text();
    // 正则表达式验证邮箱是否合法
    auto email_match = email_regex.match(email).hasMatch();
    if (email_match) {
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/get_verifycode"), json_obj,
                                            ReqId::ID_GET_VARIFY_CODE, Modules::RESETMOD);
    } else {
        showTip(tr("请输入正确的邮箱"), true);
    }
}

void ResetDialog::slotResetModFinish(ReqId id, QString res, ErrorCodes err) {
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), true);
        return;
    }
    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json解析错误
    if (jsonDoc.isNull()) {
        showTip(tr("json解析错误"), true);
        return;
    }
    // json解析错误
    if (!jsonDoc.isObject()) {
        showTip(tr("json解析错误"), true);
        return;
    }
    // 调用对应的逻辑,根据id回调
    _handlers[id](jsonDoc.object());
}

void ResetDialog::on_btn_ok_clicked() {
    if (!checkUserValid() || !checkEmailValid() || !checkPassValid() || !checkVerifyValid()) {
        return;
    }

    // 发送重置密码的请求
    QJsonObject json_obj;
    json_obj["user"] = ui->edit_user->text();
    json_obj["email"] = ui->edit_email->text();
    json_obj["password"] = hashPassword(ui->edit_pwd->text());
    json_obj["verifycode"] = ui->edit_verify->text();
    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/reset_pwd"), json_obj, ReqId::ID_RESET_PWD,
                                        Modules::RESETMOD);
}

void ResetDialog::on_btn_cancel_clicked() {
    emit sigSwitchLogin();
}
