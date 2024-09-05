#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H
/*****************************************************************************
 * @file         grouptipitem.h
 * @brief
 * @description  联系人分组item
 * @author       ACA
 * @date         2024/09/05
 * @history
 *****************************************************************************/
#include <QWidget>

#include "listitembase.h"
namespace Ui {
    class GroupTipItem;
}

class GroupTipItem : public ListItemBase {
    Q_OBJECT

  public:
    explicit GroupTipItem(QWidget* parent = nullptr);
    ~GroupTipItem();
    virtual QSize sizeHint() const override;
    // 设置分组信息
    void setGroupTip(QString str);

  private:
    QString _tip;
    Ui::GroupTipItem* ui;


    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif  // GROUPTIPITEM_H
