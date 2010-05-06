/********************************************************************************
** Form generated from reading UI file 'serverdialog.ui'
**
** Created: Thu May 6 02:40:04 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERDIALOG_H
#define UI_SERVERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerDialogClass
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ServerDialogClass)
    {
        if (ServerDialogClass->objectName().isEmpty())
            ServerDialogClass->setObjectName(QString::fromUtf8("ServerDialogClass"));
        ServerDialogClass->resize(800, 600);
        centralwidget = new QWidget(ServerDialogClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        ServerDialogClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ServerDialogClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 21));
        ServerDialogClass->setMenuBar(menubar);
        statusbar = new QStatusBar(ServerDialogClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ServerDialogClass->setStatusBar(statusbar);

        retranslateUi(ServerDialogClass);

        QMetaObject::connectSlotsByName(ServerDialogClass);
    } // setupUi

    void retranslateUi(QMainWindow *ServerDialogClass)
    {
        ServerDialogClass->setWindowTitle(QApplication::translate("ServerDialogClass", "MainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ServerDialogClass: public Ui_ServerDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERDIALOG_H
