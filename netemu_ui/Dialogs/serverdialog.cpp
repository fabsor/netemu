#include "serverdialog.h"

ServerDialog::ServerDialog(QWidget *parent, netemu_info *serverInfo)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	this->serverInfo = serverInfo;
	this->InitializeUserList();
}

void ServerDialog::InitializeUserList()
{
	ui.tableUsers->setRowCount(this->serverInfo->user_count);
	for(int i = 0; i < this->serverInfo->user_count; i++)
	{
		ui.tableUsers->setItem(i, 0,
				new QTableWidgetItem(this->serverInfo->user[i].username, 0));
		ui.tableUsers->setItem(i, 1,
				new QTableWidgetItem(QString::number(this->serverInfo->user[i].ping, 10), 0));
		switch(this->serverInfo->user[i].status)
		{
		case 1:
			ui.tableUsers->setItem(i, 2,
					new QTableWidgetItem("Idle", 0));
			break;
		case 2:
			ui.tableUsers->setItem(i, 2,
					new QTableWidgetItem("Playing", 0));
			break;
		}
	}
}

ServerDialog::~ServerDialog()
{

}
