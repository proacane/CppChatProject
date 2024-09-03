#include "applyfrienddialog.h"

#include <QEvent>
#include <QKeyEvent>

#include "ui_applyfrienddialog.h"
#include"global.h"

ApplyFriendDialog::ApplyFriendDialog(QWidget* parent) : QDialog(parent), ui(new Ui::ApplyFriendDialog) {
    ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFriend");
    this->setModal(true);
    ui->edit_name->setPlaceholderText(tr("恋恋风辰"));
    ui->lab_edit->setPlaceholderText("搜索、添加标签");
    ui->edit_back->setPlaceholderText("燃烧的胸毛");
    ui->lab_edit->setMaxLength(21);
    ui->lab_edit->move(2, 2);
    ui->lab_edit->setFixedHeight(20);
    // ui->lab_edit->setMaxLength(10);
    ui->inputWidget->hide();
    _tip_cur_point = QPoint(5, 5);
    _tip_data = {"同学",          "家人",           "菜鸟教程",       "C++ Primer",
                 "Rust 程序设计", "父与子学Python", "nodejs开发指南", "go 语言开发指南",
                 "游戏伙伴",      "金融投资",       "微信读书",       "拼多多拼友"};
    connect(ui->more_lab, &ClickedOnceLabel::clicked, this, &ApplyFriendDialog::slot_show_more_label);
    initTipLbs();
    // 链接输入标签回车事件
    connect(ui->lab_edit, &CustomizeEdit::returnPressed, this, &ApplyFriendDialog::slot_label_enter);
    connect(ui->lab_edit, &CustomizeEdit::textChanged, this, &ApplyFriendDialog::slot_label_text_change);
    connect(ui->lab_edit, &CustomizeEdit::editingFinished, this, &ApplyFriendDialog::slot_label_edit_finished);
    connect(ui->tip_lab, &ClickedOnceLabel::clicked, this, &ApplyFriendDialog::slot_add_friendlabel_by_click_tip);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->installEventFilter(this);
    ui->btn_ok->setState("normal", "hover", "press");
    ui->btn_cancel->setState("normal", "hover", "press");
    // 连接确认和取消按钮的槽函数
    connect(ui->btn_cancel, &QPushButton::clicked, this, &ApplyFriendDialog::slot_apply_cancel);
    connect(ui->btn_ok, &QPushButton::clicked, this, &ApplyFriendDialog::slot_apply_ok);
}

ApplyFriendDialog::~ApplyFriendDialog() {
    delete ui;
}

void ApplyFriendDialog::initTipLbs() {
}

void ApplyFriendDialog::addTipLbs(ClickedLabel*, QPoint cur_point, QPoint& next_point, int text_width,
                                  int text_height) {
}

void ApplyFriendDialog::setSearchInfo(std::shared_ptr<SearchInfo> si) {
}

bool ApplyFriendDialog::eventFilter(QObject* obj, QEvent* event) {
    // if (event->type() == Qt::EnterKeyType) {
    //     return true;
    // }
    if (obj == ui->scrollArea && event->type() == QEvent::Enter) {
        // ui->scrollArea->verticalScrollBar()->setHidden(false);
        // ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else if (obj == ui->scrollArea && event->type() == QEvent::Leave) {
        // ui->scrollArea->verticalScrollBar()->setHidden(true);
        ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    // 处理 Enter 键事件
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            // 将事件传递给 ui->lab_edit 处理
            QCoreApplication::sendEvent(ui->lab_edit, event);
            return true;  // 事件已处理
        }
    }
    return QObject::eventFilter(obj, event);
}

void ApplyFriendDialog::resetLabels() {
}

void ApplyFriendDialog::addLabel(QString name) {
    // 标签是否以及存在
    if (_friend_labels.find(name) != _friend_labels.end()) {
        return;
    }
    auto tmplabel = new FriendLabel(ui->gridWidget);
    tmplabel->setText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = ui->gridWidget->width();

    if (_label_point.x() + tmplabel->width() > max_width) {
        _label_point.setY(_label_point.y() + tmplabel->height() + 6);
        _label_point.setX(2);
    } else {
        // todo... 添加宽度统计
    }
    // 移动到该位置
    tmplabel->move(_label_point);
    tmplabel->show();
    _friend_labels[tmplabel->text()] = tmplabel;
    _friend_label_keys.push_back(tmplabel->text());
    connect(tmplabel, &FriendLabel::sig_close, this, &ApplyFriendDialog::slot_remove_friend_label);

    _label_point.setX(_label_point.x() + tmplabel->width() + 2);
    if (_label_point.x() + MIN_APPLY_LABEL_ED_LEN > ui->gridWidget->width()) {
        ui->lab_edit->move(2, _label_point.y() + tmplabel->height() + 2);
    } else {
        ui->lab_edit->move(_label_point);
    }

    ui->lab_edit->clear();
    if (ui->gridWidget->height() < _label_point.y() + tmplabel->height() + 2) {
        ui->gridWidget->setFixedHeight(_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

void ApplyFriendDialog::slot_show_more_label() {
}

void ApplyFriendDialog::slot_label_enter() {
    if (ui->lab_edit->text().isEmpty()) {
        return;
    }
    addLabel(ui->lab_edit->text());
    ui->inputWidget->hide();
}

void ApplyFriendDialog::slot_remove_friend_label(QString) {
}

void ApplyFriendDialog::slot_change_friendlabel_by_tip(QString, ClickLbState) {
}

void ApplyFriendDialog::slot_label_text_change(const QString& text) {
}

void ApplyFriendDialog::slot_label_edit_finished() {
}

void ApplyFriendDialog::slot_add_friendlabel_by_click_tip(QString text) {
}

void ApplyFriendDialog::slot_apply_ok() {
}

void ApplyFriendDialog::slot_apply_cancel() {
}
