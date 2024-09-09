#include "chatdialog.h"

#include <QRandomGenerator>

#include "chatuserwidget.h"
#include "loadingdialog.h"
#include "ui_chatdialog.h"



ChatDialog::ChatDialog(QWidget* parent) :
    QDialog(parent), ui(new Ui::ChatDialog), _mode(ChatUIMode::ChatMode), _state(ChatUIMode::ChatMode),
    _b_loading(false) {
    //
    ui->setupUi(this);
    ui->btn_add->setState("normal", "hover", "press");
    // 搜索框设置
    ui->edit_search->setClearButtonEnabled(true);
    QAction* search_act = new QAction(ui->edit_search);
    search_act->setIcon(QIcon(":/images/search.png"));
    ui->edit_search->addAction(search_act, QLineEdit::LeadingPosition);
    ui->edit_search->setPlaceholderText(QStringLiteral("搜索"));
    ui->edit_search->setMaxLength(15);

    addChatUserList();
    showSearchList(false);
    // 动态加载用户列表
    connect(ui->list_chat_user, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);

    ui->list_search->setSearchEdit(ui->edit_search);

    // TODO 从服务器中获取图片
    QPixmap pixmap(":/images/head_1.jpg");
    ui->lab_side_avatar->setPixmap(pixmap);  // 将图片设置到QLabel上
    QPixmap scaledPixmap = pixmap.scaled(ui->lab_side_avatar->size(), Qt::KeepAspectRatio);  // 将图片缩放到label的大小
    ui->lab_side_avatar->setPixmap(scaledPixmap);  // 将缩放后的图片设置到QLabel上
    ui->lab_side_avatar->setScaledContents(true);  // 设置QLabel自动缩放图片内容以适应大小

    ui->lab_side_chat->setState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->lab_side_contact->setState("normal", "hover", "pressed", "selected_normal", "selected_hover",
                                   "selected_pressed");
    // 添加到侧边栏组
    addLBGroup(ui->lab_side_chat);
    addLBGroup(ui->lab_side_contact);
    connect(ui->lab_side_chat, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    ui->lab_side_chat->setSelected(true);
    connect(ui->lab_side_contact, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    // 链接搜索框输入变化
    connect(ui->edit_search, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    // 检测鼠标点击位置，判断是否需要关闭搜索框
    installEventFilter(this);
}

ChatDialog::~ChatDialog() {
    removeEventFilter(this);
    delete ui;
}

void ChatDialog::addChatUserList() {
    // 创建QListWidgetItem，并设置自定义的widget
    for (int i = 0; i < 13; i++) {
        int randomValue = QRandomGenerator::global()->bounded(100);  // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();
        auto* chat_user_wid = new ChatUserWidget();
        chat_user_wid->setInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem* item = new QListWidgetItem;
        // qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->list_chat_user->addItem(item);
        ui->list_chat_user->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::showSearchList(bool b_show) {
    if (b_show) {
        ui->list_chat_user->hide();
        ui->list_contact->hide();
        ui->list_search->show();
        _mode = ChatUIMode::SearchMode;
    } else if (_state == ChatUIMode::ChatMode) {
        ui->list_chat_user->show();
        ui->list_contact->hide();
        ui->list_search->hide();
        _mode = ChatUIMode::ChatMode;
    } else if (_state == ChatUIMode::ContactMode) {
        ui->list_chat_user->hide();
        ui->list_contact->show();
        ui->list_search->hide();
        _mode = ChatUIMode::ContactMode;
    }
}

void ChatDialog::addLBGroup(StateWidget* lb) {
    _lb_list.push_back(lb);
}

void ChatDialog::clearLabelState(StateWidget* lb) {
    for (auto& label : _lb_list) {
        if (label == lb) {
            continue;
        }
        // 其它侧边栏按钮变为正常状态
        label->clearState();
    }
}

void ChatDialog::handleGlobalMousePress(QMouseEvent* mouseEvent) {
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if (_mode != ChatUIMode::SearchMode) {
        return;
    }
    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = (ui->list_search->mapFromGlobal(mouseEvent->globalPosition())).toPoint();
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->list_search->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->edit_search->clear();
        showSearchList(false);
    }
}

void ChatDialog::slot_loading_chat_user() {
    if (_b_loading) {
        return;
    }
    _b_loading = true;

    LoadingDialog* loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    // qDebug() << "add new data to list.....";
    // 添加新 item
    addChatUserList();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();
    _b_loading = false;
}

void ChatDialog::slot_side_chat() {
    clearLabelState(ui->lab_side_chat);
    ui->stackedWidget->setCurrentWidget(ui->page_chat);
    _state = ChatUIMode::ChatMode;
    showSearchList(false);
}

void ChatDialog::slot_side_contact() {
    clearLabelState(ui->lab_side_contact);
    ui->stackedWidget->setCurrentWidget(ui->page_friend_apply);
    _state = ChatUIMode::ContactMode;
    showSearchList(false);
}

void ChatDialog::slot_text_changed(const QString& str) {
    if (!str.isEmpty()) {
        showSearchList(true);
    } else {
        showSearchList(false);
    }
}

bool ChatDialog::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {  // 检查是否按下了 ESC 键
            QCoreApplication::sendEvent(parent(), event);
            return true;  // 表示事件已被处理
        }
    }
    return QDialog::eventFilter(watched,event);
}
