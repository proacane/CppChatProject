#ifndef RESETDIALOG_H
#define RESETDIALOG_H
/*****************************************************************************
 * @file         resetdialog.h
 * @brief
 * @description  重置密码界面
 * @author       ACA
 * @date         2024/08/29
 * @history
 *****************************************************************************/
#include <QDialog>

#include "global.h"
namespace Ui {
    class ResetDialog;
}

class ResetDialog : public QDialog {
    Q_OBJECT

  public:
    explicit ResetDialog(QWidget* parent = nullptr);
    ~ResetDialog();

  private:
    Ui::ResetDialog* ui;
    // 显示信息
    void showTip(const QString& tip, bool is_error);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVerifyValid();
    QMap<TipErr, QString> _tip_errs;
    void addTipErr(TipErr err, const QString& tips);
    void delTipErr(TipErr err);
    // 初始化 http 请求处理函数
    void initHttpHandlers();
    // 存储对不同请求的处理函数
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

  signals:
    void sigSwitchLogin();
  private slots:
    void on_btn_getCode_clicked();
    // 处理返回的http结果
    void slotResetModFinish(ReqId id, QString res, ErrorCodes err);
    void on_btn_ok_clicked();
    void on_btn_cancel_clicked();

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif  // RESETDIALOG_H
