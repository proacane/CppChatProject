#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
/*****************************************************************************
 * @file         chatuserlist.h
 * @brief
 * @description  聊天列表
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QDebug>
#include <QEvent>
#include <QListWidget>
#include <QScrollBar>
#include <QWheelEvent>
class ChatUserList : public QListWidget {
    Q_OBJECT
  public:
    ChatUserList(QWidget* parent = nullptr);

  signals:
    void sig_loading_chat_user();

public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif  // CHATUSERLIST_H
