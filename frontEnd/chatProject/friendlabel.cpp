#include "friendlabel.h"

#include "ui_friendlabel.h"

FriendLabel::FriendLabel(QWidget* parent) : QFrame(parent), ui(new Ui::FriendLabel) {
    ui->setupUi(this);
    setContentsMargins(0, 0, 0, 0);
    ui->close_label->setState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");

    connect(ui->close_label, &ClickedLabel::clicked, this, &FriendLabel::slot_close);
}

FriendLabel::~FriendLabel() {
    delete ui;
}

void FriendLabel::setText(QString text) {
    _text = text;
    ui->tip_lb->setText(_text);
    ui->tip_lb->adjustSize();
    // 获取字体
    QFontMetrics fontMetrics(ui->tip_lb->font());
    auto text_width = fontMetrics.horizontalAdvance(ui->tip_lb->text());
    auto text_height = fontMetrics.height();

    qDebug() << "ui->tip_lb.width is " << ui->tip_lb->width();
    qDebug() << "ui->close_lab->width is " << ui->close_label->width();
    qDebug() << "text_width is " << text_width;
    qDebug()<<"text_height is "<<text_height;
    this->setFixedWidth(ui->tip_lb->width() + ui->close_label->width() + 5);
    this->setFixedHeight(text_height + 2);
    _width =ui->tip_lb->width() + ui->close_label->width() + 5;
    _height = text_height + 2;
}

int FriendLabel::width() const{
    return _width;
}

int FriendLabel::height() const{
    return _height;
}

QString FriendLabel::text() {
    return _text;
}

void FriendLabel::slot_close() {
    emit sig_close(_text);
}
