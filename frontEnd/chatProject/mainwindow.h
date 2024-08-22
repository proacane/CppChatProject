#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "logindialog.h"
#include "registerdialog.h"
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

private slots:
    // 切换到注册界面
    void slotSwitchRegister();
    // 切换到登录界面
    void slotSwitchLogin();
};
#endif  // MAINWINDOW_H
