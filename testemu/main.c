/*
 * main.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
#include <stdlib.h>
#include "constants.h"
#include <stdio.h>
#include "netemu_p2p.h"
#include "netemu_kaillera.h"
#include "p2ptest.h"
#include "kailleratest.h"
char* games[] = {"Foo", "Bar"};
struct netemu_kaillera *info;

void menu(struct netemu_kaillera* connection);
void create_game(struct netemu_kaillera *connection);
void show_game_list(struct netemu_kaillera *connection);
void join_game(struct netemu_kaillera* connection);
void start_game(struct netemu_kaillera *connection);

void send_play_values(struct netemu_kaillera *connection);
void show_user_list(struct netemu_kaillera* connection);
void player_ready(struct netemu_kaillera *connection);
void receive_values(struct buffered_play_values *result);

void login_success(int status, struct netemu_kaillera* server_connection, void *arg);
void connect_async(netemu_sockaddr_in addr);
void server_connect(netemu_sockaddr_in addr);
void game_created(struct game* game);
void host_p2p(netemu_sockaddr_in addr);
void connect_p2p();
int main(int argc, char *argv[]) {
	netemu_sockaddr_in addr;
	int i;
	NETEMU_BOOL kaillera = FALSE;
	NETEMU_BOOL p2p = FALSE;
	NETEMU_BOOL creator = FALSE;
	int no_instructions = 1000;
	char choice;
	info = NULL;
	int connection;


	for(i = 0; i < argc; i++) {
		if(strcmp(argv[i],"kaillera") == 0) {
			kaillera = TRUE;
		}
		if(strcmp(argv[i], "p2p") == 0) {
			p2p = TRUE;
		}

		if(strcmp(argv[i],"creator") == 0) {
			creator = TRUE;
		}
		if(strstr(argv[i],"send=")) {
			no_instructions = atoi(argv[i] + strlen("send="));
		}
		if(strstr(argv[i], "connection=")) {
			connection = atoi(argv[i] + strlen("connection="));
		}
	}

	if(kaillera) {
		if(creator) {
			run_kaillera_game_creator_test(no_instructions);
		}
		else {
			run_kaillera_game_joiner_test(no_instructions);
		}
	}
	else if(p2p) {
		if(creator) {
			run_p2p_host_test(no_instructions, (char)connection);
		}
		else {
			run_p2p_join_test(no_instructions, (char)connection);
		}
	}

	if(!kaillera) {
		addr.sin_addr.s_addr = ADDR;
		addr.sin_port = PORT;
		addr.sin_family = NETEMU_AF_INET;
		netemu_init_network();
		//info = netemu_client_new(EMUNAME,games);
		//kaillera_communication_get_server_list(&servers, &games);
		//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

		printf("1. Connect Async\n2. Connect\n3. Connect p2p\n4. host p2p cloud\n5. Connect p2p async\n6. Run p2p host test\n7. Run p2p join test");
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
			case '5':
				connect_p2p_async(addr);
				break;
			case '6':
				run_p2p_host_test(no_instructions);
				break;
			case '7':
				run_p2p_join_test(no_instructions);
				break;
			case '8':
				run_kaillera_game_creator_test(no_instructions);
			break;
			case '9':
				run_kaillera_game_joiner_test(no_instructions);
			break;
		}
	}
	//muntrace();
	return 0;
}

void connect_async(netemu_sockaddr_in addr) {
	kaillera_communication_connect_async(&addr,sizeof(addr),EMUNAME,PLAYERNAME,login_success, NULL);
	while(info == NULL);
	netemu_register_play_values_received_callback(info, receive_values, NULL);
	menu(info);
}

void server_connect(netemu_sockaddr_in addr) {
	info = netemu_kaillera_create("a-sad-user", EMUNAME, 1);
	netemu_kaillera_connect(info, BIND_ADDR, 0, ADDR, PORT);
	netemu_register_play_values_received_callback(info, receive_values, NULL);
	menu(info);
}

void login_success(int status, struct netemu_kaillera* server_connection, void *arg) {
	info = server_connection;
}

void receive_values(struct buffered_play_values *result) {
	printf("%s",result->values);
}

void menu(struct netemu_kaillera* connection) {
	char val;
	val = 'n';

	while(val != '0') {
		printf("1. CREATE GAME\n2. SHOW GAME LIST\n3. SHOW USER LIST\n4. JOIN GAME\n5. START GAME\n6. SEND PLAY VALUES\n7. SEND PLAYER READY\n");
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


void send_play_values(struct netemu_kaillera *connection) {
	char* data = VALUE;
	netemu_kaillera_send_play_values(connection, strlen(data)+1, data);
}

void create_game(struct netemu_kaillera* connection) {
	struct game *result;
	printf("Creating game\n");
	netemu_kaillera_create_game_async(connection,games[0],game_created);
}

void show_game_list(struct netemu_kaillera* connection) {
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

void show_user_list(struct netemu_kaillera* connection) {
	int no_users, i;
	struct user** users;
	users = netemu_kaillera_get_user_list(connection, &no_users);

	for(i = 0; i < no_users; i++) {
		printf("%s\n",users[i]->username);
	}
}

void game_created(struct game* game) {
	printf("%s", game->name);
}

void start_game(struct netemu_kaillera *connection) {
	printf("Starting game\n");
	netemu_kaillera_start_game(connection);
}

void player_ready(struct netemu_kaillera *connection) {
	netemu_kaillera_send_player_ready(connection);
}


void join_game(struct netemu_kaillera* connection) {
	struct game** list;
	int count;
	list = netemu_kaillera_get_game_list(connection,&count);
	if(count > 0) {
		netemu_kaillera_join_game(connection,list[0]->id);
	}
}
