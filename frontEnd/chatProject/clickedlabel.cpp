#include "clickedlabel.h"

#include <QMouseEvent>
ClickedLabel::ClickedLabel(QWidget* parent) : QLabel(parent), _curstate(ClickLbState::Normal) {
}

void ClickedLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // 按下左键，切换状态
        if (_curstate == ClickLbState::Normal) {
            // qDebug() << "Mouse press event, switch to selected hover: " << _selected_press;
            _curstate = ClickLbState::Selected;
            setProperty("state", _selected_press);
            repolish(this);
            update();
        } else {
            // qDebug() << "Mouse press event, switch to normal hover: " << _normal_press;
            _curstate = ClickLbState::Normal;
            setProperty("state", _normal_press);
            repolish(this);
            update();
        }
        // emit clicked();
        return;
    }
    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEnterEvent* event) {
    setCursor(Qt::PointingHandCursor);
    if (_curstate == ClickLbState::Normal) {
        // qDebug() << "Mouse enter event, switch to normal hover: " << _normal_hover;
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    } else {
        // qDebug() << "Mouse enter event, switch to selected hover: " << _selected_hover;
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    }

    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent* event) {
    setCursor(Qt::ArrowCursor);
    if (_curstate == ClickLbState::Normal) {
        // qDebug() << "Mouse leave event, switch to normal: " << _normal;
        setProperty("state", _normal);
        repolish(this);
        update();
    } else {
        // qDebug() << "Mouse leave event, switch to selected: " << _selected;
        setProperty("state", _selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (_curstate == ClickLbState::Normal) {
            // qDebug() << "ReleaseEvent , change to normal hover: " << _normal_hover;
            setProperty("state", _normal_hover);
            repolish(this);
            update();
        } else {
            // qDebug() << "ReleaseEvent , change to select hover: " << _selected_hover;
            setProperty("state", _selected_hover);
            repolish(this);
            update();
        }
        qDebug() << "emit clickedlabel clicked event";
        emit clicked(this->text(), _curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(event);
}

void ClickedLabel::setState(QString normal, QString hover, QString press, QString select, QString select_hover,
                            QString select_press) {
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;
    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;
    setProperty("state", normal);
    repolish(this);
}

ClickLbState ClickedLabel::getCurState() {
    return _curstate;
}

void ClickedLabel::resetNormalState() {
    setProperty("state", _normal);
    repolish(this);
    update();
    _curstate = ClickLbState::Normal;
}

void ClickedLabel::setCurState(ClickLbState state) {
    _curstate = state;
}
