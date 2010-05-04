#include "mainwindow.h"
#include "Dialogs/settingsdialog.h"
//#include "Dialogs/connectdialog.h"
#include "netemu_kaillera.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	this->settings = new QSettings;
	this->setWindowTitle("NetEmu");
	netemu_init_network();
	createActions();
}


void MainWindow::refreshServerList()
{
	//struct server mo;
	//struct server **servers;
	//struct existing_game **games;
	//int server_count;
	//int game_count;
	//char* address, *portchars;
	//int port;
	//int semicolon_index;
	//kaillera_communication_get_server_list(this->settings->value("KailleraMasterServer/address", "www.kaillera.com").toString().toLatin1().constData(), &servers, &server_count, &games, &game_count);
	//
	//ui.tableServers->clearContents();
	//ui.tableServers->setRowCount(server_count);
	//for(int i = 0; i < server_count; i++)
	//{
	//	ui.tableServers->setItem(i, 0,
	//			new QTableWidgetItem(servers[i]->name, 0));
	//	ui.tableServers->setItem(i, 1,
	//			new QTableWidgetItem(servers[i]->location, 0));
	//	ui.tableServers->setItem(i, 3,
	//			new QTableWidgetItem(servers[i]->version, 0));
	//	ui.tableServers->setItem(i, 4,
	//			new QTableWidgetItem(servers[i]->players, 0));
	//	ui.tableServers->setItem(i, 5,
	//			new QTableWidgetItem(QString::number(servers[i]->games, 10), 0));
	//	ui.tableServers->setItem(i, 6,
	//			new QTableWidgetItem(servers[i]->address, 0));
	//	//struct netemu_sockaddr_in addr;
	//	//portchars = strstr(servers[i]->address, ":");
	//	//portchars = strpbrk(servers[i]->address, ":");
	//	
	//	//portchars = servers[i]->address + strlen(address);
	//	//netemu_sender_udp *sender = netemu_sender_udp_new
	//	//netemu_communication_ping_server(servers[i], NULL);
	//}
}

void MainWindow::OnButtonServerConnectClicked()
{
	//QList<QTableWidgetItem*> row = ui.tableServers->selectedItems();

	//QString serverName = row.value(5)->text();
	//ConnectDialog dialog(this, row.value(0)->text(), row.value(5)->text(), KailleraServer);

	//dialog.exec();
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

	// Menu items.
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionSettings_1, SIGNAL(triggered()), this, SLOT(showPreferences()));
}

MainWindow::~MainWindow()
{
	delete this->settings;
}
