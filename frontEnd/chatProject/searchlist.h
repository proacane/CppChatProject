#ifndef SEARCHLIST_H
#define SEARCHLIST_H
/*****************************************************************************
 * @file         searchlist.h
 * @brief
 * @description  搜索列表
 * @author       ACA
 * @date         2024/08/31
 * @history
 *****************************************************************************/
#include <QEvent>
#include <QListWidget>
#include <QScrollBar>
#include <QWidget>
#include <memory>

#include "loadingdialog.h"
#include "userdata.h"


class SearchList : public QListWidget {
    Q_OBJECT
  public:
    SearchList(QWidget* parent = nullptr);
    void closeFindDlg();
    void setSearchEdit(QWidget* edit);

    // QObject interface
  public:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;

  private:
    void waitPending(bool pending = true);
    bool _send_pending;
    void addTipItem();
    std::shared_ptr<QDialog> _find_dialog;
    QWidget* _search_edit;
    LoadingDialog* _loadingDialog;
  private slots:
    void slot_item_clicked(QListWidgetItem* item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);
};

#endif  // SEARCHLIST_H
