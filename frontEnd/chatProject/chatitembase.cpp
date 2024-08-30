#include "chatitembase.h"

ChatItemBase::ChatItemBase(ChatRole role, QWidget* parent) : QWidget(parent), _role(role) {
    _lab_name = new QLabel();
    _lab_name->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    _lab_name->setFont(font);
    _lab_name->setFixedHeight(20);

    // 头像标签
    _lab_avatar = new QLabel();
    _lab_avatar->setScaledContents(true);
    _lab_avatar->setFixedSize(42, 42);

    _bubble = new QWidget();
    // 网格布局
    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setContentsMargins(3, 3, 3, 3);

    QSpacerItem* pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    if (_role == ChatRole::Self) {
        _lab_name->setContentsMargins(0, 0, 8, 0);
        _lab_name->setAlignment(Qt::AlignRight);
        pGLayout->addWidget(_lab_name, 0, 1, 1, 1);
        pGLayout->addWidget(_lab_avatar, 0, 2, 2, 1, Qt::AlignTop);
        pGLayout->addItem(pSpacer, 1, 0, 1, 1);
        pGLayout->addWidget(_bubble, 1, 1, 1, 1);
        pGLayout->setColumnStretch(0, 2);
        pGLayout->setColumnStretch(1, 3);
    } else {
        _lab_name->setContentsMargins(8, 0, 0, 0);
        _lab_name->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(_lab_avatar, 0, 0, 2, 1, Qt::AlignTop);
        pGLayout->addWidget(_lab_name, 0, 1, 1, 1);
        pGLayout->addWidget(_bubble, 1, 1, 1, 1);
        pGLayout->addItem(pSpacer, 2, 2, 1, 1);
        pGLayout->setColumnStretch(1, 3);
        pGLayout->setColumnStretch(2, 2);
    }
    this->setLayout(pGLayout);
}

void ChatItemBase::setUserName(const QString& name) {
    _lab_name->setText(name);
}

void ChatItemBase::setUserAvatar(const QPixmap& icon) {
    _lab_avatar->setPixmap(icon);
}

void ChatItemBase::setWidget(QWidget* w) {
    QGridLayout* pGLayout = (qobject_cast<QGridLayout*>)(this->layout());
    pGLayout->replaceWidget(_bubble, w);
    delete _bubble;
    _bubble = w;
}
