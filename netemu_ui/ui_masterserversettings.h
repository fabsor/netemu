/********************************************************************************
** Form generated from reading UI file 'masterserversettings.ui'
**
** Created: Mon May 24 10:37:18 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MASTERSERVERSETTINGS_H
#define UI_MASTERSERVERSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MasterServerPreferencesClass
{
public:
    QLabel *labelMasterAddress;
    QLineEdit *textMasterAddress;

    void setupUi(QWidget *MasterServerPreferencesClass)
    {
        if (MasterServerPreferencesClass->objectName().isEmpty())
            MasterServerPreferencesClass->setObjectName(QString::fromUtf8("MasterServerPreferencesClass"));
        MasterServerPreferencesClass->resize(400, 300);
        labelMasterAddress = new QLabel(MasterServerPreferencesClass);
        labelMasterAddress->setObjectName(QString::fromUtf8("labelMasterAddress"));
        labelMasterAddress->setGeometry(QRect(10, 10, 61, 17));
        textMasterAddress = new QLineEdit(MasterServerPreferencesClass);
        textMasterAddress->setObjectName(QString::fromUtf8("textMasterAddress"));
        textMasterAddress->setGeometry(QRect(10, 30, 241, 27));

        retranslateUi(MasterServerPreferencesClass);

        QMetaObject::connectSlotsByName(MasterServerPreferencesClass);
    } // setupUi

    void retranslateUi(QWidget *MasterServerPreferencesClass)
    {
        MasterServerPreferencesClass->setWindowTitle(QApplication::translate("MasterServerPreferencesClass", "MasterServerPreferences", 0, QApplication::UnicodeUTF8));
        labelMasterAddress->setText(QApplication::translate("MasterServerPreferencesClass", "Address:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MasterServerPreferencesClass: public Ui_MasterServerPreferencesClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MASTERSERVERSETTINGS_H
