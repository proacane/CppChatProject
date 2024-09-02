#include "findsuccessdlg.h"

#include <QDir>

#include "ui_findsuccessdlg.h"

FindSuccessDlg::FindSuccessDlg(QWidget* parent) : QDialog(parent), ui(new Ui::FindSuccessDlg) {
    ui->setupUi(this);

    // 设置对话框标题
    setWindowTitle("添加");
    // 隐藏对话框标题栏
   setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path =
        QDir::toNativeSeparators(app_path + QDir::separator() + "static" + QDir::separator() + "head_2.jpg");

    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->lb_avatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->lb_avatar->setPixmap(head_pix);
    ui->lb_avatar->setText("");
    ui->btn_add_friend->setState("normal", "hover", "press");
    this->setModal(true);
}

FindSuccessDlg::~FindSuccessDlg() {
    delete ui;
}

void FindSuccessDlg::setSearchInfo(std::shared_ptr<SearchInfo> si) {
    ui->lb_name->setText(si->_name);
    _si = si;
}

void FindSuccessDlg::on_btn_add_friend_clicked() {
    // TODO 添加好友界面弹出
}
