/*
 * main.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
//#define _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include "constants.h"
//#include <crtdbg.h>
#include <stdio.h>
#include "netemu_p2p.h"
#include "netemu_kaillera.h"
#include "p2ptest.h"
char* games[] = {"Foo", "Bar"};
struct netemu_info *info;

void menu(struct netemu_info* connection);
void create_game(struct netemu_info *connection);
void show_game_list(struct netemu_info *connection);
void join_game(struct netemu_info* connection);
void start_game(struct netemu_info *connection);

void send_play_values(struct netemu_info *connection);
void show_user_list(struct netemu_info* connection);
void player_ready(struct netemu_info *connection);
void receive_values(struct buffered_play_values *result);

void login_success(int status, struct netemu_info* server_connection, void *arg);
void connect_async(netemu_sockaddr_in addr);
void server_connect(netemu_sockaddr_in addr);
void game_created(struct game* game);
void host_p2p(netemu_sockaddr_in addr);
void connect_p2p();

int main() {
	netemu_sockaddr_in addr;
	char choice;
	info = NULL;
	addr.sin_addr.s_addr = ADDR;
	addr.sin_port = PORT;
	addr.sin_family = NETEMU_AF_INET;
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
			/*connect_p2p();*/
		break;
		case '4':
			/*host_p2p(addr);*/
		break;
	}

	//muntrace();
	return 0;
}


void connect_async(netemu_sockaddr_in addr) {
	kaillera_communication_connect_async(&addr,sizeof(addr),EMUNAME,PLAYERNAME,login_success, NULL);
	while(info == NULL);
	netemu_register_play_values_received_callback(info, receive_values);
	menu(info);
}

void server_connect(netemu_sockaddr_in addr) {
	info = kaillera_communication_connect(&addr,sizeof(addr),EMUNAME,PLAYERNAME);
	netemu_register_play_values_received_callback(info, receive_values);
	menu(info);
}

void login_success(int status, struct netemu_info* server_connection, void *arg) {
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
