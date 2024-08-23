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

    connect(ui->edit_user, &QLineEdit::editingFinished, this, [this]() { checkUserValid(); });
    connect(ui->edit_email, &QLineEdit::editingFinished, this, [this]() { checkEmailValid(); });
    connect(ui->edit_pwd, &QLineEdit::editingFinished, this, [this]() { checkPassValid(); });
    connect(ui->edit_confirm, &QLineEdit::editingFinished, this, [this]() { checkConfirmValid(); });
    connect(ui->edit_verify, &QLineEdit::editingFinished, this, [this]() { checkVerifyValid(); });

    ui->lab_pwd_visible->setCursor(Qt::PointingHandCursor);
    ui->lab_confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->lab_pwd_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    ui->lab_confirm_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");

    connect(ui->lab_pwd_visible, &ClickedLabel::clicked, this, [this] {
        auto state = ui->lab_pwd_visible->getCurState();
        if (state == ClickLbState::Normal) {
            ui->edit_pwd->setEchoMode(QLineEdit::Password);
        } else {
            ui->edit_pwd->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "lab_pwd_visible was clicked!";
    });

    connect(ui->lab_confirm_visible, &ClickedLabel::clicked, this, [this] {
        auto state = ui->lab_confirm_visible->getCurState();
        if (state == ClickLbState::Normal) {
            ui->edit_confirm->setEchoMode(QLineEdit::Password);
        } else {
            ui->edit_confirm->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "lab_confirm_visible was clicked!";
    });

    // 创建定时器，注册成功后调用
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, [this]() {
        if (_countdown == 0) {
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后返回登录界面").arg(_countdown);
        ui->lab_tip->setText(str);
    });
}

RegisterDialog::~RegisterDialog() {
    qDebug() << "RegisterDialog destructor";
    delete ui;
}

void RegisterDialog::on_btn_getCode_clicked() {
    // 获取邮箱信息
    auto email = ui->edit_email->text();
    // 正则表达式验证邮箱是否合法
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
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject json_obj) {
        int error = json_obj["error"].toInt();
        if(error == ErrorCodes::UserExist){
            showTip(tr("用户已经存在"), true);
            return;
        }else if(error== ErrorCodes::VerifyExpired){
            showTip(tr("验证码过期或失效"), true);
            return;
        }
        else if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), true);
            return;
        }
        auto uid = json_obj["uid"].toInt();
        showTip(tr("用户注册成功"), false);
        qDebug() << "Register succeed, uid is " << uid;
        changTipPage();
    });
}

bool RegisterDialog::checkUserValid() {
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

bool RegisterDialog::checkEmailValid() {
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

bool RegisterDialog::checkPassValid() {
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

bool RegisterDialog::checkConfirmValid() {
    auto confirm = ui->edit_confirm->text();
    if (confirm != ui->edit_pwd->text()) {
        addTipErr(TIP_CONFIRM_ERR, tr("两次密码输入不一致"));
        return false;
    }
    delTipErr(TIP_CONFIRM_ERR);
    return true;
}

bool RegisterDialog::checkVerifyValid() {
    auto pass = ui->edit_verify->text();
    if (pass.isEmpty()) {
        addTipErr(TipErr::TIP_VARIFY_ERR, tr("请输入验证码"));
        return false;
    }
    delTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::addTipErr(TipErr err, const QString& tips) {
    _tip_errs[err] = tips;
    showTip(tips, true);
}

void RegisterDialog::delTipErr(TipErr err) {
    _tip_errs.remove(err);
    if (_tip_errs.empty()) {
        ui->lab_errTip->clear();
        return;
    }
    // 还有错误就继续显示
    showTip(_tip_errs.first(), true);
}

void RegisterDialog::changTipPage() {
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    // 启动定时器，间隔为1 秒
    _countdown_timer->start(1000);
}

void RegisterDialog::on_btn_ok_clicked() {
    if (!checkUserValid() || !checkPassValid() || !checkVerifyValid() || !checkConfirmValid() || !checkEmailValid()) {
        return;
    }

    // 发送 http post 请求
    QJsonObject json_obj;
    json_obj["user"] = ui->edit_user->text();
    json_obj["email"] = ui->edit_email->text();
    json_obj["password"] = hashPassword(ui->edit_pwd->text());
    json_obj["confirm"] = hashPassword(ui->edit_confirm->text());
    json_obj["verifycode"] = ui->edit_verify->text();
    HttpMgr::getInstance()->postHttpReq(QUrl(gate_url_prefix + "/user_register"), json_obj, ReqId::ID_REG_USER,
                                        Modules::REGISTERMOD);
}

void RegisterDialog::on_btn_cancel_clicked() {
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

void RegisterDialog::on_btn_to_login_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

