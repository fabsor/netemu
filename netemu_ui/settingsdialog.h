#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtGui/QDialog>
#include "ui_preferencesdialog.h"
#include "SettingsWidgets/masterserversettings.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
	void changePreferencePage(QListWidgetItem *current, QListWidgetItem *previous);
	void OnButtonOkClicked();

private:
    Ui::preferencesdialogClass ui;
    MasterServerSettings masterServerPreferences;
    void setupPreferencesWidgets();
    void createActions();
};

#endif // PREFERENCESDIALOG_H
