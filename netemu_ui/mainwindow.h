#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = 0);
    ~mainwindow();
	
private slots:
	void refreshServerList();


private:
	void createActions();
    Ui::mainwindowClass ui;
};

#endif // MAINWINDOW_H
