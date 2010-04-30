/*
 * main.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAPALLOC
#include <stdlib.h>

//#include <crtdbg.h>
#include <stdio.h>
#include "netemu_p2p.h"
#include "netemu_kaillera.h"

#define EMUNAME		"testemu"
#define PLAYERNAME	"foobar"


#define BIND_ADDR	netemu_inet_addr("127.0.0.1")

#define ADDR	netemu_inet_addr("127.0.0.1")
#define PORT	netemu_htons(27888)
#define VALUE	"0x0x0xff"

#define CLOUD_NAME	"the_cloud"


char* games[] = {"Foo", "Bar"};
struct netemu_info *info;
#define NO_GAMES	2

void menu(struct netemu_info* connection);
void create_game(struct netemu_info *connection);
void show_game_list(struct netemu_info *connection);
void join_game(struct netemu_info* connection);
void start_game(struct netemu_info *connection);

void send_play_values(struct netemu_info *connection);
void show_user_list(struct netemu_info* connection);
void player_ready(struct netemu_info *connection);
void receive_values(struct buffered_play_values *result);
void login_success(int status, struct netemu_info* connection);
void connect_async(struct netemu_sockaddr_in addr);
void server_connect(struct netemu_sockaddr_in addr);
void game_created(struct game* game);
void host_p2p(struct netemu_sockaddr_in addr);
void connect_p2p();

int main() {
	struct netemu_sockaddr_in addr;
	char choice;
	  //mtrace();
	info = NULL;
	addr.addr = ADDR;
	addr.port = PORT;
	addr.family = NETEMU_AF_INET;
	netemu_init_network();
	//info = netemu_client_new(EMUNAME,games);
	//kaillera_communication_get_server_list(&servers, &games);
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	printf("1. Connect Async\n2. Connect\n3. Connect p2p\n4. host p2p cloud\n");
	choice = getchar();
	switch(choice) {
		case '1':
			connect_async(addr);
		break;
		case '2':
			server_connect(addr);
		break;
		case '3':
			connect_p2p();
		break;
		case '4':
			host_p2p(addr);
		break;
	}
	netemu_register_play_values_received_callback(info, receive_values);
	menu(info);
	//muntrace();
	return 0;
}

void host_p2p(struct netemu_sockaddr_in addr) {
	int port;
	struct netemu_p2p_connection *p2p;
	printf("Enter Port Number:\n");
	scanf("%d",&port);
	addr.port = netemu_htons(port);
	printf("\n");
	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME);
	netemu_p2p_host(p2p, &addr,sizeof(addr),CLOUD_NAME);
	info = p2p->info;
}

void connect_p2p() {
	int port;
	struct netemu_sockaddr_in in_addr;
	struct netemu_sockaddr_in out_addr;
	struct netemu_p2p_connection *p2p;

	in_addr.addr = ADDR;
	in_addr.family = NETEMU_AF_INET;

	out_addr.addr = ADDR;
	out_addr.family = NETEMU_AF_INET;

	printf("Enter Port Number for you:\n");
	scanf("%d",&port);
	in_addr.port = netemu_htons(port);
	printf("\n");

	printf("Enter Port Number to the host:\n");
	scanf("%d",&port);
	out_addr.port = netemu_htons(port);
	printf("\n");

	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME);
	netemu_p2p_connect(p2p,&in_addr,sizeof(in_addr),&out_addr,sizeof(out_addr));
	//struct netemu_p2p_connection* p2p, struct netemu_sockaddr_in *in_addr, int in_addr_size,  struct netemu_sockaddr_in *connect_addr, int connect_addr_size
	info = p2p->info;
}

void connect_async(struct netemu_sockaddr_in addr) {
	kaillera_communication_connect_async(&addr,sizeof(addr),EMUNAME,PLAYERNAME,login_success);
	while(info == NULL);
}

void server_connect(struct netemu_sockaddr_in addr) {
	info = kaillera_communication_connect(&addr,sizeof(addr),EMUNAME,PLAYERNAME);
}

void login_success(int status, struct netemu_info* server_connection) {
	info = server_connection;
}

void receive_values(struct buffered_play_values *result) {
	printf("%s",result->values);
}

void menu(struct netemu_info* connection) {
	char val;
	val = 'n';

	while(val != '0') {
		printf("1. CREATE GAME\n2. SHOW GAME LIST\n3. SHOW USER LIST\n4. JOIN GAME\n5. START GAME\n6. SEND PLAY VALUES\n7. SEND PLAYER READY");
		val = getchar();
		
		switch(val) {
			case '1':
				create_game(connection);
				break;
			case '2':
				show_game_list(connection);
				break;
			case '3':
				show_user_list(connection);
				break;
			case '4':
				join_game(connection);
				break;
			case '5':
				start_game(connection);
				break;
			case '6':
				send_play_values(connection);
				break;
			case '7':
				player_ready(connection);
				break;
		}
	}
	//_CrtDumpMemoryLeaks();
}

void send_play_values(struct netemu_info *connection) {
	char* data = VALUE;
	netemu_send_play_values(connection, strlen(data)+1, data);
}

void create_game(struct netemu_info* connection) {
	struct game *result;
	printf("Creating game\n");
	netemu_kaillera_create_game_async(connection,games[0],game_created);
}

void show_game_list(struct netemu_info* connection) {
	int no_games, i;
	struct game **games;
	games = netemu_kaillera_get_game_list(connection, &no_games);

	for(i = 0; i < no_games; i++) {
		if(games[i]->name != NULL) {
			printf("%s\n",games[i]->name);
		}
		else {
			printf("A name was null?");
		}
	}

}

void show_user_list(struct netemu_info* connection) {
	int no_users, i;
	struct user** users;
	users = server_connection_get_user_list(connection, &no_users);

	for(i = 0; i < no_users; i++) {
		printf("%s\n",users[i]->username);
	}
}

void game_created(struct game* game) {
	printf("%s", game->name);
}

void start_game(struct netemu_info *connection) {
	printf("Starting game\n");
	server_connection_start_game(connection);
}

void player_ready(struct netemu_info *connection) {
	netemu_send_player_ready(connection);
}


void join_game(struct netemu_info* connection) {
	struct game** list;
	int count;
	list = netemu_kaillera_get_game_list(connection,&count);
	if(count > 0) {
		netemu_kaillera_join_game(connection,list[0]->id);
	}
}
