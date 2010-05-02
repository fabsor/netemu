#ifndef MASTERSERVERPREFERENCES_H
#define MASTERSERVERPREFERENCES_H

#include <QtGui/QWidget>
#include "ui_masterserverpreferences.h"
#include <QSettings>

class MasterServerPreferences : public QWidget
{
    Q_OBJECT

public:
    MasterServerPreferences(QWidget *parent = 0);
    ~MasterServerPreferences();
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
