/********************************************************************************
** Form generated from reading UI file 'serverdialog.ui'
**
** Created: Fri May 7 14:41:34 2010
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
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerDialogClass
{
public:
    QAction *actionExit;
    QAction *actionSettings;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupPartyLine;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTableWidget *tableUsers;
    QPlainTextEdit *plainTextChat;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *textSendChat;
    QPushButton *buttonSendChat;
    QGroupBox *groupGames;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tableUsers_2;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonJoinGame;
    QPushButton *buttonCreateGame;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ServerDialogClass)
    {
        if (ServerDialogClass->objectName().isEmpty())
            ServerDialogClass->setObjectName(QString::fromUtf8("ServerDialogClass"));
        ServerDialogClass->resize(800, 600);
        actionExit = new QAction(ServerDialogClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionSettings = new QAction(ServerDialogClass);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionAbout = new QAction(ServerDialogClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralwidget = new QWidget(ServerDialogClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupPartyLine = new QGroupBox(centralwidget);
        groupPartyLine->setObjectName(QString::fromUtf8("groupPartyLine"));
        groupPartyLine->setFlat(false);
        groupPartyLine->setCheckable(false);
        verticalLayout = new QVBoxLayout(groupPartyLine);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tableUsers = new QTableWidget(groupPartyLine);
        if (tableUsers->columnCount() < 3)
            tableUsers->setColumnCount(3);
        QFont font;
        font.setUnderline(false);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setFont(font);
        tableUsers->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableUsers->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableUsers->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableUsers->setObjectName(QString::fromUtf8("tableUsers"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tableUsers->sizePolicy().hasHeightForWidth());
        tableUsers->setSizePolicy(sizePolicy);
        tableUsers->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableUsers->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableUsers->verticalHeader()->setVisible(false);

        horizontalLayout->addWidget(tableUsers);

        plainTextChat = new QPlainTextEdit(groupPartyLine);
        plainTextChat->setObjectName(QString::fromUtf8("plainTextChat"));

        horizontalLayout->addWidget(plainTextChat);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        textSendChat = new QLineEdit(groupPartyLine);
        textSendChat->setObjectName(QString::fromUtf8("textSendChat"));

        horizontalLayout_2->addWidget(textSendChat);

        buttonSendChat = new QPushButton(groupPartyLine);
        buttonSendChat->setObjectName(QString::fromUtf8("buttonSendChat"));

        horizontalLayout_2->addWidget(buttonSendChat);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_3->addWidget(groupPartyLine);

        groupGames = new QGroupBox(centralwidget);
        groupGames->setObjectName(QString::fromUtf8("groupGames"));
        verticalLayout_2 = new QVBoxLayout(groupGames);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tableUsers_2 = new QTableWidget(groupGames);
        if (tableUsers_2->columnCount() < 5)
            tableUsers_2->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        __qtablewidgetitem3->setFont(font);
        tableUsers_2->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableUsers_2->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableUsers_2->setHorizontalHeaderItem(2, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableUsers_2->setHorizontalHeaderItem(3, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableUsers_2->setHorizontalHeaderItem(4, __qtablewidgetitem7);
        tableUsers_2->setObjectName(QString::fromUtf8("tableUsers_2"));
        tableUsers_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableUsers_2->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableUsers_2->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(tableUsers_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        buttonJoinGame = new QPushButton(groupGames);
        buttonJoinGame->setObjectName(QString::fromUtf8("buttonJoinGame"));
        buttonJoinGame->setEnabled(false);

        horizontalLayout_3->addWidget(buttonJoinGame);

        buttonCreateGame = new QPushButton(groupGames);
        buttonCreateGame->setObjectName(QString::fromUtf8("buttonCreateGame"));

        horizontalLayout_3->addWidget(buttonCreateGame);


        verticalLayout_2->addLayout(horizontalLayout_3);


        verticalLayout_3->addWidget(groupGames);

        ServerDialogClass->setCentralWidget(centralwidget);
        groupPartyLine->raise();
        groupGames->raise();
        tableUsers_2->raise();
        menubar = new QMenuBar(ServerDialogClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        ServerDialogClass->setMenuBar(menubar);
        statusbar = new QStatusBar(ServerDialogClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ServerDialogClass->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuEdit->addAction(actionSettings);
        menuHelp->addAction(actionAbout);

        retranslateUi(ServerDialogClass);

        QMetaObject::connectSlotsByName(ServerDialogClass);
    } // setupUi

    void retranslateUi(QMainWindow *ServerDialogClass)
    {
        ServerDialogClass->setWindowTitle(QApplication::translate("ServerDialogClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("ServerDialogClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionSettings->setText(QApplication::translate("ServerDialogClass", "Settings", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("ServerDialogClass", "About", 0, QApplication::UnicodeUTF8));
        groupPartyLine->setTitle(QApplication::translate("ServerDialogClass", "Partyline", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableUsers->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("ServerDialogClass", "Nick", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableUsers->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("ServerDialogClass", "Ping", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableUsers->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("ServerDialogClass", "Status", 0, QApplication::UnicodeUTF8));
        buttonSendChat->setText(QApplication::translate("ServerDialogClass", "Send chat", 0, QApplication::UnicodeUTF8));
        groupGames->setTitle(QApplication::translate("ServerDialogClass", "Games", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableUsers_2->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QApplication::translate("ServerDialogClass", "Game", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableUsers_2->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QApplication::translate("ServerDialogClass", "Version", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableUsers_2->horizontalHeaderItem(2);
        ___qtablewidgetitem5->setText(QApplication::translate("ServerDialogClass", "Owner", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableUsers_2->horizontalHeaderItem(3);
        ___qtablewidgetitem6->setText(QApplication::translate("ServerDialogClass", "Status", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableUsers_2->horizontalHeaderItem(4);
        ___qtablewidgetitem7->setText(QApplication::translate("ServerDialogClass", "# Users", 0, QApplication::UnicodeUTF8));
        buttonJoinGame->setText(QApplication::translate("ServerDialogClass", "Join game", 0, QApplication::UnicodeUTF8));
        buttonCreateGame->setText(QApplication::translate("ServerDialogClass", "Create Game", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("ServerDialogClass", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("ServerDialogClass", "Edit", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("ServerDialogClass", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ServerDialogClass: public Ui_ServerDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERDIALOG_H
