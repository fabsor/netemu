/********************************************************************************
** Form generated from reading UI file 'connectdialog.ui'
**
** Created: Wed May 12 20:33:42 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTDIALOG_H
#define UI_CONNECTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_WorkingDialogClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelConnecting;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *buttonCancel;

    void setupUi(QDialog *WorkingDialogClass)
    {
        if (WorkingDialogClass->objectName().isEmpty())
            WorkingDialogClass->setObjectName(QString::fromUtf8("WorkingDialogClass"));
        WorkingDialogClass->resize(419, 119);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WorkingDialogClass->sizePolicy().hasHeightForWidth());
        WorkingDialogClass->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(WorkingDialogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        labelConnecting = new QLabel(WorkingDialogClass);
        labelConnecting->setObjectName(QString::fromUtf8("labelConnecting"));

        horizontalLayout_2->addWidget(labelConnecting);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonCancel = new QPushButton(WorkingDialogClass);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
        sizePolicy.setHeightForWidth(buttonCancel->sizePolicy().hasHeightForWidth());
        buttonCancel->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(WorkingDialogClass);

        QMetaObject::connectSlotsByName(WorkingDialogClass);
    } // setupUi

    void retranslateUi(QDialog *WorkingDialogClass)
    {
        WorkingDialogClass->setWindowTitle(QApplication::translate("WorkingDialogClass", "WorkingDialog", 0, QApplication::UnicodeUTF8));
        labelConnecting->setText(QApplication::translate("WorkingDialogClass", "Connecting...", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("WorkingDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WorkingDialogClass: public Ui_WorkingDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTDIALOG_H
