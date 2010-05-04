#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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


private:
	void createActions();
    Ui::mainwindowClass ui;
    QSettings *settings;
};

#endif // MAINWINDOW_H
