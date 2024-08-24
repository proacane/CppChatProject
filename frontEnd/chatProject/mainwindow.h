#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private:
    Ui::MainWindow* ui;
    // 登录界面
    LoginDialog* _login_dialog;
    // 注册界面
    RegisterDialog* _register_dialog;
    // 重置密码界面
    ResetDialog* _reset_dialog;
  private slots:
    // 从登陆界面切换到注册界面
    void slot_switch_register();
    // 从注册界面切换到登录界面
    void slot_switch_login();
    // 从登录界面切换到重置密码界面
    void slot_switch_reset();
    // 从重置密码界面切换到登录界面
    void slot_switch_login2();
};
#endif  // MAINWINDOW_H
