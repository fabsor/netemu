/*
 * p2ptest.c
 *
 *  Created on: 28 apr 2010
 *      Author: fabian
 */
#include "netemu_socket.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "netemu_p2p.h"

void p2p_menu(struct netemu_p2p_connection *connection);
void create_p2p_game(struct netemu_p2p_connection* connection);

void host_p2p(netemu_sockaddr_in addr) {
	int port;
	struct netemu_p2p_connection *p2p;
	printf("Enter Port Number:\n");
	scanf("%d",&port);
	addr.sin_port = netemu_htons(port);
	printf("\n");
	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME);
	netemu_p2p_host(p2p, &addr,sizeof(addr),CLOUD_NAME);
	p2p_menu(p2p);
}

void connect_p2p() {
	int port;
	netemu_sockaddr_in in_addr;
	netemu_sockaddr_in out_addr;
	struct netemu_p2p_connection *p2p;

	in_addr.sin_addr.s_addr = ADDR;
	in_addr.sin_family = NETEMU_AF_INET;

	out_addr.sin_addr.s_addr = ADDR;
	out_addr.sin_family = NETEMU_AF_INET;

	printf("Enter Port Number for you:\n");
	scanf("%d",&port);
	in_addr.sin_port = netemu_htons(port);
	printf("\n");

	printf("Enter Port Number to the host:\n");
	scanf("%d",&port);
	out_addr.sin_port = netemu_htons(port);
	printf("\n");

	p2p = netemu_p2p_new(EMUNAME,PLAYERNAME);
	netemu_p2p_connect(p2p,&in_addr,sizeof(in_addr),&out_addr,sizeof(out_addr));
	//struct netemu_p2p_connection* p2p, struct netemu_sockaddr_in *in_addr, int in_addr_size,  struct netemu_sockaddr_in *connect_addr, int connect_addr_size
	p2p_menu(p2p);
}

void p2p_menu(struct netemu_p2p_connection *connection) {
	char val;
	val = 10;
	while(val != '0') {
		printf("1. CREATE GAME\n2. SHOW GAME LIST\n3. SHOW USER LIST\n4. JOIN GAME\n5. START GAME\n6. SEND PLAY VALUES\n7. SEND PLAYER READY");
		val = getchar();

		switch(val) {
			case '1':
				create_p2p_game(connection);
				break;
			case '2':
				show_p2p_game_list(connection);
				break;
			case '3':
				show_p2p_user_list(connection);
				break;
			case '4':
				join_p2p_game(connection);
				break;
		}

	}
}

void join_p2p_game(struct netemu_p2p_connection *connection) {
	struct p2p_game **p2p_games;
	int no_games;
	p2p_games = netemu_p2p_get_game_list(connection, &no_games);
	if(no_games > 0) {
		netemu_p2p_join_game(connection, p2p_games[0]->creator);
	}
	else {
		printf("No games to Join!");
	}
}

void create_p2p_game(struct netemu_p2p_connection* connection) {
	struct p2p_game *result;
	printf("Creating game\n");
	netemu_p2p_create_game(connection,"foo",&result);

}

void show_p2p_game_list(struct netemu_p2p_connection* connection) {
	int no_games, i;
	struct p2p_game **p2p_games;

	p2p_games = netemu_p2p_get_game_list(connection, &no_games);

	for(i = 0; i < no_games; i++) {
		if(p2p_games[i]->name != NULL) {
			printf("%s\n",p2p_games[i]->name);
		}
		else {
			printf("A name was null?");
		}
	}
}


void show_p2p_user_list(struct netemu_info* connection) {
	int no_users, i;
	struct p2p_user** users;
	users = netemu_p2p_get_user_list(connection, &no_users);

	for(i = 0; i < no_users; i++) {
		printf("%s\n",users[i]->name);
	}
}

