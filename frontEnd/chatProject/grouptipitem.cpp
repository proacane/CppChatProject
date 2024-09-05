#include "grouptipitem.h"

#include "ui_grouptipitem.h"

GroupTipItem::GroupTipItem(QWidget* parent) : ListItemBase(parent), ui(new Ui::GroupTipItem) {
    ui->setupUi(this);
    setItemType(ListItemType::GROUP_TIP_ITEM);
    installEventFilter(this);
}

GroupTipItem::~GroupTipItem() {
    removeEventFilter(this);
    delete ui;
}

QSize GroupTipItem::sizeHint() const {
    return QSize(250, 25);
}

void GroupTipItem::setGroupTip(QString str) {
    ui->label->setText(str);
}

bool GroupTipItem::eventFilter(QObject* watched, QEvent* event) {
    return true;
}
