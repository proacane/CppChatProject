#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>

#include "userdata.h"
namespace Ui {
    class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog {
    Q_OBJECT

  public:
    explicit FindSuccessDlg(QWidget* parent = nullptr);
    ~FindSuccessDlg();
    // 设置用户信息
    void setSearchInfo(std::shared_ptr<SearchInfo> si);

private slots:
    void on_btn_add_friend_clicked();

private:
    Ui::FindSuccessDlg* ui;
    std::shared_ptr<SearchInfo> _si;
    QWidget* _parent;

};

#endif  // FINDSUCCESSDLG_H
