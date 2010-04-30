/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu 29. Apr 17:24:00 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainwindowClass
{
public:
    QAction *actionSettings;
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionSettings_2;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tableServers;
    QLabel *labelServerRefreshStatus;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonServerRefresh;
    QPushButton *buttonServerConnect;
    QWidget *tab_6;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableServers_2;
    QLabel *labelCloudRefreshStatus;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *buttonCloudRefresh;
    QPushButton *buttonCloudConnect;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_5;
    QTableWidget *tableServers_3;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *buttonRecentConnect;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_6;
    QTableWidget *tableServers_4;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *buttonFavoritesAdd;
    QPushButton *buttonFavoritesRemove;
    QPushButton *buttonFavoritesConnect;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *tableServers_5;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *buttonGameConnect;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *labelUsername;
    QLineEdit *lineEditUsername;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *buttonConnectIP;
    QPushButton *buttonAbout;
    QPushButton *buttonCancel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuEdit;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *mainwindowClass)
    {
        if (mainwindowClass->objectName().isEmpty())
            mainwindowClass->setObjectName(QString::fromUtf8("mainwindowClass"));
        mainwindowClass->resize(1122, 706);
        actionSettings = new QAction(mainwindowClass);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionAbout = new QAction(mainwindowClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionExit = new QAction(mainwindowClass);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionSettings_2 = new QAction(mainwindowClass);
        actionSettings_2->setObjectName(QString::fromUtf8("actionSettings_2"));
        centralwidget = new QWidget(mainwindowClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tableServers = new QTableWidget(tab);
        if (tableServers->columnCount() < 6)
            tableServers->setColumnCount(6);
        QFont font;
        font.setUnderline(false);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setFont(font);
        tableServers->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tableServers->setObjectName(QString::fromUtf8("tableServers"));
        tableServers->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(tableServers);

        labelServerRefreshStatus = new QLabel(tab);
        labelServerRefreshStatus->setObjectName(QString::fromUtf8("labelServerRefreshStatus"));

        verticalLayout_2->addWidget(labelServerRefreshStatus);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        buttonServerRefresh = new QPushButton(tab);
        buttonServerRefresh->setObjectName(QString::fromUtf8("buttonServerRefresh"));

        horizontalLayout_3->addWidget(buttonServerRefresh);

        buttonServerConnect = new QPushButton(tab);
        buttonServerConnect->setObjectName(QString::fromUtf8("buttonServerConnect"));

        horizontalLayout_3->addWidget(buttonServerConnect);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        verticalLayout_4 = new QVBoxLayout(tab_6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tableServers_2 = new QTableWidget(tab_6);
        if (tableServers_2->columnCount() < 4)
            tableServers_2->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        __qtablewidgetitem6->setFont(font);
        tableServers_2->setHorizontalHeaderItem(0, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableServers_2->setHorizontalHeaderItem(1, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableServers_2->setHorizontalHeaderItem(2, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableServers_2->setHorizontalHeaderItem(3, __qtablewidgetitem9);
        tableServers_2->setObjectName(QString::fromUtf8("tableServers_2"));
        tableServers_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers_2->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers_2->verticalHeader()->setVisible(false);

        verticalLayout_4->addWidget(tableServers_2);

        labelCloudRefreshStatus = new QLabel(tab_6);
        labelCloudRefreshStatus->setObjectName(QString::fromUtf8("labelCloudRefreshStatus"));

        verticalLayout_4->addWidget(labelCloudRefreshStatus);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        buttonCloudRefresh = new QPushButton(tab_6);
        buttonCloudRefresh->setObjectName(QString::fromUtf8("buttonCloudRefresh"));

        horizontalLayout->addWidget(buttonCloudRefresh);

        buttonCloudConnect = new QPushButton(tab_6);
        buttonCloudConnect->setObjectName(QString::fromUtf8("buttonCloudConnect"));

        horizontalLayout->addWidget(buttonCloudConnect);


        verticalLayout_4->addLayout(horizontalLayout);

        tabWidget->addTab(tab_6, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_5 = new QVBoxLayout(tab_2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tableServers_3 = new QTableWidget(tab_2);
        if (tableServers_3->columnCount() < 2)
            tableServers_3->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        __qtablewidgetitem10->setFont(font);
        tableServers_3->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableServers_3->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        tableServers_3->setObjectName(QString::fromUtf8("tableServers_3"));
        tableServers_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers_3->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers_3->verticalHeader()->setVisible(false);

        verticalLayout_5->addWidget(tableServers_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);

        buttonRecentConnect = new QPushButton(tab_2);
        buttonRecentConnect->setObjectName(QString::fromUtf8("buttonRecentConnect"));

        horizontalLayout_5->addWidget(buttonRecentConnect);


        verticalLayout_5->addLayout(horizontalLayout_5);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_6 = new QVBoxLayout(tab_3);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        tableServers_4 = new QTableWidget(tab_3);
        if (tableServers_4->columnCount() < 2)
            tableServers_4->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        __qtablewidgetitem12->setFont(font);
        tableServers_4->setHorizontalHeaderItem(0, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableServers_4->setHorizontalHeaderItem(1, __qtablewidgetitem13);
        tableServers_4->setObjectName(QString::fromUtf8("tableServers_4"));
        tableServers_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers_4->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers_4->verticalHeader()->setVisible(false);

        verticalLayout_6->addWidget(tableServers_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        buttonFavoritesAdd = new QPushButton(tab_3);
        buttonFavoritesAdd->setObjectName(QString::fromUtf8("buttonFavoritesAdd"));

        horizontalLayout_6->addWidget(buttonFavoritesAdd);

        buttonFavoritesRemove = new QPushButton(tab_3);
        buttonFavoritesRemove->setObjectName(QString::fromUtf8("buttonFavoritesRemove"));

        horizontalLayout_6->addWidget(buttonFavoritesRemove);

        buttonFavoritesConnect = new QPushButton(tab_3);
        buttonFavoritesConnect->setObjectName(QString::fromUtf8("buttonFavoritesConnect"));

        horizontalLayout_6->addWidget(buttonFavoritesConnect);


        verticalLayout_6->addLayout(horizontalLayout_6);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_7 = new QVBoxLayout(tab_4);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        tableServers_5 = new QTableWidget(tab_4);
        if (tableServers_5->columnCount() < 7)
            tableServers_5->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        __qtablewidgetitem14->setFont(font);
        tableServers_5->setHorizontalHeaderItem(0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(1, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(2, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(3, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(4, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(5, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableServers_5->setHorizontalHeaderItem(6, __qtablewidgetitem20);
        tableServers_5->setObjectName(QString::fromUtf8("tableServers_5"));
        tableServers_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers_5->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers_5->verticalHeader()->setVisible(false);

        verticalLayout_7->addWidget(tableServers_5);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_7);

        buttonGameConnect = new QPushButton(tab_4);
        buttonGameConnect->setObjectName(QString::fromUtf8("buttonGameConnect"));

        horizontalLayout_7->addWidget(buttonGameConnect);


        verticalLayout_7->addLayout(horizontalLayout_7);

        tabWidget->addTab(tab_4, QString());

        verticalLayout_3->addWidget(tabWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        labelUsername = new QLabel(centralwidget);
        labelUsername->setObjectName(QString::fromUtf8("labelUsername"));

        horizontalLayout_2->addWidget(labelUsername);

        lineEditUsername = new QLineEdit(centralwidget);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));

        horizontalLayout_2->addWidget(lineEditUsername);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        buttonConnectIP = new QPushButton(centralwidget);
        buttonConnectIP->setObjectName(QString::fromUtf8("buttonConnectIP"));

        horizontalLayout_2->addWidget(buttonConnectIP);

        buttonAbout = new QPushButton(centralwidget);
        buttonAbout->setObjectName(QString::fromUtf8("buttonAbout"));

        horizontalLayout_2->addWidget(buttonAbout);

        buttonCancel = new QPushButton(centralwidget);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_2->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_3->addLayout(verticalLayout);

        mainwindowClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(mainwindowClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1122, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        mainwindowClass->setMenuBar(menubar);
        statusbar = new QStatusBar(mainwindowClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mainwindowClass->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);
        menuEdit->addAction(actionSettings_2);

        retranslateUi(mainwindowClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(mainwindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainwindowClass)
    {
        mainwindowClass->setWindowTitle(QApplication::translate("mainwindowClass", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionSettings->setText(QApplication::translate("mainwindowClass", "Settings...", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("mainwindowClass", "About...", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("mainwindowClass", "Exit", 0, QApplication::UnicodeUTF8));
        actionSettings_2->setText(QApplication::translate("mainwindowClass", "Settings...", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableServers->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("mainwindowClass", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableServers->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("mainwindowClass", "Location", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableServers->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("mainwindowClass", "Ping", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableServers->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("mainwindowClass", "Version", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableServers->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("mainwindowClass", "Users", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tableServers->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("mainwindowClass", "Games", 0, QApplication::UnicodeUTF8));
        labelServerRefreshStatus->setText(QApplication::translate("mainwindowClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        buttonServerRefresh->setText(QApplication::translate("mainwindowClass", "Refresh", 0, QApplication::UnicodeUTF8));
        buttonServerConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("mainwindowClass", "Kaillera Servers", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableServers_2->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QApplication::translate("mainwindowClass", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableServers_2->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QApplication::translate("mainwindowClass", "Ping", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableServers_2->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QApplication::translate("mainwindowClass", "Users", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableServers_2->horizontalHeaderItem(3);
        ___qtablewidgetitem9->setText(QApplication::translate("mainwindowClass", "Games", 0, QApplication::UnicodeUTF8));
        labelCloudRefreshStatus->setText(QApplication::translate("mainwindowClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        buttonCloudRefresh->setText(QApplication::translate("mainwindowClass", "Refresh", 0, QApplication::UnicodeUTF8));
        buttonCloudConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("mainwindowClass", "NetEmu clouds", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tableServers_3->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QApplication::translate("mainwindowClass", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tableServers_3->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QApplication::translate("mainwindowClass", "Type", 0, QApplication::UnicodeUTF8));
        buttonRecentConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("mainwindowClass", "Recent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tableServers_4->horizontalHeaderItem(0);
        ___qtablewidgetitem12->setText(QApplication::translate("mainwindowClass", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tableServers_4->horizontalHeaderItem(1);
        ___qtablewidgetitem13->setText(QApplication::translate("mainwindowClass", "Type", 0, QApplication::UnicodeUTF8));
        buttonFavoritesAdd->setText(QApplication::translate("mainwindowClass", "Add...", 0, QApplication::UnicodeUTF8));
        buttonFavoritesRemove->setText(QApplication::translate("mainwindowClass", "Remove", 0, QApplication::UnicodeUTF8));
        buttonFavoritesConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("mainwindowClass", "Favorites", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tableServers_5->horizontalHeaderItem(0);
        ___qtablewidgetitem14->setText(QApplication::translate("mainwindowClass", "Game name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tableServers_5->horizontalHeaderItem(1);
        ___qtablewidgetitem15->setText(QApplication::translate("mainwindowClass", "Emulator", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tableServers_5->horizontalHeaderItem(2);
        ___qtablewidgetitem16->setText(QApplication::translate("mainwindowClass", "Username", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tableServers_5->horizontalHeaderItem(3);
        ___qtablewidgetitem17->setText(QApplication::translate("mainwindowClass", "Server name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tableServers_5->horizontalHeaderItem(4);
        ___qtablewidgetitem18->setText(QApplication::translate("mainwindowClass", "Location", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem19 = tableServers_5->horizontalHeaderItem(5);
        ___qtablewidgetitem19->setText(QApplication::translate("mainwindowClass", "# Players", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem20 = tableServers_5->horizontalHeaderItem(6);
        ___qtablewidgetitem20->setText(QApplication::translate("mainwindowClass", "Ping", 0, QApplication::UnicodeUTF8));
        buttonGameConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("mainwindowClass", "Waiting games", 0, QApplication::UnicodeUTF8));
        labelUsername->setText(QApplication::translate("mainwindowClass", "Username:", 0, QApplication::UnicodeUTF8));
        buttonConnectIP->setText(QApplication::translate("mainwindowClass", "Connect to IP...", 0, QApplication::UnicodeUTF8));
        buttonAbout->setText(QApplication::translate("mainwindowClass", "About", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("mainwindowClass", "Cancel", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("mainwindowClass", "File", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("mainwindowClass", "Help", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("mainwindowClass", "Edit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainwindowClass: public Ui_mainwindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
