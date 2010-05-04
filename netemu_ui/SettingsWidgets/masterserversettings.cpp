#include "masterserversettings.h"

MasterServerSettings::MasterServerSettings(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	this->createActions();
	this->LoadPreferences();
}

MasterServerSettings::~MasterServerSettings()
{

}

void MasterServerSettings::createActions()
{
	connect(ui.textMasterAddress, SIGNAL(editingFinished()), this, SLOT(OnMasterServerAddressChanged()));
}
//
void MasterServerSettings::OnMasterServerAddressChanged()
{
	this->masterServerAddressChanged = true;
}
//
void MasterServerSettings::LoadPreferences()
{
	QSettings *settings = new QSettings;
	ui.textMasterAddress->setText(settings->value("KailleraMasterServer/Address", "www.kaillera.com").toString());

	delete settings;
}

void MasterServerSettings::ApplyChanges(QSettings *settings)
{
	if(this->masterServerAddressChanged)
	{
		settings->setValue("KailleraMasterServer/Address", ui.textMasterAddress->text());
	}
}
