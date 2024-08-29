#include "chatdialog.h"

#include <QRandomGenerator>

#include "chatuserwidget.h"
#include "loadingdialog.h"
#include "ui_chatdialog.h"

// TODO 测试数据
std::vector<QString> strs = {"hello world !", "nice to meet u", "New year，new life", "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};
std::vector<QString> heads = {":/images/head_1.jpg", ":/images/head_2.jpg", ":/images/head_3.jpg",
                              ":/images/head_4.jpg", ":/images/head_5.jpg"};
std::vector<QString> names = {"mmmm", "zack", "golang", "cpp", "java", "nodejs", "python", "rust"};

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

    showSearchList(false);

    addChatUserList();
    // 动态加载用户列表
    connect(ui->list_chat_user, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
}

ChatDialog::~ChatDialog() {
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

void ChatDialog::slot_loading_chat_user() {
    if (_b_loading) {
        return;
    }
    _b_loading = true;

    LoadingDialog *loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    // qDebug() << "add new data to list.....";
    // 添加新 item
    addChatUserList();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();
    _b_loading = false;
}
