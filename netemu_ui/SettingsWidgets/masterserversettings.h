#ifndef MASTERSERVERPREFERENCES_H
#define MASTERSERVERPREFERENCES_H

#include <QtGui/QWidget>
#include "ui_masterserversettings.h"
#include <QSettings>

class MasterServerSettings : public QWidget
{
    Q_OBJECT

public:
    MasterServerSettings(QWidget *parent = 0);
    ~MasterServerSettings();
    void ApplyChanges(QSettings *settings);
private slots:
	void OnMasterServerAddressChanged();

private:
    Ui::MasterServerPreferencesClass ui;
    bool masterServerAddressChanged;
    void LoadPreferences();
    void createActions();
};

#endif // MASTERSERVERPREFERENCES_H
