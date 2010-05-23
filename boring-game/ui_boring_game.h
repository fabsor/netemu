/********************************************************************************
** Form generated from reading UI file 'boring_game.ui'
**
** Created: Sun May 23 17:52:10 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BORING_GAME_H
#define UI_BORING_GAME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_boring_gameClass
{
public:
    QAction *actionExit;
    QWidget *centralwidget;
    QLineEdit *lineEdit;
    QLabel *nameLabel;
    QListWidget *playerList;
    QLabel *nameLabel_2;
    QGroupBox *groupBox;
    QLabel *nameLabel_3;
    QLineEdit *lineEdit_2;
    QPushButton *playButton;
    QGroupBox *groupBox_2;
    QLabel *nameLabel_4;
    QLineEdit *joinHostPort;
    QPushButton *joinButton;
    QLineEdit *joinConnectPort;
    QLabel *nameLabel_5;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *boring_gameClass)
    {
        if (boring_gameClass->objectName().isEmpty())
            boring_gameClass->setObjectName(QString::fromUtf8("boring_gameClass"));
        boring_gameClass->resize(800, 600);
        actionExit = new QAction(boring_gameClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        centralwidget = new QWidget(boring_gameClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(10, 30, 181, 27));
        nameLabel = new QLabel(centralwidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
        nameLabel->setGeometry(QRect(10, 10, 81, 17));
        playerList = new QListWidget(centralwidget);
        playerList->setObjectName(QString::fromUtf8("playerList"));
        playerList->setGeometry(QRect(400, 30, 381, 192));
        nameLabel_2 = new QLabel(centralwidget);
        nameLabel_2->setObjectName(QString::fromUtf8("nameLabel_2"));
        nameLabel_2->setGeometry(QRect(400, 10, 91, 17));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 260, 301, 191));
        nameLabel_3 = new QLabel(groupBox);
        nameLabel_3->setObjectName(QString::fromUtf8("nameLabel_3"));
        nameLabel_3->setGeometry(QRect(10, 30, 81, 17));
        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(10, 50, 113, 27));
        playButton = new QPushButton(groupBox);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        playButton->setGeometry(QRect(10, 100, 92, 27));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(480, 250, 301, 191));
        nameLabel_4 = new QLabel(groupBox_2);
        nameLabel_4->setObjectName(QString::fromUtf8("nameLabel_4"));
        nameLabel_4->setGeometry(QRect(10, 30, 81, 17));
        joinHostPort = new QLineEdit(groupBox_2);
        joinHostPort->setObjectName(QString::fromUtf8("joinHostPort"));
        joinHostPort->setGeometry(QRect(10, 50, 113, 27));
        joinButton = new QPushButton(groupBox_2);
        joinButton->setObjectName(QString::fromUtf8("joinButton"));
        joinButton->setGeometry(QRect(10, 100, 92, 27));
        joinConnectPort = new QLineEdit(groupBox_2);
        joinConnectPort->setObjectName(QString::fromUtf8("joinConnectPort"));
        joinConnectPort->setGeometry(QRect(140, 50, 113, 27));
        nameLabel_5 = new QLabel(groupBox_2);
        nameLabel_5->setObjectName(QString::fromUtf8("nameLabel_5"));
        nameLabel_5->setGeometry(QRect(140, 30, 81, 17));
        boring_gameClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(boring_gameClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 23));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        boring_gameClass->setMenuBar(menubar);
        statusbar = new QStatusBar(boring_gameClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        boring_gameClass->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionExit);

        retranslateUi(boring_gameClass);

        QMetaObject::connectSlotsByName(boring_gameClass);
    } // setupUi

    void retranslateUi(QMainWindow *boring_gameClass)
    {
        boring_gameClass->setWindowTitle(QApplication::translate("boring_gameClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("boring_gameClass", "Exit", 0, QApplication::UnicodeUTF8));
        nameLabel->setText(QApplication::translate("boring_gameClass", "Your name", 0, QApplication::UnicodeUTF8));
        nameLabel_2->setText(QApplication::translate("boring_gameClass", "Other players", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("boring_gameClass", "Host Game", 0, QApplication::UnicodeUTF8));
        nameLabel_3->setText(QApplication::translate("boring_gameClass", "Host Port", 0, QApplication::UnicodeUTF8));
        playButton->setText(QApplication::translate("boring_gameClass", "Host", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("boring_gameClass", "Join Game", 0, QApplication::UnicodeUTF8));
        nameLabel_4->setText(QApplication::translate("boring_gameClass", "Host Port", 0, QApplication::UnicodeUTF8));
        joinButton->setText(QApplication::translate("boring_gameClass", "Join", 0, QApplication::UnicodeUTF8));
        nameLabel_5->setText(QApplication::translate("boring_gameClass", "Connect port", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("boring_gameClass", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class boring_gameClass: public Ui_boring_gameClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BORING_GAME_H
