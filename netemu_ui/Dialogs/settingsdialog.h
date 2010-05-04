#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtGui/QDialog>
#include "ui_settingsdialog.h"
#include <QSettings>
#include "SettingsWidgets/masterserversettings.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0, QSettings *settings = NULL);
    ~SettingsDialog();
//
//private slots:
//	void changePreferencePage(QListWidgetItem *current, QListWidgetItem *previous);
	void OnButtonOkClicked();

private:
    Ui::preferencesdialogClass ui;
    MasterServerSettings masterServerPreferences;
    QSettings *settings;
    void setupPreferencesWidgets();
    void createActions();
};

#endif // PREFERENCESDIALOG_H
