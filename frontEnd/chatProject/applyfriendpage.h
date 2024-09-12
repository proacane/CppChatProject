#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QJsonArray>
#include <QWidget>
#include <memory>
#include <unordered_map>
#include"applyfrienditem.h"
#include "userdata.h"
namespace Ui {
    class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget {
    Q_OBJECT

  public:
    explicit ApplyFriendPage(QWidget* parent = nullptr);
    ~ApplyFriendPage();
    // 添加新 item
    void addNewApply(std::shared_ptr<AddFriendApply> apply);

  private:
    Ui::ApplyFriendPage* ui;
    void loadApplyList();
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;


  protected:
    virtual void paintEvent(QPaintEvent* event) override;

  public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
  signals:
    void sig_show_search(bool);
};

#endif  // APPLYFRIENDPAGE_H
