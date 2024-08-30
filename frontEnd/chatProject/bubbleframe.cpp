#include "bubbleframe.h"

#include <QPainter>

const int WIDTH_SANJIAO = 8;  // 三角宽

BubbleFrame::BubbleFrame(ChatRole role, QWidget* parent) : m_role(role), QFrame(parent), m_margin(5) {
    m_pHLayout = new QHBoxLayout();
    if (m_role == ChatRole::Self)
        m_pHLayout->setContentsMargins(m_margin, m_margin-2, WIDTH_SANJIAO + m_margin, m_margin-2);
    else
        m_pHLayout->setContentsMargins(WIDTH_SANJIAO + m_margin, m_margin-2, m_margin, m_margin-2);

    this->setLayout(m_pHLayout);
}

void BubbleFrame::setMargin(int margin) {
    m_margin = margin;
}

void BubbleFrame::setWidget(QWidget* w) {
    if (m_pHLayout->count() > 0)
        return;
    else {
        m_pHLayout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    if (m_role == ChatRole::Other) {
        // 画气泡
        QColor background_color(Qt::white);
        painter.setBrush(QBrush(background_color));
        QRect rect = QRect(WIDTH_SANJIAO, 0, this->width() - WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(rect, 5, 5);
        // 画三角
        QPointF points[3] = {
            QPointF(rect.x(), 12), QPointF(rect.x(), 10 + WIDTH_SANJIAO * sqrt(3) / 2),  // 等边三角形高度
            QPointF(rect.x() - WIDTH_SANJIAO, 12 + WIDTH_SANJIAO * sqrt(3) / 4),         // 中点
        };
        painter.drawPolygon(points, 3);
    } else {
        QColor background_color(149, 236, 105);
        painter.setBrush(QBrush(background_color));
        // 画气泡
        QRect rect = QRect(0, 0, this->width() - WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(rect, 5, 5);
        // 画三角
        QPointF points[3] = {
            QPointF(rect.x() + rect.width(), 12),
            QPointF(rect.x() + rect.width(), 12 + WIDTH_SANJIAO * sqrt(3) / 2),  // 等边三角形的高度
            QPointF(rect.x() + rect.width() + WIDTH_SANJIAO, 12 + WIDTH_SANJIAO * sqrt(3) / 4)  // 中间点
        };
        painter.drawPolygon(points, 3);
    }

    return QFrame::paintEvent(e);
}
