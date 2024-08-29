#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H
/*****************************************************************************
 * @file         loadingdialog.h
 * @brief
 * @description  加载界面
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QDialog>

namespace Ui {
    class LoadingDialog;
}

class LoadingDialog : public QDialog {
    Q_OBJECT

  public:
    explicit LoadingDialog(QWidget* parent = nullptr);
    ~LoadingDialog();

  private:
    Ui::LoadingDialog* ui;
};

#endif  // LOADINGDIALOG_H
