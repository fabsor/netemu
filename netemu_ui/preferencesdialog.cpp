#include "preferencesdialog.h"
#include "mainwindow.h"

preferencesdialog::preferencesdialog(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("NetEmu Settings");
	this->setupPreferencesWidgets();
	this->createActions();
}

void preferencesdialog::createActions()
{
	connect(ui.listSettings, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(changePreferencePage(QListWidgetItem*,QListWidgetItem*)));
	connect(ui.buttonOK, SIGNAL(clicked()), this, SLOT(OnButtonOkClicked()));
}

void preferencesdialog::OnButtonOkClicked()
{
	QSettings *settings = new QSettings;
	this->masterServerPreferences.ApplyChanges(settings);
	delete settings;
}

void preferencesdialog::setupPreferencesWidgets()
{
	ui.stackedWidget->addWidget(&this->masterServerPreferences);
	qDebug("Setup preferences");
}

void preferencesdialog::changePreferencePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if(!current)
		current = previous;

	ui.stackedWidget->setCurrentIndex(ui.listSettings->row(current));
	qDebug("Changed!");
}

preferencesdialog::~preferencesdialog()
{

}
