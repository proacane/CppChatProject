#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H
/*****************************************************************************
 * @file         chatitembase.h
 * @brief
 * @description  消息框整体布局
 * @author       ACA
 * @date         2024/08/30
 * @history
 *****************************************************************************/
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "global.h"
// class BubbleFrame;

class ChatItemBase : public QWidget {
    Q_OBJECT
  public:
    ChatItemBase(ChatRole role, QWidget* parent = nullptr);
    void setUserName(const QString& name);
    void setUserAvatar(const QPixmap& icon);
    void setWidget(QWidget* w);

  private:
    // 谁发送的消息
    ChatRole _role;
    QLabel* _lab_name;
    QLabel* _lab_avatar;
    // 显示的气泡
    QWidget* _bubble;
};

#endif  // CHATITEMBASE_H
