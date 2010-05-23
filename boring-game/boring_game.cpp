#include "boring_game.h"
#include "netemu_p2p.h"

boring_game::boring_game(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.playButton, SIGNAL(clicked()), this, SLOT(hostGame()));
	connect(ui.joinButton, SIGNAL(clicked()), this, SLOT(joinGame()));
}

void boring_game::hostGame()
{
	QByteArray username;
	int port;
	struct p2p_game *game;
	username = ui.lineEdit->text().toLatin1();
	port = ui.lineEdit_2->text().toInt(NULL, 10);
	p2p = netemu_p2p_new(username.data(), "boring_game", 1);
	netemu_p2p_host(p2p, netemu_inet_addr("127.0.0.1"), netemu_htons(port) , "myCloud");
	netemu_p2p_create_game(p2p, "thegame", 1, 2, &game);
}

void boring_game::joinGame()
{
	QByteArray username;
	struct p2p_game *game;
	struct p2p_user **users;
	int joinPort, hostPort, size;
	username = ui.lineEdit->text().toLatin1();
	joinPort = ui.joinConnectPort->text().toInt(NULL,10);
	hostPort = ui.joinHostPort->text().toInt(NULL,10);
	p2p = netemu_p2p_new(username.data(), "boring_game", 1);
	netemu_p2p_connect(p2p, netemu_inet_addr("127.0.0.1"), netemu_htons(hostPort), netemu_inet_addr("127.0.0.1"), netemu_htons(joinPort));
	users = netemu_p2p_get_user_list(p2p, &size);
	for(int i = 0; i < size; i++) {
		ui.playerList->addItem(new QListWidgetItem(users[i]->name));
	}
}



boring_game::~boring_game()
{

}
