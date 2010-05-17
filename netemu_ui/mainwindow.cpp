#include "mainwindow.h"
#include "Dialogs/settingsdialog.h"
#include "Dialogs/inputdialog.h"
#include "netemu_kaillera.h"
#include "netlib_error.h"
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	this->settings = new QSettings;
	ui.setupUi(this);
	ui.labelServerRefreshStatus->setText("");
	ui.textUsername->setText(this->settings->value("Username", "").toString());
	this->setWindowTitle("NetEmu");
	netemu_init_network();
	createActions();
	this->serverDialog = NULL;
}

void MainWindow::refreshServerList()
{
	QString masterServer;
	struct server **servers;
	struct existing_game **games;
	int server_count;
	int game_count;
	int return_value;

	masterServer = this->settings->value("KailleraMasterServer/address", "www.kaillera.com").toString();

	return_value = kaillera_communication_get_server_list(masterServer.toLatin1().constData(), &servers, &server_count, &games, &game_count);
	if(return_value != 0)
	{
		//ui.labelServerRefreshStatus->setText("Could not retrieve master server list. Error: " + QString::number(netlib_get_last_error(), 10));
		ui.labelServerRefreshStatus->setText("Could not retrieve master server list. Error: " + QString(netlib_get_last_error_string()));

		return;
	}
	else
	{

		ui.labelServerRefreshStatus->setText("Retrieved " + QString::number(server_count, 10) + " servers from the master server @ " + masterServer);
	}
	ui.tableServers->clearContents();
	ui.tableServers->setRowCount(server_count);
	for(int i = 0; i < server_count; i++)
	{
		ui.tableServers->setItem(i, 0,
				new QTableWidgetItem(servers[i]->name, 0));
		ui.tableServers->setItem(i, 1,
				new QTableWidgetItem(servers[i]->location, 0));
		ui.tableServers->setItem(i, 3,
				new QTableWidgetItem(servers[i]->version, 0));
		ui.tableServers->setItem(i, 4,
				new QTableWidgetItem(servers[i]->players, 0));
		ui.tableServers->setItem(i, 5,
				new QTableWidgetItem(QString::number(servers[i]->games, 10), 0));
		ui.tableServers->setItem(i, 6,
				new QTableWidgetItem(servers[i]->address, 0));
		//struct netemu_sockaddr_in addr;
		//portchars = strstr(servers[i]->address, ":");
		//portchars = strpbrk(servers[i]->address, ":");

		//portchars = servers[i]->address + strlen(address);
		//netemu_sender_udp *sender = netemu_sender_udp_new
		//netemu_communication_ping_server(servers[i], NULL);
	}
}

void MainWindow::OnButtonServerConnectClicked()
{
	QString username = ui.textUsername->text().trimmed();
	if(username == "")
	{
		QMessageBox messageBox;
		messageBox.setText("You must enter a username!");
		messageBox.setWindowTitle("NetEmu");
		messageBox.exec();
	}
	else
	{
		QList<QTableWidgetItem*> row = ui.tableServers->selectedItems();

		QString serverName = row.value(5)->text();
		this->ShowConnectDialog(row.value(0)->text(), row.value(5)->text(), KailleraServer);
	}

}

void MainWindow::ShowConnectDialog(QString name, QString address, HostType type)
{
	ConnectDialog dialog(this, name, address, type, ui.textUsername->text(), 1);
	if(dialog.exec() == QDialog::Accepted)
	{
		if(type == KailleraServer)
		{
			this->serverDialog = new ServerDialog(this, dialog.connectionInfo);
			this->serverDialog->show();
		}

	}
}

void MainWindow::OnButtonConnectServerIPClicked()
{
	QString username = ui.textUsername->text().trimmed();
	if(username == "")
	{
		QMessageBox messageBox;
		messageBox.setText("You must enter a username!");
		messageBox.setWindowTitle("NetEmu");
		messageBox.exec();
	}
	else
	{
		InputDialog dialog(this, "NetEmu - Enter IP", "Example: 92.105.23.43:27888");
		if(dialog.exec() == QDialog::Accepted)
		{
			this->ShowConnectDialog(QString::null, dialog.GetText(), KailleraServer);
		}
	}
}

void MainWindow::showPreferences()
{
	SettingsDialog dialog(this, this->settings);
	dialog.exec();
}

void MainWindow::tableServersItemChanged()
{
	ui.buttonServerConnect->setEnabled(ui.tableServers->selectedItems().count() > 0);
}

void MainWindow::tableCloudsItemChanged()
{
	ui.buttonCloudConnect->setEnabled(ui.tableClouds->selectedItems().count() > 0);
}

void MainWindow::tableRecentItemChanged()
{
	ui.buttonRecentConnect->setEnabled(ui.tableRecent->selectedItems().count() > 0);
}

void MainWindow::tableFavoritesItemChanged()
{
	bool itemsSelected = ui.tableFavorites->selectedItems().count() > 0;
	ui.buttonFavoritesConnect->setEnabled(itemsSelected);
	ui.buttonFavoritesRemove->setEnabled(itemsSelected);
}

void MainWindow::createActions()
{
	connect(ui.buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.buttonServerRefresh, SIGNAL(clicked()), this, SLOT(refreshServerList()));
	connect(ui.tableServers, SIGNAL(itemSelectionChanged()), this, SLOT(tableServersItemChanged()));
	connect(ui.buttonServerConnect, SIGNAL(clicked()), this, SLOT(OnButtonServerConnectClicked()));
	connect(ui.buttonConnectServerIP, SIGNAL(clicked()), this, SLOT(OnButtonConnectServerIPClicked()));
	// Menu items.
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionSettings_1, SIGNAL(triggered()), this, SLOT(showPreferences()));
}

MainWindow::~MainWindow()
{
	this->settings->setValue("Username", ui.textUsername->text());
	delete this->settings;
}
