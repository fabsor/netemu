/********************************************************************************
** Form generated from reading UI file 'inputdialog.ui'
**
** Created: Wed May 12 12:28:43 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INPUTDIALOG_H
#define UI_INPUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InputDialogClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelInfo;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *textAddress;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonOK;
    QPushButton *buttonCancel;

    void setupUi(QDialog *InputDialogClass)
    {
        if (InputDialogClass->objectName().isEmpty())
            InputDialogClass->setObjectName(QString::fromUtf8("InputDialogClass"));
        InputDialogClass->resize(362, 108);
        verticalLayout = new QVBoxLayout(InputDialogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelInfo = new QLabel(InputDialogClass);
        labelInfo->setObjectName(QString::fromUtf8("labelInfo"));

        horizontalLayout_2->addWidget(labelInfo);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        textAddress = new QLineEdit(InputDialogClass);
        textAddress->setObjectName(QString::fromUtf8("textAddress"));

        verticalLayout->addWidget(textAddress);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonOK = new QPushButton(InputDialogClass);
        buttonOK->setObjectName(QString::fromUtf8("buttonOK"));

        horizontalLayout->addWidget(buttonOK);

        buttonCancel = new QPushButton(InputDialogClass);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(InputDialogClass);
        QObject::connect(buttonOK, SIGNAL(clicked()), InputDialogClass, SLOT(accept()));
        QObject::connect(buttonCancel, SIGNAL(clicked()), InputDialogClass, SLOT(reject()));

        QMetaObject::connectSlotsByName(InputDialogClass);
    } // setupUi

    void retranslateUi(QDialog *InputDialogClass)
    {
        InputDialogClass->setWindowTitle(QApplication::translate("InputDialogClass", "InputDialog", 0, QApplication::UnicodeUTF8));
        labelInfo->setText(QApplication::translate("InputDialogClass", "Input:", 0, QApplication::UnicodeUTF8));
        buttonOK->setText(QApplication::translate("InputDialogClass", "OK", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("InputDialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class InputDialogClass: public Ui_InputDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INPUTDIALOG_H
