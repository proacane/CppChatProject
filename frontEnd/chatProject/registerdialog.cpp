#include "registerdialog.h"
#include"global.h"
#include "ui_registerdialog.h"

#include <QRegularExpression>

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
    ui->setupUi(this);

    ui->lab_errTip->setProperty("state","normal");
    repolish(ui->lab_errTip);
}

RegisterDialog::~RegisterDialog() {
    qDebug() << "RegisterDialog destructor";
    delete ui;
}

void RegisterDialog::on_btn_getCode_clicked()
{
    // 获取邮箱信息
    auto email = ui->edit_email->text();
    // 正则表达式验证邮箱是否合法
    QRegularExpression regex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
    auto email_match = regex.match(email).hasMatch();
    if(email_match){
        // TODO 发送验证码
    }else{
        showTip(tr("邮箱地址不正确"));
    }
}

void RegisterDialog::showTip(const QString &tip)
{
    ui->lab_errTip->setText(tip);
    ui->lab_errTip->setProperty("state","err");
    repolish(ui->lab_errTip);
}

