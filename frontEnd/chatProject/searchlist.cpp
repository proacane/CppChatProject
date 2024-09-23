#include "searchlist.h"

#include <QWheelEvent>

#include "adduseritem.h"
#include "customizeedit.h"
#include "findfaildialog.h"
#include "findsuccessdlg.h"
#include "tcpmgr.h"
SearchList::SearchList(QWidget* parent) :
    QListWidget(parent), _find_dialog(nullptr), _search_edit(nullptr), _send_pending(false) {
    // 隐藏滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    viewport()->installEventFilter(this);
    // 链接信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    // 添加条目
    addTipItem();
    // 连接搜索条目
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
    connect(TcpMgr::getInstance().get(), &TcpMgr::sig_user_search_failed, this, &SearchList::slot_user_search_failed);
}

void SearchList::closeFindDlg() {
    if (_find_dialog) {
        _find_dialog->hide();
        _find_dialog = nullptr;
    }
}

void SearchList::setSearchEdit(QWidget* edit) {
    _search_edit = edit;
}

bool SearchList::eventFilter(QObject* watched, QEvent* event) {
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
        return true;  // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}

void SearchList::waitPending(bool pending) {
    if (pending) {
        _loadingDialog = new LoadingDialog(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = pending;
    } else {
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;
    }
}

void SearchList::addTipItem() {
    auto* invalid_item = new QWidget();
    QListWidgetItem* item_tmp = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250, 10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);

    auto* add_user_item = new AddUserItem();
    QListWidgetItem* item = new QListWidgetItem;
    // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem* item) {
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
    if (itemType == ListItemType::INVALID_ITEM) {
        qDebug() << "slot invalid item clicked ";
        return;
    }
    if (itemType == ListItemType::ADD_USER_TIP_ITEM) {
        // 根据 uid、姓名查询用户
        if (_send_pending) {
            // 上次发送的查找请求还没处理完
            return;
        }
        if (!_search_edit) {
            qDebug() << "searchlist: _search_edit is nullptr";
            return;
        }
        // 显示加载场景
        waitPending(true);

        // 转换成原本的类型
        auto search_edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str = search_edit->text();
        QJsonObject json_obj;
        json_obj["searchInfo"] = uid_str;
        QJsonDocument doc(json_obj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        emit TcpMgr::getInstance() -> sig_send_data(ReqId::ID_SEARCH_USER_REQ, jsonData);
    }
    // 清除弹出框
    closeFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si) {
    waitPending(false);
    if (si == nullptr) {
        // TODO 查询失败的 dialog
        _find_dialog = std::make_shared<FindFailDialog>(this);
    } else {
        // 可能是自己的好友，也可能不是自己的好友，也可能是自己
        // TODO 处理已经是自己的好友或者是自己的情况
        _find_dialog = std::make_shared<FindSuccessDlg>(this);
        std::dynamic_pointer_cast<FindSuccessDlg>(_find_dialog)->setSearchInfo(si);
    }

    _find_dialog->show();
}

void SearchList::slot_user_search_failed(int err) {
    waitPending(false);
    // 弹出 dialog 显示查询失败
    _find_dialog = std::make_shared<FindFailDialog>(this);
    _find_dialog->show();
}
