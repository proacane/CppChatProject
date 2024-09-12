#include "contactuserlist.h"

#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QScrollBar>

#include "contactuseritem.h"
#include "grouptipitem.h"
#include "tcpmgr.h"
#include "usermgr.h"

ContactUserList::ContactUserList(QWidget* parent) : QListWidget(parent) {
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);

    // TODO 后期改用数据库的信息
    addContactUserList();

    connect(this, &ContactUserList::itemClicked, this, &ContactUserList::slot_item_clicked);
    // 链接对端同意认证后通知的信号，添加到联系人列表
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_add_auth_friend, this, &ContactUserList::slot_add_auth_firend);
    // 链接自己点击同意认证后界面刷新，添加到联系人列表
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_auth_rsp, this, &ContactUserList::slot_auth_rsp);
}

void ContactUserList::showRedPoint(bool b_show) {
    _add_friend_item->showRedPoint(b_show);
}

void ContactUserList::addContactUserList() {
    // 添加好友的分组条
    auto* groupTip = new GroupTipItem(this);
    QListWidgetItem* item = new QListWidgetItem(this);
    item->setSizeHint(groupTip->sizeHint());
    // 添加 item
    addItem(item);
    // 提升item 类型
    setItemWidget(item, groupTip);

    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    // 添加好友的item
    _add_friend_item = new ContactUserItem(this);
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->setInfo(0, tr("新的朋友"), ":/images/add_friend.png");
    // 添加进去正常类型，提升为 _add_friend_item
    QListWidgetItem* add_item = new QListWidgetItem(this);
    add_item->setSizeHint(_add_friend_item->sizeHint());
    addItem(add_item);
    setItemWidget(add_item, _add_friend_item);
    // 设置默认选中
    setCurrentItem(add_item);

    // 下面是联系人列表
    auto* groupContact = new GroupTipItem(this);
    groupContact->setGroupTip("联系人");
    _groupitem = new QListWidgetItem(this);
    _groupitem->setSizeHint(groupContact->sizeHint());
    addItem(_groupitem);
    setItemWidget(_groupitem, groupContact);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

    // 添加模拟用户
    // 创建QListWidgetItem，并设置自定义的widget
    // for (int i = 0; i < 13; i++) {
    //     int randomValue = QRandomGenerator::global()->bounded(100);  // 生成0到99之间的随机整数
    //     int str_i = randomValue % strs.size();
    //     int head_i = randomValue % heads.size();
    //     int name_i = randomValue % names.size();
    //     auto* con_user_wid = new ContactUserItem();
    //     con_user_wid->setInfo(0, names[name_i], heads[head_i]);
    //     QListWidgetItem* item = new QListWidgetItem;
    //     // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(con_user_wid->sizeHint());
    //     this->addItem(item);
    //     this->setItemWidget(item, con_user_wid);
    // }
}

void ContactUserList::slot_item_clicked(QListWidgetItem* item) {
    // 根据不同的类型发出不同的信号
    QWidget* widget = this->itemWidget(item);  // 获取自定义widget对象
    if (!widget) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }
    auto itemType = customItem->getItemType();
    if (itemType == ListItemType::INVALID_ITEM || itemType == ListItemType::GROUP_TIP_ITEM) {
        qDebug() << "slot invalid item clicked ";
        return;
    }
    if (itemType == ListItemType::APPLY_FRIEND_ITEM) {
        // 创建对话框，提示用户
        qDebug() << "apply friend item clicked ";
        // 跳转到好友申请界面
        emit sig_switch_apply_friend_page();
        return;
    }
    if (itemType == ListItemType::CONTACT_USER_ITEM) {
        // 创建对话框，提示用户
        qDebug() << "contact user item clicked ";
        // 跳转到好友申请界面
        emit sig_switch_friend_info_page();
        return;
    }
}

void ContactUserList::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info) {
    qDebug() << "slot add auth friend called";
    bool isFriend = UserMgr::getInstance()->checkFriendById(auth_info->_uid);
    if (isFriend) {
        return;
    }
    // 在 groupitem 之后插入新项
    auto* con_user_wid = new ContactUserItem();
    con_user_wid->setInfo(auth_info->_uid, auth_info->_name, auth_info->_avatar);
    QListWidgetItem* item = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());
    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid);
}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp) {
    qDebug() << "slot auth rsp called";
    bool isFriend = UserMgr::getInstance()->checkFriendById(auth_rsp->_uid);
    if (isFriend) {
        return;
    }
    // 在 groupitem 之后插入新项
    auto* con_user_wid = new ContactUserItem();
    con_user_wid->setInfo(auth_rsp->_uid, auth_rsp->_name, auth_rsp->_avatar);
    QListWidgetItem* item = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(con_user_wid->sizeHint());
    // 获取 groupitem 的索引
    int index = this->row(_groupitem);
    // 在 groupitem 之后插入新项
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid);
}

bool ContactUserList::eventFilter(QObject* watched, QEvent* event) {
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;  // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        // 检查是否滚动到底部
        QScrollBar* scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        // int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            qDebug() << "load more contact user";
            // 发送信号通知聊天界面加载更多聊天内容
            // TODO 加载更多联系人
            emit sig_loading_contact_user();
        }
        return true;  // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}
