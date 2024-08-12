#include "logindialog.h"

#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    connect(ui->btn_register, &QPushButton::clicked, this, &LoginDialog::switch_to_register);
}

LoginDialog::~LoginDialog() {
    qDebug()<<"LoginDialog destructor";
    delete ui;
}

