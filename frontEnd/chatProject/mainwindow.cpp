#include "mainwindow.h"

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), _login_dialog(new LoginDialog(this)), _register_dialog(nullptr),
    _reset_dialog(nullptr) {
    ui->setupUi(this);
    // 将 LoginDialog 设置为中心组件
    setCentralWidget(_login_dialog);
    _login_dialog->show();
    // 连接信号与槽
    connect(_login_dialog, &LoginDialog::switch_to_register, this, &MainWindow::slotSwitchRegister);
    connect(_login_dialog, &LoginDialog::switchReset, this, &MainWindow::slotSwitchReset);
    // 自定义样式，设置无边框
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

MainWindow::~MainWindow() {
    qDebug() << "MainWindow destructor";
    delete ui;
    if (_login_dialog) {
        delete _login_dialog;
    }
    if (_register_dialog) {
        delete _register_dialog;
    }
}

void MainWindow::slotSwitchRegister() {
    if (_register_dialog == nullptr) {
        qDebug() << "_register_dialog is nullptr, start initing";
        _register_dialog = new RegisterDialog(this);
        _register_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        connect(_register_dialog, &RegisterDialog::sigSwitchLogin, this, &MainWindow::slotSwitchLogin);
    }

    // 隐藏登录对话
    _login_dialog->hide();
    // 取出中心组件，防止析构
    takeCentralWidget();
    setCentralWidget(_register_dialog);
    _register_dialog->show();
}

void MainWindow::slotSwitchLogin() {
    takeCentralWidget();
    _register_dialog->hide();
    _login_dialog->show();
    setCentralWidget(_login_dialog);
}

void MainWindow::slotSwitchReset() {
    if (_reset_dialog == nullptr) {
        // qDebug() << "_reset_dialog is nullptr, start initing";
        // 创建一个CentralWidget, 并将其设置为MainWindow的中心部件
        _reset_dialog = new ResetDialog(this);
        _reset_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        // 注册返回登录信号和槽函数
        connect(_reset_dialog, &ResetDialog::sigSwitchLogin, this, &MainWindow::slotSwitchLogin2);
    }
    takeCentralWidget();
    _login_dialog->hide();
    setCentralWidget(_reset_dialog);
    _reset_dialog->show();
}

void MainWindow::slotSwitchLogin2() {
    takeCentralWidget();
    _reset_dialog->hide();
    _login_dialog->show();
    setCentralWidget(_login_dialog);
}
