/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#include "netlib_network.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "netemu_p2p.h"
#include "netlib_thread.h"

void p2p_menu(struct netemu_p2p *connection);
void create_p2p_game(struct netemu_p2p* connection);
void respond_to_login(struct netemu_p2p *connection, int status);

void start_p2p_game(struct netemu_p2p *connection);
void p2p_send_play_values(struct netemu_p2p *connection);
void p2p_player_ready(struct netemu_p2p *connection);
void host_p2p();
void join_p2p_game(struct netemu_p2p *connection);
void show_p2p_game_list(struct netemu_p2p* connection);
void show_p2p_user_list(struct netemu_p2p* connection);
void p2p_player_joined_callback(struct netemu_p2p *connection, struct p2p_game *game, struct p2p_user *user);
void p2p_player_joined(struct netemu_p2p *p2p, struct p2p_game *game, void* arg);
void join_p2p_game_async(struct netemu_p2p *connection);

int logged_in = 0;

void host_p2p() {
	int port;
	struct netemu_p2p *p2p;
	printf("Enter Port Number:\n");
	scanf("%d",&port);
	printf("\n");
	p2p = netemu_p2p_create(EMUNAME,PLAYERNAME, 1);
	netemu_p2p_host(p2p, BIND_ADDR,netlib_htons(port),CLOUD_NAME);
	p2p_menu(p2p);
}

void connect_p2p() {
	int host_port, join_port;
	struct netemu_p2p *p2p;


	printf("Enter Port Number for you:\n");
	scanf("%d",&host_port);
	printf("\n");

	printf("Enter Port Number to the host:\n");
	scanf("%d",&join_port);
	printf("\n");
	p2p = netemu_p2p_create(EMUNAME,PLAYERNAME, 1);
	netemu_p2p_connect(p2p,BIND_ADDR,netlib_htons(host_port),ADDR,netlib_htons(join_port));
	p2p_menu(p2p);
}

void connect_p2p_async() {
	int port;
	netlib_sockaddr_in in_addr;
	netlib_sockaddr_in out_addr;
	struct netemu_p2p *p2p;

	in_addr.sin_addr.s_addr = ADDR;
	in_addr.sin_family = NETLIB_AF_INET;

	out_addr.sin_addr.s_addr = ADDR;
	out_addr.sin_family = NETLIB_AF_INET;

	printf("Enter Port Number for you:\n");
	scanf("%d",&port);
	in_addr.sin_port = netlib_htons(port);
	printf("\n");

	printf("Enter Port Number to the host:\n");
	scanf("%d",&port);
	out_addr.sin_port = netlib_htons(port);
	printf("\n");

	p2p = netemu_p2p_create(EMUNAME,PLAYERNAME, 1);
	netemu_p2p_connect_async(p2p,&in_addr,sizeof(in_addr),&out_addr,sizeof(out_addr), respond_to_login);
	while(logged_in != 1);
	p2p_menu(p2p);
}

void respond_to_login(struct netemu_p2p *connection, int status) {
	logged_in = status;
}

void p2p_menu(struct netemu_p2p *connection) {
	char val;
	val = 10;
	while(val != '0') {
		printf("1. CREATE GAME\n2. SHOW GAME LIST\n3. SHOW USER LIST\n4. JOIN GAME\n5. START GAME\n6. SEND PLAYER_READY\n7. SEND PLAY VALUES\n8. JOIN GAME ASYNC\n 9. LEAVE GAME\n A. DISCONNECT\n");
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
			case '5':
				start_p2p_game(connection);
				break;
			case '6':
				p2p_player_ready(connection);
				break;
			case '7':
				p2p_send_play_values(connection);
			case '8':
				join_p2p_game_async(connection);
			case '9':
				netemu_p2p_leave_game(connection);
			case 'A':
				netemu_p2p_disconnect(connection);
				break;

		}

	}
}

void start_p2p_game(struct netemu_p2p *connection) {
	netemu_p2p_start_game(connection,ADDR,PORT);
}

void p2p_send_play_values(struct netemu_p2p *connection) {
	char* data;
	data = malloc(512);
	memcpy(data, VALUE, sizeof(VALUE)+1);
	netemu_p2p_send_play_values(connection, data);
}

void p2p_player_ready(struct netemu_p2p *connection) {
	netlib_sockaddr_in addr;
	addr.sin_family = NETLIB_AF_INET;
	addr.sin_addr.s_addr = ADDR;
	addr.sin_port = netlib_htons(40001);
	netemu_p2p_player_ready(connection,(netlib_sockaddr*)&addr,sizeof(addr));
}

void join_p2p_game(struct netemu_p2p *connection) {
	struct p2p_game **p2p_games;
	int no_games;
	p2p_games = netemu_p2p_get_game_list(connection, &no_games);
	if(no_games > 0) {
		netemu_p2p_join_game(connection, p2p_games[0]);
	}
	else {
		printf("No games to Join!");
	}
}

void join_p2p_game_async(struct netemu_p2p *connection) {
	struct p2p_game **p2p_games;
	int no_games;
	p2p_games = netemu_p2p_get_game_list(connection, &no_games);
	if(no_games > 0) {
		netemu_p2p_join_game_async(connection, p2p_games[0], p2p_player_joined, NULL);
	}
	else {
		printf("No games to Join!");
	}
}

void p2p_player_joined(struct netemu_p2p *p2p, struct p2p_game *game, void* arg) {
	printf("yey");
}

void create_p2p_game(struct netemu_p2p* connection) {
	struct p2p_game *result;
	printf("Creating game\n");
	netemu_p2p_create_game(connection,"foo",1,sizeof(VALUE)+1,&result);

}

void show_p2p_game_list(struct netemu_p2p* connection) {
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

void show_p2p_user_list(struct netemu_p2p* connection) {
	int no_users, i;
	struct p2p_user** users;
	users = netemu_p2p_get_user_list(connection, &no_users);

	for(i = 0; i < no_users; i++) {
		printf("%s\n",users[i]->name);
	}
}

