#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QMap>
#include <QSet>
#include <QTimer>
#include <functional>

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
    void on_get_code_clicked();
    void on_sure_btn_clicked();

    void on_return_btn_clicked();

    void on_cancel_btn_clicked();

  public slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);

  private:
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVerifyValid();
    bool checkConfirmValid();
    void initHttpHandlers();
    void addTipErr(TipErr te, QString tips);
    void delTipErr(TipErr te);
    void changeTipPage();
    Ui::RegisterDialog* ui;
    void showTip(QString str, bool b_ok);
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    QTimer* _countdown_timer;
    int _countdown;
  signals:
    void sigSwitchLogin();

  public:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif  // REGISTERDIALOG_H
