#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
/*****************************************************************************
 * @file         clickedlabel.h
 * @brief
 * @description  为标签添加四种状态图片
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include "global.h"
class ClickedLabel : public QLabel {
    Q_OBJECT
  public:
    ClickedLabel(QWidget* parent = nullptr);
    ~ClickedLabel()=default;

    /**
     * @brief setState
     * @param normal普通
     * @param hover悬浮
     * @param press点击
     * @param select选择
     * @param select_hover选择悬浮
     * @param select_press选择点击
     */
    void setState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    // 获取当前标签状态
    ClickLbState getCurState();

  private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    ClickLbState _curstate;
  signals:
    void clicked(void);

protected:
    // 鼠标点击
    virtual void mousePressEvent(QMouseEvent* event) override;
    // 鼠标悬停
    virtual void enterEvent(QEnterEvent* event) override;
    // 鼠标移走
    virtual void leaveEvent(QEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif  // CLICKEDLABEL_H
