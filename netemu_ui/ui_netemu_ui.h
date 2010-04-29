/********************************************************************************
** Form generated from reading UI file 'netemu_ui.ui'
**
** Created: Thu Apr 29 10:05:28 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NETEMU_UI_H
#define UI_NETEMU_UI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_netemu_uiClass
{
public:

    void setupUi(QWidget *netemu_uiClass)
    {
        if (netemu_uiClass->objectName().isEmpty())
            netemu_uiClass->setObjectName(QString::fromUtf8("netemu_uiClass"));
        netemu_uiClass->resize(400, 300);

        retranslateUi(netemu_uiClass);

        QMetaObject::connectSlotsByName(netemu_uiClass);
    } // setupUi

    void retranslateUi(QWidget *netemu_uiClass)
    {
        netemu_uiClass->setWindowTitle(QApplication::translate("netemu_uiClass", "netemu_ui", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class netemu_uiClass: public Ui_netemu_uiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NETEMU_UI_H
