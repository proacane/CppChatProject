#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H
/*****************************************************************************
 * @file         contactuserlist.h
 * @brief
 * @description  联系人列表
 * @author       ACA
 * @date         2024/09/05
 * @history
 *****************************************************************************/
#include <QListWidget>

#include "userdata.h"

class ContactUserItem;
class ContactUserList : public QListWidget {
    Q_OBJECT
  public:
    explicit ContactUserList(QWidget* parent = nullptr);
    // 未读消息的红点
    void showRedPoint(bool b_show = true);
    virtual bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    // 填充列表
    void addContactUserList();
  public slots:
    void slot_item_clicked(QListWidgetItem* item);
  private slots:
    void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
  signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page();

  private:
    // 添加好友的框
    ContactUserItem* _add_friend_item;
    // 联系人分组()
    QListWidgetItem* _groupitem;

};

#endif  // CONTACTUSERLIST_H
