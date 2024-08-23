#include "logindialog.h"

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    connect(ui->btn_register, &QPushButton::clicked, this, &LoginDialog::switch_to_register);
    ui->edit_password->setEchoMode(QLineEdit::Password);

    ui->lab_forgetPwd->setState("normal", "hover", "", "selected", "selected_hover", "");
    ui->lab_forgetPwd->setCursor(Qt::PointingHandCursor);
    connect(ui->lab_forgetPwd, &ClickedLabel::clicked, this, &LoginDialog::slotForgetPwd);
}

LoginDialog::~LoginDialog() {
    qDebug() << "LoginDialog destructor";
    delete ui;
}

void LoginDialog::slotForgetPwd() {
    // qDebug() << "slot forget pwd";
    emit switchReset();
}
