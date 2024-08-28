#include "chatuserlist.h"

ChatUserList::ChatUserList(QWidget* parent) : QListWidget(parent) {
    // 关闭横向、纵向滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject* watched, QEvent* event) {
    // 检查事件是否是鼠标进入或离开
    if(watched == viewport()){
        if(event->type() == QEvent::Enter){
            // 鼠标进入，显示滚动条
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            // 鼠标离开，隐藏滚动条
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

    }

    // 检查是否是鼠标滚轮事件
    if(watched == viewport() && event->type() == QEvent::Wheel){
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int num_degrees = wheelEvent->angleDelta().y()/8;
        // 计算滚动步数
        int num_steps = num_degrees/15;
        // 设置滚动幅度
        verticalScrollBar()->setValue(verticalScrollBar()->value() - num_steps);

        // 检查是否滚动到底部
        QScrollBar* scroll_bar = verticalScrollBar();
        int max_scroll_value = scroll_bar->maximum();
        int current_value = scroll_bar->value();

        if(max_scroll_value<=current_value){
            // 滚动到底部，加载新联系人
            // qDebug()<<"Load more chat user";
            // 发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_chat_user();
        }
        // 停止事件传递
        return true;
    }

    return QListWidget::eventFilter(watched,event);
}
