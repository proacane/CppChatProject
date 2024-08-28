#include "chatuserwidget.h"

#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget* parent) : ListItemBase(parent), ui(new Ui::ChatUserWidget) {
    ui->setupUi(this);
}

ChatUserWidget::~ChatUserWidget() {
    delete ui;
}

void ChatUserWidget::setInfo(QString name, QString avatar, QString msg) {
    _name = name;
    _avatar = avatar;
    _msg = msg;

    // 加载图片
    QPixmap pix(_avatar);

    // 设置图片自动缩放
    // 设置图片自动缩放
    ui->lab_avatar->setPixmap(pix.scaled(ui->lab_avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->lab_avatar->setScaledContents(true);

    ui->lab_user_name->setText(_name);
    ui->lab_chat_msg->setText(_msg);
}
