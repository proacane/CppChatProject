/********************************************************************************
** Form generated from reading UI file 'registerdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTERDIALOG_H
#define UI_REGISTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RegisterDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QLabel *lab_errTip;
    QHBoxLayout *horizontalLayout;
    QLabel *lab_user;
    QLineEdit *edit_user;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lab_email;
    QLineEdit *edit_email;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lab_pwd;
    QLineEdit *edit_pwd;
    QHBoxLayout *horizontalLayout_4;
    QLabel *lab_confirm;
    QLineEdit *edit_confirm;
    QHBoxLayout *horizontalLayout_6;
    QLabel *lab_varify;
    QLineEdit *edit_varify;
    QPushButton *btn_getCode;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btn_ok;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_cancel;
    QSpacerItem *verticalSpacer_4;
    QWidget *page_2;

    void setupUi(QDialog *RegisterDialog)
    {
        if (RegisterDialog->objectName().isEmpty())
            RegisterDialog->setObjectName("RegisterDialog");
        RegisterDialog->resize(300, 500);
        verticalLayout_2 = new QVBoxLayout(RegisterDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        stackedWidget = new QStackedWidget(RegisterDialog);
        stackedWidget->setObjectName("stackedWidget");
        page = new QWidget();
        page->setObjectName("page");
        verticalLayout = new QVBoxLayout(page);
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        widget = new QWidget(page);
        widget->setObjectName("widget");
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        lab_errTip = new QLabel(widget);
        lab_errTip->setObjectName("lab_errTip");
        lab_errTip->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lab_errTip);


        verticalLayout->addWidget(widget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        lab_user = new QLabel(page);
        lab_user->setObjectName("lab_user");
        lab_user->setMinimumSize(QSize(30, 30));

        horizontalLayout->addWidget(lab_user);

        edit_user = new QLineEdit(page);
        edit_user->setObjectName("edit_user");
        edit_user->setMinimumSize(QSize(30, 30));

        horizontalLayout->addWidget(edit_user);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        lab_email = new QLabel(page);
        lab_email->setObjectName("lab_email");
        lab_email->setMinimumSize(QSize(30, 30));

        horizontalLayout_3->addWidget(lab_email);

        edit_email = new QLineEdit(page);
        edit_email->setObjectName("edit_email");
        edit_email->setMinimumSize(QSize(30, 30));

        horizontalLayout_3->addWidget(edit_email);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        lab_pwd = new QLabel(page);
        lab_pwd->setObjectName("lab_pwd");
        lab_pwd->setMinimumSize(QSize(30, 30));

        horizontalLayout_2->addWidget(lab_pwd);

        edit_pwd = new QLineEdit(page);
        edit_pwd->setObjectName("edit_pwd");
        edit_pwd->setMinimumSize(QSize(30, 30));

        horizontalLayout_2->addWidget(edit_pwd);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        lab_confirm = new QLabel(page);
        lab_confirm->setObjectName("lab_confirm");
        lab_confirm->setMinimumSize(QSize(30, 30));

        horizontalLayout_4->addWidget(lab_confirm);

        edit_confirm = new QLineEdit(page);
        edit_confirm->setObjectName("edit_confirm");
        edit_confirm->setMinimumSize(QSize(30, 30));

        horizontalLayout_4->addWidget(edit_confirm);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        lab_varify = new QLabel(page);
        lab_varify->setObjectName("lab_varify");
        lab_varify->setMinimumSize(QSize(30, 30));

        horizontalLayout_6->addWidget(lab_varify);

        edit_varify = new QLineEdit(page);
        edit_varify->setObjectName("edit_varify");
        edit_varify->setMinimumSize(QSize(30, 30));

        horizontalLayout_6->addWidget(edit_varify);

        btn_getCode = new QPushButton(page);
        btn_getCode->setObjectName("btn_getCode");
        btn_getCode->setMinimumSize(QSize(30, 30));

        horizontalLayout_6->addWidget(btn_getCode);


        verticalLayout->addLayout(horizontalLayout_6);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Preferred);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        btn_ok = new QPushButton(page);
        btn_ok->setObjectName("btn_ok");
        btn_ok->setMinimumSize(QSize(20, 30));

        horizontalLayout_7->addWidget(btn_ok);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        btn_cancel = new QPushButton(page);
        btn_cancel->setObjectName("btn_cancel");
        btn_cancel->setMinimumSize(QSize(20, 30));

        horizontalLayout_7->addWidget(btn_cancel);


        verticalLayout->addLayout(horizontalLayout_7);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_4);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        stackedWidget->addWidget(page_2);

        verticalLayout_2->addWidget(stackedWidget);


        retranslateUi(RegisterDialog);

        QMetaObject::connectSlotsByName(RegisterDialog);
    } // setupUi

    void retranslateUi(QDialog *RegisterDialog)
    {
        RegisterDialog->setWindowTitle(QCoreApplication::translate("RegisterDialog", "Dialog", nullptr));
        lab_errTip->setText(QCoreApplication::translate("RegisterDialog", "\351\224\231\350\257\257\346\217\220\347\244\272", nullptr));
        lab_user->setText(QCoreApplication::translate("RegisterDialog", "\347\224\250\346\210\267\357\274\232", nullptr));
        lab_email->setText(QCoreApplication::translate("RegisterDialog", "\351\202\256\347\256\261\357\274\232", nullptr));
        lab_pwd->setText(QCoreApplication::translate("RegisterDialog", "\345\257\206\347\240\201\357\274\232", nullptr));
        lab_confirm->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\350\256\244\357\274\232", nullptr));
        lab_varify->setText(QCoreApplication::translate("RegisterDialog", "\351\252\214\350\257\201\347\240\201\357\274\232", nullptr));
        btn_getCode->setText(QCoreApplication::translate("RegisterDialog", "\350\216\267\345\217\226", nullptr));
        btn_ok->setText(QCoreApplication::translate("RegisterDialog", "\347\241\256\345\256\232", nullptr));
        btn_cancel->setText(QCoreApplication::translate("RegisterDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RegisterDialog: public Ui_RegisterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTERDIALOG_H
