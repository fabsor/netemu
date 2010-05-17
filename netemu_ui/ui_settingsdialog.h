/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created: Mon May 17 13:09:24 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_preferencesdialogClass
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listSettings;
    QStackedWidget *stackedWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonOK;
    QPushButton *buttonCancel;

    void setupUi(QDialog *preferencesdialogClass)
    {
        if (preferencesdialogClass->objectName().isEmpty())
            preferencesdialogClass->setObjectName(QString::fromUtf8("preferencesdialogClass"));
        preferencesdialogClass->resize(739, 485);
        verticalLayout = new QVBoxLayout(preferencesdialogClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        listSettings = new QListWidget(preferencesdialogClass);
        new QListWidgetItem(listSettings);
        listSettings->setObjectName(QString::fromUtf8("listSettings"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listSettings->sizePolicy().hasHeightForWidth());
        listSettings->setSizePolicy(sizePolicy);
        listSettings->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_2->addWidget(listSettings);

        stackedWidget = new QStackedWidget(preferencesdialogClass);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setFrameShadow(QFrame::Raised);

        horizontalLayout_2->addWidget(stackedWidget);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonOK = new QPushButton(preferencesdialogClass);
        buttonOK->setObjectName(QString::fromUtf8("buttonOK"));

        horizontalLayout->addWidget(buttonOK);

        buttonCancel = new QPushButton(preferencesdialogClass);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(preferencesdialogClass);
        QObject::connect(buttonOK, SIGNAL(clicked()), preferencesdialogClass, SLOT(accept()));
        QObject::connect(buttonCancel, SIGNAL(clicked()), preferencesdialogClass, SLOT(reject()));

        listSettings->setCurrentRow(0);


        QMetaObject::connectSlotsByName(preferencesdialogClass);
    } // setupUi

    void retranslateUi(QDialog *preferencesdialogClass)
    {
        preferencesdialogClass->setWindowTitle(QApplication::translate("preferencesdialogClass", "preferencesdialog", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listSettings->isSortingEnabled();
        listSettings->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listSettings->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("preferencesdialogClass", "Master kaillera server", 0, QApplication::UnicodeUTF8));
        listSettings->setSortingEnabled(__sortingEnabled);

        buttonOK->setText(QApplication::translate("preferencesdialogClass", "OK", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("preferencesdialogClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class preferencesdialogClass: public Ui_preferencesdialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
