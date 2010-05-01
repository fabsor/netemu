#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSettings>
#include "ui_mainwindow.h"

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = 0);
    ~mainwindow();
	
private slots:
	void refreshServerList();
	void showPreferences();
	void tableServersItemChanged();
	void tableCloudsItemChanged();
	void tableRecentItemChanged();
	void tableFavoritesItemChanged();


private:
	void createActions();
    Ui::mainwindowClass ui;
    QSettings *settings;
};

#endif // MAINWINDOW_H
