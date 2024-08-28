#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget* parent) : QLineEdit(parent),_max_len(0) {
    connect(this,&QLineEdit::textEdited,this,&CustomizeEdit::limitTextLength);
}

void CustomizeEdit::setMaxLength(int max_len) {
    _max_len = max_len;
}

void CustomizeEdit::limitTextLength(QString text) {
    if (_max_len <= 0) {
        return;
    }
    QByteArray arr = text.toUtf8();
    if (arr.size() > _max_len) {
        arr = arr.left(_max_len);
        setText(QString::fromUtf8(arr));
    }
}
