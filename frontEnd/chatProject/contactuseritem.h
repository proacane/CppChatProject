#ifndef CONTACTUSERITEM_H
#define CONTACTUSERITEM_H
/*****************************************************************************
 * @file         contactuseritem.h
 * @brief
 * @description  联系列表的 item
 * @author       ACA
 * @date         2024/09/05
 * @history
 *****************************************************************************/
#include <QWidget>

#include "listitembase.h"
#include "userdata.h"
namespace Ui {
    class ContactUserItem;
}

class ContactUserItem : public ListItemBase {
    Q_OBJECT

  public:
    explicit ContactUserItem(QWidget* parent = nullptr);
    ~ContactUserItem();
    QSize sizeHint() const override;
    void setInfo(std::shared_ptr<AuthInfo> auth_info);
    void setInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void setInfo(int uid, QString name, QString icon);
    void showRedPoint(bool show = false);

  private:
    Ui::ContactUserItem* ui;
    std::shared_ptr<UserInfo> _info;
};

#endif  // CONTACTUSERITEM_H
