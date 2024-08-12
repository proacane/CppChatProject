#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

#include "global.h"
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
    // 处理 http 请求的响应
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
  private:
    Ui::RegisterDialog* ui;
    // 显示信息
    void showTip(const QString& tip,bool is_error);
    // 初始化 http 请求处理函数
    void initHttpHandlers();
    // 存储对不同请求的处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;


};

#endif  // REGISTERDIALOG_H
