/********************************************************************************
** Form generated from reading UI file 'netemu_ui.ui'
**
** Created: Wed Apr 28 22:12:30 2010
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
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_netemu_uiClass
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tableServers;
    QLabel *labelRefreshStatus;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *buttonRefresh;
    QPushButton *buttonConnect;
    QWidget *tab_2;
    QWidget *tab_3;
    QWidget *tab_4;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonConnectIP;
    QPushButton *buttonAbout;
    QPushButton *buttonCancel;

    void setupUi(QWidget *netemu_uiClass)
    {
        if (netemu_uiClass->objectName().isEmpty())
            netemu_uiClass->setObjectName(QString::fromUtf8("netemu_uiClass"));
        netemu_uiClass->resize(1025, 620);
        gridLayout = new QGridLayout(netemu_uiClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(netemu_uiClass);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        tableServers = new QTableWidget(tab);
        if (tableServers->columnCount() < 5)
            tableServers->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableServers->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tableServers->setObjectName(QString::fromUtf8("tableServers"));
        tableServers->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableServers->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableServers->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(tableServers);

        labelRefreshStatus = new QLabel(tab);
        labelRefreshStatus->setObjectName(QString::fromUtf8("labelRefreshStatus"));

        verticalLayout_2->addWidget(labelRefreshStatus);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        buttonRefresh = new QPushButton(tab);
        buttonRefresh->setObjectName(QString::fromUtf8("buttonRefresh"));

        horizontalLayout_3->addWidget(buttonRefresh);

        buttonConnect = new QPushButton(tab);
        buttonConnect->setObjectName(QString::fromUtf8("buttonConnect"));

        horizontalLayout_3->addWidget(buttonConnect);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
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

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(netemu_uiClass);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(netemu_uiClass);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        buttonConnectIP = new QPushButton(netemu_uiClass);
        buttonConnectIP->setObjectName(QString::fromUtf8("buttonConnectIP"));

        horizontalLayout_2->addWidget(buttonConnectIP);

        buttonAbout = new QPushButton(netemu_uiClass);
        buttonAbout->setObjectName(QString::fromUtf8("buttonAbout"));

        horizontalLayout_2->addWidget(buttonAbout);

        buttonCancel = new QPushButton(netemu_uiClass);
        buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

        horizontalLayout_2->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout, 1, 0, 1, 1);


        retranslateUi(netemu_uiClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(netemu_uiClass);
    } // setupUi

    void retranslateUi(QWidget *netemu_uiClass)
    {
        netemu_uiClass->setWindowTitle(QApplication::translate("netemu_uiClass", "netemu_ui", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableServers->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("netemu_uiClass", "Servers", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableServers->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("netemu_uiClass", "Recent", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableServers->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("netemu_uiClass", "Favorites", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableServers->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("netemu_uiClass", "Waiting games", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tableServers->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("netemu_uiClass", "Options", 0, QApplication::UnicodeUTF8));
        labelRefreshStatus->setText(QApplication::translate("netemu_uiClass", "TextLabel", 0, QApplication::UnicodeUTF8));
        buttonRefresh->setText(QApplication::translate("netemu_uiClass", "Refresh", 0, QApplication::UnicodeUTF8));
        buttonConnect->setText(QApplication::translate("netemu_uiClass", "Connect", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("netemu_uiClass", "Servers", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("netemu_uiClass", "Recent", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("netemu_uiClass", "Favorites", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("netemu_uiClass", "Waiting games", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("netemu_uiClass", "Options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("netemu_uiClass", "Username: ", 0, QApplication::UnicodeUTF8));
        buttonConnectIP->setText(QApplication::translate("netemu_uiClass", "Connect to IP...", 0, QApplication::UnicodeUTF8));
        buttonAbout->setText(QApplication::translate("netemu_uiClass", "About", 0, QApplication::UnicodeUTF8));
        buttonCancel->setText(QApplication::translate("netemu_uiClass", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class netemu_uiClass: public Ui_netemu_uiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NETEMU_UI_H
