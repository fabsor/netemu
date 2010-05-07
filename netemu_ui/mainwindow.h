#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Dialogs/connectdialog.h"
#include <QtGui/QMainWindow>
#include <QSettings>
#include "ui_mainwindow.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
	
private slots:
	void refreshServerList();
	void showPreferences();
	void tableServersItemChanged();
	void tableCloudsItemChanged();
	void tableRecentItemChanged();
	void tableFavoritesItemChanged();
	void OnButtonServerConnectClicked();
	void OnButtonConnectServerIPClicked();


private:
	void createActions();
	void ShowConnectDialog(QString name, QString address, HostType type);
    Ui::mainwindowClass ui;
    QSettings *settings;
};

#endif // MAINWINDOW_H
