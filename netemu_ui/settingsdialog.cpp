#include "settingsdialog.h"
#include "mainwindow.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("NetEmu Settings");
	this->setupPreferencesWidgets();
	this->createActions();
}

void SettingsDialog::createActions()
{
	connect(ui.listSettings, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(changePreferencePage(QListWidgetItem*,QListWidgetItem*)));
	connect(ui.buttonOK, SIGNAL(clicked()), this, SLOT(OnButtonOkClicked()));
}

void SettingsDialog::OnButtonOkClicked()
{
	QSettings *settings = new QSettings;
	this->masterServerPreferences.ApplyChanges(settings);
	delete settings;
}

void SettingsDialog::setupPreferencesWidgets()
{
	ui.stackedWidget->addWidget(&this->masterServerPreferences);
	qDebug("Setup preferences");
}

void SettingsDialog::changePreferencePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if(!current)
		current = previous;

	ui.stackedWidget->setCurrentIndex(ui.listSettings->row(current));
	qDebug("Changed!");
}

SettingsDialog::~SettingsDialog()
{

}
