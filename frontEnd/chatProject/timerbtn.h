#ifndef TIMERBTN_H
#define TIMERBTN_H
/*****************************************************************************
 * @file         timerbtn.h
 * @brief
 * @description  获取验证码倒计时的按钮
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QPushButton>
#include <QTimer>
class TimerBtn : public QPushButton {
    Q_OBJECT
  public:
    TimerBtn(QWidget* parent = nullptr);
    ~TimerBtn();
    // 鼠标抬起
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    QTimer* _timer;
      // 倒计时
    int _counter;
};

#endif  // TIMERBTN_H
