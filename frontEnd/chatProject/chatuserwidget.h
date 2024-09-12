#ifndef CHATUSERWIDGET_H
#define CHATUSERWIDGET_H
/*****************************************************************************
 * @file         chatuserwidget.h
 * @brief
 * @description  聊天列表的 item
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QWidget>

#include "listitembase.h"
#include "userdata.h"
namespace Ui {
    class ChatUserWidget;
}

class ChatUserWidget : public ListItemBase {
    Q_OBJECT

  public:
    explicit ChatUserWidget(QWidget* parent = nullptr);
    ~ChatUserWidget();

    QSize sizeHint() const override {
        return QSize(250, 70);
    }
    void setInfo(QString name, QString avatar, QString msg);
    void setInfo(std::shared_ptr<UserInfo> user_info);
  private:
    Ui::ChatUserWidget* ui;
    // 用户名
    QString _name;
    // 头像
    QString _avatar;
    // 最后一条消息
    QString _msg;
};

#endif  // CHATUSERWIDGET_H
