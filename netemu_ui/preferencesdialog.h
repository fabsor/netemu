#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtGui/QDialog>
#include "ui_preferencesdialog.h"
#include "PreferencesWidgets/masterserverpreferences.h"

class preferencesdialog : public QDialog
{
    Q_OBJECT

public:
    preferencesdialog(QWidget *parent = 0);
    ~preferencesdialog();

private slots:
	void changePreferencePage(QListWidgetItem *current, QListWidgetItem *previous);
	void OnButtonOkClicked();

private:
    Ui::preferencesdialogClass ui;
    MasterServerPreferences masterServerPreferences;
    void setupPreferencesWidgets();
    void createActions();
};

#endif // PREFERENCESDIALOG_H
