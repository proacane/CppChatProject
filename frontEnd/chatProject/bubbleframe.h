#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H
/*****************************************************************************
 * @file         bubbleframe.h
 * @brief
 * @description  气泡绘制基类
 * @author       ACA
 * @date         2024/08/30
 * @history
 *****************************************************************************/
#include <QFrame>
#include <QHBoxLayout>

#include "global.h"

class BubbleFrame : public QFrame {
  public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void setMargin(int margin);

    void setWidget(QWidget* w);

  protected:
    void paintEvent(QPaintEvent* e) override;

  private:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role;
    int m_margin;
};

#endif  // BUBBLEFRAME_H
