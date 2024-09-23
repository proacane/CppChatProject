#include "chatview.h"

#include <QEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>

ChatView::ChatView(QWidget* parent) : QWidget(parent), _is_appended(false) {
    QVBoxLayout* p_main_layout = new QVBoxLayout();
    this->setLayout(p_main_layout);
    // 垂直布局边距为 0
    p_main_layout->setContentsMargins(0, 0, 0, 0);

    // 创建滚动区域
    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    p_main_layout->addWidget(m_pScrollArea);

    // 显示聊天信息的 widget
    QWidget* w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);

    QVBoxLayout* pVLayout_1 = new QVBoxLayout();
    pVLayout_1->addWidget(new QWidget(), 100000);
    // w 里嵌套布局
    w->setLayout(pVLayout_1);
    // 滚动区域显示 w 的内容
    m_pScrollArea->setWidget(w);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 获取垂直滚动条
    QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
    // 滚动的时候调用槽函数
    connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::slot_VScrollBar_moved);
    // 把垂直滚动条放到滚动区域的右侧
    QHBoxLayout* pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout_2->setContentsMargins(0, 0, 0, 0);

    m_pScrollArea->setLayout(pHLayout_2);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    // initStyleSheet();
}

void ChatView::appendChatItem(QWidget* item) {
    QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    // 追加后面
    vl->insertWidget(vl->count() - 1, item);
    _is_appended = true;
}

void ChatView::prependChatItem(QWidget* item) {
}

void ChatView::insertChatItem(QWidget* before, QWidget* item) {
}

void ChatView::removeAllItem()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

    int count = layout->count();

    for (int i = 0; i < count - 1; ++i) {
        QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
        if (item) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    }
}

bool ChatView::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Enter && watched == m_pScrollArea) {
        // 鼠标进入显示滚动条
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    } else if (event->type() == QEvent::Leave && watched == m_pScrollArea) {
        // 鼠标离开隐藏滚动条
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::slot_VScrollBar_moved(int min, int max) {
    if (_is_appended) {
        QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
        // 滚动条下滑
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        // 500毫秒内可能调用多次
        QTimer::singleShot(500, this, [this]() { _is_appended = false; });
    }
}

void ChatView::initStyleSheet() {
}
