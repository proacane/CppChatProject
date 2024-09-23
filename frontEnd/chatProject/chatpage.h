#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>

#include "userdata.h"

namespace Ui {
    class ChatPage;
}

class ChatPage : public QWidget {
    Q_OBJECT

  public:
    explicit ChatPage(QWidget* parent = nullptr);
    ~ChatPage();

    void setUserInfo(std::shared_ptr<UserInfo> user_info);
    void appendChatMsg(std::shared_ptr<TextChatData> msg);
  private:
    Ui::ChatPage* ui;
    void clearItems();
    std::shared_ptr<UserInfo> _user_info;
    QMap<QString, QWidget*> _bubble_map;
    // QWidget interface
  protected:
    virtual void paintEvent(QPaintEvent* event) override;
  private slots:
    void on_btn_send_clicked();
};

#endif  // CHATPAGE_H
