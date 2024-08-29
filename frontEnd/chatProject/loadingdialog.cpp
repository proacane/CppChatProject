#include "loadingdialog.h"

#include <QMovie>

#include "ui_loadingdialog.h"

LoadingDialog::LoadingDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoadingDialog) {
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    // 背景透明
    setAttribute(Qt::WA_TranslucentBackground);
    // 设置对话框为全屏尺寸
    setFixedSize(parent->size());
    QMovie* movie = new QMovie(":/images/loading.gif");
    ui->lab_loading->setMovie(movie);
    movie->start();
}

LoadingDialog::~LoadingDialog() {
    delete ui;
}
