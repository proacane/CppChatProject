#include "contactuseritem.h"

#include "ui_contactuseritem.h"

ContactUserItem::ContactUserItem(QWidget* parent) : ListItemBase(parent), ui(new Ui::ContactUserItem) {
    ui->setupUi(this);
    setItemType(ListItemType::CONTACT_USER_ITEM);
    ui->lab_point->raise();
    showRedPoint(false);
}

ContactUserItem::~ContactUserItem() {
    delete ui;
}

QSize ContactUserItem::sizeHint() const {
    return QSize(250, 70);
}

void ContactUserItem::setInfo(std::shared_ptr<AuthInfo> auth_info) {
    _info = std::make_shared<UserInfo>(auth_info);
    // 加载图片
    QPixmap pixmap(_info->_avatar);
    // 设置图片自动缩放
    ui->lab_avatar->setPixmap(pixmap.scaled(ui->lab_avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->lab_avatar->setScaledContents(true);
    ui->label_user_name->setText(_info->_name);
}

void ContactUserItem::setInfo(std::shared_ptr<AuthRsp> auth_rsp) {
    _info = std::make_shared<UserInfo>(auth_rsp);
    // 加载图片
    QPixmap pixmap(_info->_avatar);
    // 设置图片自动缩放
    ui->lab_avatar->setPixmap(pixmap.scaled(ui->lab_avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->lab_avatar->setScaledContents(true);
    ui->label_user_name->setText(_info->_name);
}

void ContactUserItem::setInfo(int uid, QString name, QString icon) {
    _info = std::make_shared<UserInfo>(uid, name, icon);
    // 加载图片
    QPixmap pixmap(_info->_avatar);
    // 设置图片自动缩放
    ui->lab_avatar->setPixmap(pixmap.scaled(ui->lab_avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->lab_avatar->setScaledContents(true);
    ui->label_user_name->setText(_info->_name);
}

void ContactUserItem::showRedPoint(bool show) {
    if (show) {
        ui->lab_point->show();
    } else {
        ui->lab_point->hide();
    }
}

std::shared_ptr<UserInfo> ContactUserItem::getInfo() const {
    return _info;
}
