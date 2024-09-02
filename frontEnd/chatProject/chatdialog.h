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

#include "global.h"
#include "statewidget.h"

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
  private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif  // CHATDIALOG_H
