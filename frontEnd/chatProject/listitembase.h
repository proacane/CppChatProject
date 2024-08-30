#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
/*****************************************************************************
 * @file         listitembase.h
 * @brief
 * @description  item 的基类
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QWidget>

#include "global.h"
class ListItemBase : public QWidget {
    Q_OBJECT
  public:
    explicit ListItemBase(QWidget* parent = nullptr);

    void setItemType(ListItemType itemType);
    ListItemType getItemType();

  private:
    ListItemType _itemType;
  signals:

      // QWidget interface
  protected:
      // virtual void paintEvent(QPaintEvent *event) override;
};

#endif  // LISTITEMBASE_H
