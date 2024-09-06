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

    // TODO 测试数据
    std::vector<QString> strs = {"hello world !", "nice to meet u", "New year，new life", "You have to love yourself",
                                 "My love is written in the wind ever since the whole world is you"};
    std::vector<QString> heads = {":/images/head_1.jpg", ":/images/head_2.jpg", ":/images/head_3.jpg",
                                  ":/images/head_4.jpg", ":/images/head_5.jpg"};
    std::vector<QString> names = {"mmmm", "zack", "golang", "cpp", "java", "nodejs", "python", "rust"};
  protected:
    virtual void paintEvent(QPaintEvent* event) override;

  public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
  signals:
    void sig_show_search(bool);
};

#endif  // APPLYFRIENDPAGE_H
