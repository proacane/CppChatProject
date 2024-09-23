#ifndef CHATDIALOG_H
#define CHATDIALOG_H
/*****************************************************************************
 * @file         chatdialog.h
 * @brief
 * @description  聊天主界面
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QDialog>
#include <QListWidgetItem>

#include "global.h"
#include "statewidget.h"
#include "userdata.h"

namespace Ui {
    class ChatDialog;
}

class ChatDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ChatDialog(QWidget* parent = nullptr);
    ~ChatDialog();
    // 加载聊天列表
    void addChatUserList();

  private:
    Ui::ChatDialog* ui;
    // 在侧边栏切换
    ChatUIMode _mode;
    // 在搜索栏切换
    ChatUIMode _state;
    bool _b_loading;
    void showSearchList(bool b_show);

    void addLBGroup(StateWidget* lb);
    // 侧边栏组
    QList<StateWidget*> _lb_list;
    // 清楚侧边栏的状态
    void clearLabelState(StateWidget* lb);
    // 处理鼠标点击事件
    void handleGlobalMousePress(QMouseEvent* mouseEvent);

    // 已经添加的聊天列表
    QMap<int, QListWidgetItem*> _chat_items_added;
    // 当天聊天的好友uid
    int _cur_chat_uid;
  private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);

    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);

    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void slot_item_clicked(QListWidgetItem* item);
  public:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif  // CHATDIALOG_H
