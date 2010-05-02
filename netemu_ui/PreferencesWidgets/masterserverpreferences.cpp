#include "masterserverpreferences.h"

MasterServerPreferences::MasterServerPreferences(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	this->createActions();
	this->LoadPreferences();
}

MasterServerPreferences::~MasterServerPreferences()
{

}

void MasterServerPreferences::createActions()
{
	connect(ui.textMasterAddress, SIGNAL(editingFinished()), this, SLOT(OnMasterServerAddressChanged()));
}

void MasterServerPreferences::OnMasterServerAddressChanged()
{
	this->masterServerAddressChanged = true;
}

void MasterServerPreferences::LoadPreferences()
{
	QSettings *settings = new QSettings;
	ui.textMasterAddress->setText(settings->value("KailleraMasterServer/Address", "www.kaillera.com").toString());

	delete settings;
}

void MasterServerPreferences::ApplyChanges(QSettings *settings)
{
	if(this->masterServerAddressChanged)
	{
		settings->setValue("KailleraMasterServer/Address", ui.textMasterAddress->text());
	}
}
