/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Apr 29 10:07:45 2010
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
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainwindowClass
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tableServers;
    QLabel *labelRefreshStatus;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *buttonRefresh;
    QPushButton *buttonConnect;
    QWidget *tab_6;
    QTableWidget *tableServers_2;
    QWidget *tab_2;
    QPushButton *pushButton;
    QWidget *tab_3;
    QWidget *tab_4;
    QWidget *tab_5;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonConnectIP;
    QPushButton *buttonAbout;
    QPushButton *buttonCancel;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *mainwindowClass)
    {
        if (mainwindowClass->objectName().isEmpty())
            mainwindowClass->setObjectName(QString::fromUtf8("mainwindowClass"));
        mainwindowClass->resize(1008, 622);
        centralwidget = new QWidget(mainwindowClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 4, 1007, 530));
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

        labelRefreshStatus = new QLabel(tab);
        labelRefreshStatus->setObjectName(QString::fromUtf8("labelRefreshStatus"));

        verticalLayout_2->addWidget(labelRefreshStatus);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        buttonRefresh = new QPushButton(tab);
        buttonRefresh->setObjectName(QString::fromUtf8("buttonRefresh"));

        horizontalLayout_3->addWidget(buttonRefresh);

        buttonConnect = new QPushButton(tab);
        buttonConnect->setObjectName(QString::fromUtf8("buttonConnect"));

        horizontalLayout_3->addWidget(buttonConnect);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
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
        tableServers_2->setGeometry(QRect(20, 10, 941, 391));
        tableServers_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers_2->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers_2->verticalHeader()->setVisible(false);
        tabWidget->addTab(tab_6, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        pushButton = new QPushButton(tab_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(130, 160, 93, 27));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        tabWidget->addTab(tab_5, QString());
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 540, 1007, 66));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(verticalLayoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonConnectIP = new QPushButton(verticalLayoutWidget);
        buttonConnectIP->setObjectName(QString::fromUtf8("buttonConnectIP"));

        horizontalLayout_2->addWidget(buttonConnectIP);

        buttonAbout = new QPushButton(verticalLayoutWidget);
        buttonAbout->setObjectName(QString::fromUtf8("buttonAbout"));

        horizontalLayout_2->addWidget(buttonAbout);

        buttonCancel = new QPushButton(verticalLayoutWidget);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_2->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_2);

        mainwindowClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(mainwindowClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1008, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        mainwindowClass->setMenuBar(menubar);
        statusbar = new QStatusBar(mainwindowClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        mainwindowClass->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());

        retranslateUi(mainwindowClass);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(mainwindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainwindowClass)
    {
        mainwindowClass->setWindowTitle(QApplication::translate("mainwindowClass", "MainWindow", 0, QApplication::UnicodeUTF8));
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
        labelRefreshStatus->setText(QApplication::translate("mainwindowClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        buttonRefresh->setText(QApplication::translate("mainwindowClass", "Refresh", 0, QApplication::UnicodeUTF8));
        buttonConnect->setText(QApplication::translate("mainwindowClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("mainwindowClass", "Kaillera Servers", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tableServers_2->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QApplication::translate("mainwindowClass", "Name", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tableServers_2->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QApplication::translate("mainwindowClass", "Ping", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tableServers_2->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QApplication::translate("mainwindowClass", "Users", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tableServers_2->horizontalHeaderItem(3);
        ___qtablewidgetitem9->setText(QApplication::translate("mainwindowClass", "Games", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("mainwindowClass", "NetEmu clouds", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("mainwindowClass", "PushButton", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("mainwindowClass", "Recent", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("mainwindowClass", "Favorites", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("mainwindowClass", "Waiting games", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("mainwindowClass", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("mainwindowClass", "Username: ", 0, QApplication::UnicodeUTF8));
        buttonConnectIP->setText(QApplication::translate("mainwindowClass", "Connect to IP...", 0, QApplication::UnicodeUTF8));
        buttonAbout->setText(QApplication::translate("mainwindowClass", "About", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("mainwindowClass", "Cancel", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("mainwindowClass", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainwindowClass: public Ui_mainwindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
