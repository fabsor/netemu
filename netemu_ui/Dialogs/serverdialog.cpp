#include "serverdialog.h"
#include "netemu_kaillera.h"

void ChatCallBackWrapper(char *user, char *message, void *user_data);
void UserJoinCallBackWrapper(char *user, NETEMU_DWORD ping, char connection, void *user_data);

ServerDialog::ServerDialog(QWidget *parent, netemu_info *serverInfo)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	this->serverInfo = serverInfo;
	this->InitializeUserList();
	this->CreateActions();
	netemu_register_chat_callback(this->serverInfo, ChatCallBackWrapper, this);
	netemu_register_user_join_callback(this->serverInfo, UserJoinCallBackWrapper, this);
}

void UserJoinCallBackWrapper(char *user, NETEMU_DWORD ping, char connection, void *user_data)
{
	ServerDialog *dialog = (ServerDialog*)user_data;
	dialog->UserJoinCallBack(user, ping, connection);
}

void ServerDialog::UserJoinCallBack(char *user, NETEMU_DWORD ping, char connection)
{
	int rowCount = ui.tableUsers->rowCount();
	ui.tableUsers->setRowCount(rowCount + 1);
	ui.tableUsers->setItem(rowCount, 0,
			new QTableWidgetItem(user, 0));
	ui.tableUsers->setItem(rowCount, 1,
			new QTableWidgetItem(QString::number(ping, 10), 0));
}

void ChatCallBackWrapper(char *user, char *message, void *user_data)
{
	ServerDialog *dialog = (ServerDialog*)user_data;
	dialog->ChatCallBack(user, message);
}

void ServerDialog::ChatCallBack(char *user, char *message)
{
	QString userString(user);
	ui.plainTextChat->appendPlainText(QString(user) + QString(": ") + QString(message));
	ui.plainTextChat->update();
}

void ServerDialog::CreateActions()
{
	connect(ui.buttonSendChat, SIGNAL(clicked()), this, SLOT(OnButtonSendChatClick()));
}

void ServerDialog::OnButtonSendChatClick()
{
	QString message = ui.textSendChat->text();
	QByteArray byteArray;
	if(message != "")
	{
		byteArray = message.toLatin1();
		netemu_send_chat_message(this->serverInfo, byteArray.data());
	}
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
