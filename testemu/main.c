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
#include "kaillera_communication.h"
#include "netemu_client.h"

#define EMUNAME		"testemu"
#define PLAYERNAME	"foobar"


#define ADDR	netemu_inet_addr("127.0.0.1")
#define PORT	netemu_htons(27888)
#define VALUE	"0x0x0xff"



char* games[] = {"Foo", "Bar"};
struct server_connection *connection;
#define NO_GAMES	2

void menu(struct server_connection* connection);
void create_game(struct server_connection *connection);
void show_game_list(struct server_connection *connection);
void join_game(struct server_connection* connection);
void start_game(struct server_connection *connection);

void send_play_values(struct server_connection *connection);
void show_user_list(struct server_connection* connection);
void player_ready(struct server_connection *connection);
void receive_values(struct buffered_play_values *result);
void login_success(int status, struct server_connection* connection);
void connect_async(struct netemu_sockaddr_in addr);
void server_connect(struct netemu_sockaddr_in addr);
void game_created(struct game* game);
int main() {
	struct netemu_sockaddr_in addr;
	char choice;


	connection = NULL;
	addr.addr = ADDR;
	addr.port = PORT;
	addr.family = NETEMU_AF_INET;
	netemu_init_network();
	//info = netemu_client_new(EMUNAME,games);
	//kaillera_communication_get_server_list(&servers, &games);
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	printf("1. Connect Async\n2. Connect\n");
	choice = getchar();
	switch(choice) {
		case '1':
			connect_async(addr);
		break;
		case '2':
			server_connect(addr);
		break;
	}
	server_connection_register_play_values_received_callback(connection, receive_values);
	menu(connection);
	return 0;
}

void connect_async(struct netemu_sockaddr_in addr) {
	kaillera_communication_connect_async(&addr,sizeof(addr),EMUNAME,PLAYERNAME,login_success);
	while(connection == NULL);
}

void server_connect(struct netemu_sockaddr_in addr) {
	connection = kaillera_communication_connect(&addr,sizeof(addr),EMUNAME,PLAYERNAME);
}

void login_success(int status, struct server_connection* server_connection) {
	connection = server_connection;
}

void receive_values(struct buffered_play_values *result) {
	printf("%s",result->values);
}

void menu(struct server_connection* connection) {
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

void send_play_values(struct server_connection *connection) {
	char* data = VALUE;
	server_connection_send_play_values(connection, strlen(data)+1, data);
}

void create_game(struct server_connection* connection) {
	struct game *result;
	printf("Creating game\n");
	server_connection_create_game_async(connection,games[0],game_created);
}

void show_game_list(struct server_connection* connection) {
	int no_games, i;
	struct game **games;
	games = server_connection_get_game_list(connection, &no_games);

	for(i = 0; i < no_games; i++) {
		if(games[i]->name != NULL) {
			printf("%s\n",games[i]->name);
		}
		else {
			printf("A name was null?");
		}
	}

}

void show_user_list(struct server_connection* connection) {
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

void start_game(struct server_connection *connection) {
	printf("Starting game\n");
	server_connection_create_game_async(connection, games[0], game_created);
}

void player_ready(struct server_connection *connection) {
	server_connection_send_player_ready(connection);
}


void join_game(struct server_connection* connection) {
	struct game** list;
	int count;
	list = server_connection_get_game_list(connection,&count);
	if(count > 0) {
		server_connection_join_game(connection,list[0]->id);
	}
}
