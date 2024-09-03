#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H
/*****************************************************************************
 * @file         friendlabel.h
 * @brief
 * @description  添加好友时创建新的标签
 * @author       ACA
 * @date         2024/09/03
 * @history
 *****************************************************************************/
#include <QFrame>

namespace Ui {
class FriendLabel;
}

class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();

    void setText(QString text);
    int width();
    int height();
    QString text();
private:
    Ui::FriendLabel *ui;
    QString _text;
    int _width;
    int _height;

public slots:
    void slot_close();
signals:
    void sig_close(QString );
};

#endif // FRIENDLABEL_H
