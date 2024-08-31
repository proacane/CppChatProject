#ifndef STATEWIDGET_H
#define STATEWIDGET_H
/*****************************************************************************
 * @file         statewidget.h
 * @brief
 * @description  侧边栏
 * @author       ACA
 * @date         2024/08/31
 * @history
 *****************************************************************************/
#include <QLabel>
#include <QWidget>

#include "global.h"
class StateWidget : public QWidget {
    Q_OBJECT
  public:
    explicit StateWidget(QWidget* parent = nullptr);
    void setState(QString normal = "", QString hover = "", QString press = "", QString select = "",
                  QString select_hover = "", QString select_press = "");
    ClickLbState getCurState();
    // 恢复正常
    void clearState();
    // 设置选中
    void setSelected(bool bselected);
    // 添加红点
    void addRedPoint();
    // 显示红点
    void showRedPoint(bool show = true);

  protected:
    void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

  private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
    QLabel* _red_point;
  signals:
    void clicked(void);

  public slots:
};

#endif  // STATEWIDGET_H
