#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "tcpmgr.h"
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), _login_dialog(new LoginDialog(this)), _register_dialog(nullptr),
    _reset_dialog(nullptr),_chat_dialog(nullptr) {
    ui->setupUi(this);
    // 将 LoginDialog 设置为中心组件
    setCentralWidget(_login_dialog);
    _login_dialog->show();
    // 连接信号与槽
    connect(_login_dialog, &LoginDialog::switch_to_register, this, &MainWindow::slot_switch_register);
    connect(_login_dialog, &LoginDialog::switchReset, this, &MainWindow::slot_switch_reset);
    // 自定义样式，设置无边框
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_swich_chatdlg, this, &MainWindow::slot_switch_chatdlg);

    // 禁用最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
    emit TcpMgr::getInstance().get() -> sig_swich_chatdlg();
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

void MainWindow::slot_switch_register() {
    if (_register_dialog == nullptr) {
        // qDebug() << "_register_dialog is nullptr, start initing";
        _register_dialog = new RegisterDialog(this);
        _register_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        connect(_register_dialog, &RegisterDialog::sigSwitchLogin, this, &MainWindow::slot_switch_login);
    }

    // 隐藏登录对话
    _login_dialog->hide();
    // 取出中心组件，防止析构
    takeCentralWidget();
    setCentralWidget(_register_dialog);
    _register_dialog->show();
    // 禁用最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
}

void MainWindow::slot_switch_login() {
    takeCentralWidget();
    _register_dialog->hide();
    _login_dialog->show();
    setCentralWidget(_login_dialog);
    // 禁用最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
}

void MainWindow::slot_switch_reset() {
    if (_reset_dialog == nullptr) {
        // qDebug() << "_reset_dialog is nullptr, start initing";
        // 创建一个CentralWidget, 并将其设置为MainWindow的中心部件
        _reset_dialog = new ResetDialog(this);
        _reset_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        // 注册返回登录信号和槽函数
        connect(_reset_dialog, &ResetDialog::sigSwitchLogin, this, &MainWindow::slot_switch_login2);
    }
    takeCentralWidget();
    _login_dialog->hide();
    setCentralWidget(_reset_dialog);
    _reset_dialog->show();
    // 禁用最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
}

void MainWindow::slot_switch_login2() {
    takeCentralWidget();
    _reset_dialog->hide();
    _login_dialog->show();
    setCentralWidget(_login_dialog);
    // 禁用最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
}

void MainWindow::slot_switch_chatdlg() {
    if (_chat_dialog == nullptr) {
        _chat_dialog = new ChatDialog(this);
        _chat_dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    }
    takeCentralWidget();
    _login_dialog->hide();
    setCentralWidget(_chat_dialog);
    _chat_dialog->show();
    // 禁用最大化按钮
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    show();  // 重新显示窗口以应用新的窗口标志
    this->setMinimumSize(QSize(910, 640));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    if (_register_dialog != nullptr) {
        delete _register_dialog;
        _register_dialog = nullptr;
    }
    if (_login_dialog != nullptr) {
        delete _login_dialog;
        _login_dialog = nullptr;
    }
    if (_reset_dialog != nullptr) {
        delete _reset_dialog;
        _reset_dialog = nullptr;
    }
}
