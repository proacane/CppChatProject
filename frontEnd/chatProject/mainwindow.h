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
    // 显示注册界面
    void slotSwitchRegister();
};
#endif  // MAINWINDOW_H
