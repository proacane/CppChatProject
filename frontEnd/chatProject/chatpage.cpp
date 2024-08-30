#include "chatpage.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

#include "chatitembase.h"
#include "clickedbtn.h"
#include "clickedlabel.h"
#include "global.h"
#include "messagetextedit.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "ui_chatpage.h"


ChatPage::ChatPage(QWidget* parent) : QWidget(parent), ui(new Ui::ChatPage) {
    ui->setupUi(this);
    ui->btn_receive->setState("normal", "hover", "press");
    ui->btn_send->setState("normal", "hover", "press");

    ui->lab_emoji->setState("normal", "hover", "press", "normal", "hover", "press");
    ui->lab_file->setState("normal", "hover", "press", "normal", "hover", "press");
    ui->lab_emoji->setText("");
    ui->lab_file->setText("");

    connect(ui->chat_edit,&MessageTextEdit::send,this,&ChatPage::on_btn_send_clicked);
}

ChatPage::~ChatPage() {
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_btn_send_clicked() {
    auto pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("texttext");
    QString userIcon = ":/images/head_1.jpg";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for (int i = 0; i < msgList.size(); ++i) {
        QString type = msgList[i].msgFlag;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserAvatar(QPixmap(userIcon));
        QWidget* pBubble = nullptr;
        if (type == "text") {
            pBubble = new TextBubble(role, msgList[i].content);
        } else if (type == "image") {
            pBubble = new PictureBubble(QPixmap(msgList[i].content), role);
        } else if (type == "file") {
        }
        if (pBubble != nullptr) {
            pChatItem->setWidget(pBubble);
            ui->widget_chat_data_list->appendChatItem(pChatItem);
        }
    }
}
