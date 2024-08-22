#ifndef TIMERBTN_H
#define TIMERBTN_H
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
