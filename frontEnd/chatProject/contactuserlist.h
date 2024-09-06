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
    //     void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    //     void slot_auth_rsp(std::shared_ptr<AuthRsp>);
  signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page();
private:
    // 添加好友的框
    ContactUserItem* _add_friend_item;
    // 联系人分组()
    QListWidgetItem * _groupitem;

    // TODO 测试数据
    std::vector<QString> strs = {"hello world !", "nice to meet u", "New year，new life", "You have to love yourself",
                                 "My love is written in the wind ever since the whole world is you"};
    std::vector<QString> heads = {":/images/head_1.jpg", ":/images/head_2.jpg", ":/images/head_3.jpg",
                                  ":/images/head_4.jpg", ":/images/head_5.jpg"};
    std::vector<QString> names = {"mmmm", "zack", "golang", "cpp", "java", "nodejs", "python", "rust"};
};

#endif  // CONTACTUSERLIST_H
