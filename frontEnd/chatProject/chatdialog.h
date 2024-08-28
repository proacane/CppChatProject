#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

#include "global.h"

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
};

#endif  // CHATDIALOG_H
