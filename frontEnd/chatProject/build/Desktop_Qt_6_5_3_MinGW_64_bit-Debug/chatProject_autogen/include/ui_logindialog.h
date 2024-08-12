/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *userhorizontalLayout;
    QLabel *lab_user;
    QLineEdit *edit_user;
    QHBoxLayout *pwdhorizontalLayout;
    QLabel *lab_password;
    QLineEdit *edit_password;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_5;
    QLabel *lab_forgetPwd;
    QHBoxLayout *loginhorizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btn_login;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *reghorizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btn_register;
    QSpacerItem *horizontalSpacer_4;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(300, 500);
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(10, -1, 10, 5);
        widget = new QWidget(LoginDialog);
        widget->setObjectName("widget");
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(widget);
        label->setObjectName("label");
        label->setMinimumSize(QSize(200, 200));
        label->setMaximumSize(QSize(200, 200));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/icon.png")));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);


        verticalLayout_4->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        userhorizontalLayout = new QHBoxLayout();
        userhorizontalLayout->setObjectName("userhorizontalLayout");
        lab_user = new QLabel(LoginDialog);
        lab_user->setObjectName("lab_user");

        userhorizontalLayout->addWidget(lab_user);

        edit_user = new QLineEdit(LoginDialog);
        edit_user->setObjectName("edit_user");
        edit_user->setMinimumSize(QSize(25, 25));

        userhorizontalLayout->addWidget(edit_user);


        verticalLayout_4->addLayout(userhorizontalLayout);

        pwdhorizontalLayout = new QHBoxLayout();
        pwdhorizontalLayout->setObjectName("pwdhorizontalLayout");
        lab_password = new QLabel(LoginDialog);
        lab_password->setObjectName("lab_password");

        pwdhorizontalLayout->addWidget(lab_password);

        edit_password = new QLineEdit(LoginDialog);
        edit_password->setObjectName("edit_password");
        edit_password->setMinimumSize(QSize(25, 25));

        pwdhorizontalLayout->addWidget(edit_password);


        verticalLayout_4->addLayout(pwdhorizontalLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        lab_forgetPwd = new QLabel(LoginDialog);
        lab_forgetPwd->setObjectName("lab_forgetPwd");
        lab_forgetPwd->setMinimumSize(QSize(30, 30));

        horizontalLayout->addWidget(lab_forgetPwd);


        verticalLayout_4->addLayout(horizontalLayout);

        loginhorizontalLayout = new QHBoxLayout();
        loginhorizontalLayout->setObjectName("loginhorizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        loginhorizontalLayout->addItem(horizontalSpacer_2);

        btn_login = new QPushButton(LoginDialog);
        btn_login->setObjectName("btn_login");
        btn_login->setMinimumSize(QSize(30, 30));

        loginhorizontalLayout->addWidget(btn_login);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        loginhorizontalLayout->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(loginhorizontalLayout);

        reghorizontalLayout = new QHBoxLayout();
        reghorizontalLayout->setObjectName("reghorizontalLayout");
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        reghorizontalLayout->addItem(horizontalSpacer_3);

        btn_register = new QPushButton(LoginDialog);
        btn_register->setObjectName("btn_register");
        btn_register->setMinimumSize(QSize(30, 30));

        reghorizontalLayout->addWidget(btn_register);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        reghorizontalLayout->addItem(horizontalSpacer_4);


        verticalLayout_4->addLayout(reghorizontalLayout);


        verticalLayout->addLayout(verticalLayout_4);


        retranslateUi(LoginDialog);

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "Dialog", nullptr));
        label->setText(QString());
        lab_user->setText(QCoreApplication::translate("LoginDialog", "\347\224\250\346\210\267\357\274\232", nullptr));
        lab_password->setText(QCoreApplication::translate("LoginDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        lab_forgetPwd->setText(QCoreApplication::translate("LoginDialog", "\345\277\230\350\256\260\345\257\206\347\240\201", nullptr));
        btn_login->setText(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
        btn_register->setText(QCoreApplication::translate("LoginDialog", "\346\263\250\345\206\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
