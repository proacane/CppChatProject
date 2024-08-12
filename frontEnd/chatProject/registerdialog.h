#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
    class RegisterDialog;
}

class RegisterDialog : public QDialog {
    Q_OBJECT

  public:
    explicit RegisterDialog(QWidget* parent = nullptr);
    ~RegisterDialog();

private slots:
    // 获取验证码
    void on_btn_getCode_clicked();

private:
    Ui::RegisterDialog* ui;
    // 显示错误信息
    void showTip(const QString &tip);
};

#endif  // REGISTERDIALOG_H
