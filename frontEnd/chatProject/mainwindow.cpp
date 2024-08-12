#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // 将 LoginDialog 设置为中心组件
    _login_dialog = new LoginDialog(this);
    setCentralWidget(_login_dialog);
    _login_dialog->show();
    // 连接信号与槽
    connect(_login_dialog,&LoginDialog::switch_to_register,this,&MainWindow::slotSwitchRegister);
    // 自定义样式，设置无边框
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
}

MainWindow::~MainWindow() {
    qDebug()<<"MainWindow destructor";
    delete ui;
    if(_login_dialog){
        delete _login_dialog;
    }
    if(_register_dialog){
        delete _register_dialog;
    }
}

void MainWindow::slotSwitchRegister() {
    _register_dialog = new RegisterDialog(this);
    _register_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    // 隐藏登录对话
    _login_dialog->hide();
    // 取出中心组件，防止析构
    takeCentralWidget();
    setCentralWidget(_register_dialog);
    _register_dialog->show();
}
