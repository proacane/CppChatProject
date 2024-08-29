#include "chatpage.h"

#include "clickedbtn.h"
#include "clickedlabel.h"
#include "ui_chatpage.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
ChatPage::ChatPage(QWidget* parent) : QWidget(parent), ui(new Ui::ChatPage) {
    ui->setupUi(this);
    ui->btn_receive->setState("normal", "hover", "press");
    ui->btn_send->setState("normal", "hover", "press");

    ui->lab_emoji->setState("normal", "hover", "press", "normal", "hover", "press");
    ui->lab_file->setState("normal", "hover", "press", "normal", "hover", "press");
    ui->lab_emoji->setText("");
    ui->lab_file->setText("");
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
