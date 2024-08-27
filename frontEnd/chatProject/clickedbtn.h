#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H

#include <QPushButton>

class ClickedBtn : public QPushButton {
    Q_OBJECT
  public:
    ClickedBtn(QWidget* parent = nullptr);
    ~ClickedBtn();
    void setState(QString nomal, QString hover, QString press);

    // QWidget interface
  protected:
    // 鼠标点击
    virtual void mousePressEvent(QMouseEvent* event) override;
    // 鼠标释放
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    // 鼠标进入
    virtual void enterEvent(QEnterEvent* event) override;
    // 鼠标离开
    virtual void leaveEvent(QEvent* event) override;

  private:
    // 存储每个状态的图片路径
    QString _normal;
    QString _hover;
    QString _press;
};

#endif  // CLICKEDBTN_H
