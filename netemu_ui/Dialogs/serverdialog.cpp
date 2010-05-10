#include "serverdialog.h"
#include "netemu_kaillera.h"

void ChatCallBackWrapper(char *user, char *message, void *user_data);
void UserJoinCallBackWrapper(char *user, NETEMU_DWORD ping, char connection, void *user_data);
void UserLeaveCallBackWrapper(NETEMU_WORD id, char *user, char *exit_message, void *user_data);
void GameCreatedCallBackWrapper(struct game *createdGame, void *user_data);

ServerDialog::ServerDialog(QWidget *parent, netemu_info *serverInfo)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	this->serverInfo = serverInfo;
	this->InitializeUserList();
	this->CreateActions();
	netemu_register_chat_callback(this->serverInfo, ChatCallBackWrapper, this);
	netemu_register_user_join_callback(this->serverInfo, UserJoinCallBackWrapper, this);
	netemu_register_user_leave_callback(this->serverInfo, UserLeaveCallBackWrapper, this);
	netemu_register_game_created_callback(this->serverInfo, GameCreatedCallBackWrapper, this);
}

void UserJoinCallBackWrapper(char *user, NETEMU_DWORD ping, char connection, void *user_data)
{
	ServerDialog *dialog = (ServerDialog*)user_data;
	dialog->OnUserListChanged();
}

void UserLeaveCallBackWrapper(NETEMU_WORD id, char *user, char *exit_message, void *user_data)
{
	ServerDialog *dialog = (ServerDialog*)user_data;
	dialog->OnUserListChanged();
}

void GameCreatedCallBackWrapper(struct game *createdGame, void *user_data)
{
	ServerDialog *dialog = (ServerDialog*)user_data;
	dialog->OnGameListChanged();
}

void ServerDialog::OnGameListChanged()
{
	struct game **games;
	int gameCount;

	games = netemu_kaillera_get_game_list(this->serverInfo, &gameCount);
	ui.tableGames->setRowCount(gameCount);
	ui.tableGames->clearContents();

	for(int i = 0; i < gameCount; i++)
	{
		ui.tableGames->setItem(i, 0,
				new QTableWidgetItem(games[i]->name, 0));
		ui.tableGames->setItem(i, 0,
				new QTableWidgetItem(games[i]->app_name, 0));
		ui.tableGames->setItem(i, 0,
				new QTableWidgetItem(games[i]->creator->username, 0));
		ui.tableGames->setItem(i, 0,
						new QTableWidgetItem(QString::number(games[i]->status, 10), 0));
		ui.tableGames->setItem(i, 0,
				new QTableWidgetItem(games[i]->users_count, 0));
	}
}


void ServerDialog::OnUserListChanged()
{
	struct user **users;
	int userCount;
	int i;

	users = netemu_kaillera_get_user_list(this->serverInfo, &userCount);
	ui.groupPartyLine->setTitle(QString("Partyline (") + QString::number(userCount, 10) + QString(" users)"));
	ui.tableUsers->setRowCount(userCount);
	ui.tableUsers->clearContents();
	for(i = 0; i < userCount; i++)
	{
		ui.tableUsers->setItem(i, 0,
				new QTableWidgetItem(users[i]->username, 0));
		ui.tableUsers->setItem(i, 1,
				new QTableWidgetItem(QString::number(users[i]->ping, 10), 0));
		switch(users[i]->status)
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
	/* Removed the following block of code because, while it is the best way to do it, we are getting duplicate instructions from the server
	 * and thus will be adding duplicates in the list. I have chosen to just use netemus internal user list instead, as it filters out duplicates. */
	/*int rowCount = ui.tableUsers->rowCount();
	ui.tableUsers->setRowCount(rowCount + 1);
	ui.tableUsers->setItem(rowCount, 0,
			new QTableWidgetItem(user, 0));
	ui.tableUsers->setItem(rowCount, 1,
			new QTableWidgetItem(QString::number(ping, 10), 0));*/
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
	ui.plainTextChat->ensureCursorVisible();
	/* moveToThread kanske?*/
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
