#include "adduseritem.h"

#include "ui_adduseritem.h"

AddUserItem::AddUserItem(QWidget* parent) : ui(new Ui::AddUserItem), ListItemBase(parent) {
    ui->setupUi(this);
    setItemType(ListItemType::ADD_USER_TIP_ITEM);
}

AddUserItem::~AddUserItem() {
}
