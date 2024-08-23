#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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
signals:
    // 跳转到注册界面
    void switch_to_register();
    // 跳转到忘记密码界面
    void switchReset();
private slots:
    // 忘记密码
    void slotForgetPwd();
};

#endif // LOGINDIALOG_H
