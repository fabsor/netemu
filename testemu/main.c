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

/**
 * @file
 * This is the main file of the testemu test program.
 */
#include <stdlib.h>
#include "constants.h"
#include <stdio.h>
#include "netemu_p2p.h"
#include "netemu_kaillera.h"
#include "p2ptest.h"
#include "kailleratest.h"
char* games[] = { "Foo", "Bar" };
struct netemu_kaillera *info;

void menu(struct netemu_kaillera* connection);
void create_game(struct netemu_kaillera *connection);
void show_game_list(struct netemu_kaillera *connection);
void join_game(struct netemu_kaillera* connection);
void start_game(struct netemu_kaillera *connection);
void start_game_async(struct netemu_kaillera *connection);
void game_started_callback(struct netemu_kaillera *info, struct game* game, struct game_start *start, void* args);
void send_play_values(struct netemu_kaillera *connection);
void show_user_list(struct netemu_kaillera* connection);
void player_ready(struct netemu_kaillera *connection);
void connect_async();
void server_connect();
void connection_success(struct netemu_kaillera *info, int status, void *user_data);
void game_created(struct netemu_kaillera *info, struct game* new_game, void *user_data);
void host_p2p();
void connect_p2p();

/**
 * The main function contains some of the dispatching logic for this program.
 * it handles any valid parameters passed in, and will show an interactive menu
 * if no parameters are applicable.
 * @param argc the number of arguments passed to this program
 * @param argv an array containing the arguments.
 */
int main(int argc, char *argv[]) {
	int i;
	NETEMU_BOOL kaillera = FALSE;
	NETEMU_BOOL p2p = FALSE;
	NETEMU_BOOL creator = FALSE;
	int no_instructions = 1000;
	char choice;
		int connection = 1;
	info = NULL;

	/* Let's go through and parse the arguments. */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "kaillera") == 0) {
			kaillera = TRUE;
		}
		if (strcmp(argv[i], "p2p") == 0) {
			p2p = TRUE;
		}

		if (strcmp(argv[i], "creator") == 0) {
			creator = TRUE;
		}
		if (strstr(argv[i], "send=")) {
			no_instructions = atoi(argv[i] + strlen("send="));
		}
		if (strstr(argv[i], "connection=")) {
			connection = atoi(argv[i] + strlen("connection="));
		}
	}
	/* If we got a kaillera argument, we might be able to start some automatic tests. */
	if (kaillera) {
		if (creator) {
			run_kaillera_game_creator_test(no_instructions);
		} else {
			run_kaillera_game_joiner_test(no_instructions);
		}
	}
	/* If we got a p2p argument, we might be able to start some automatic tests. */
	else if (p2p) {
		if (creator) {
			run_p2p_host_test(no_instructions, (char) connection);
		} else {
			run_p2p_join_test(no_instructions, (char) connection);
		}
	}

	if (!kaillera && !p2p) {
		printf("1. Connect Async\n2. Connect\n3. Connect p2p\n4. host p2p cloud\n5. Connect p2p async\n6. Run p2p host test\n7. Run p2p join test");
		choice = getchar();
		switch (choice) {
		case '1':
			netemu_kaillera_initialize();
			connect_async();
			break;
		case '2':
			netemu_kaillera_initialize();
			server_connect();
			break;
		case '3':
			netemu_p2p_initialize();
			connect_p2p();
			break;
		case '4':
			netemu_p2p_initialize();
			host_p2p();
		case '5':
			connect_p2p_async();
			break;
		case '6':
			run_p2p_host_test(no_instructions, connection);
			break;
		case '7':
			run_p2p_join_test(no_instructions, connection);
			break;
		case '8':
			run_kaillera_game_creator_test(no_instructions);
			break;
		case '9':
			run_kaillera_game_joiner_test(no_instructions);
			break;
		}
	}
	return 0;
}

/**
 * Connect asyncronously to a server.
 */
void connect_async() {
	info = netemu_kaillera_create("a-sad-user", EMUNAME, 1);
	netemu_kaillera_connect_async(info, BIND_ADDR, 0, ADDR, PORT, connection_success, NULL);
	while (info == NULL);
	menu(info);
}

void connection_success(struct netemu_kaillera *info, int status, void *user_data) {
	printf("Hey dude!");
}
/**
 * Connect to a server.
 */
void server_connect() {
	info = netemu_kaillera_create("a-sad-user", EMUNAME, 1);
	netemu_kaillera_connect(info, BIND_ADDR, 0, ADDR, PORT);
	menu(info);
}

void game_created(struct netemu_kaillera *info, struct game* new_game, void *user_data) {
}

/**
 * Displays and handles a menu with applicable choices
 * when connected to a kaillera server.
 */
void menu(struct netemu_kaillera* connection) {
	char val;
	val = 'n';

	while (val != '0') {
		printf("1. CREATE GAME\n2. SHOW GAME LIST\n3. SHOW USER LIST\n4. JOIN GAME\n5. START GAME\n6. SEND PLAY VALUES\n7. SEND PLAYER READY\n\8. START GAME ASYNC\n9. DISCONNECT\nA. LEAVE GAME");
		val = getchar();

		switch (val) {
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
		case '8':
			start_game_async(connection);
		case '9':
			netemu_kaillera_disconnect(connection, "BYE");
			break;
		case 'A':
			netemu_kaillera_leave_game(connection);
			break;
		}
	}
}

/**
 * Send play values to the server.
 */
void send_play_values(struct netemu_kaillera *connection) {
	char* data = VALUE;
	netemu_kaillera_send_play_values(connection, strlen(data) + 1, data);
}

/**
 * Create a game.
 */
void create_game(struct netemu_kaillera* connection) {
	printf("Creating game\n");
	netemu_kaillera_create_game_async(connection, "thegame", game_created);
}



/**
 * Show a list of games.
 */
void show_game_list(struct netemu_kaillera* connection) {
	int no_games, i;
	struct game **games;
	games = netemu_kaillera_get_game_list(connection, &no_games);

	for (i = 0; i < no_games; i++) {
		if (games[i]->name != NULL) {
			printf("%s\n", games[i]->name);
		} else {
			printf("A name was null?");
		}
	}

}

/**
 * Show a list of users.
 */
void show_user_list(struct netemu_kaillera* connection) {
	int no_users, i;
	struct user** users;
	users = netemu_kaillera_get_user_list(connection, &no_users);

	for (i = 0; i < no_users; i++) {
		printf("%s\n", users[i]->username);
	}
}

/**
 * Callback for game started.
 */
void start_game(struct netemu_kaillera *connection) {
	printf("Starting game\n");
	netemu_kaillera_start_game(connection);
}

/**
 * Callback for game started.
 */
void start_game_async(struct netemu_kaillera *connection) {
	printf("Starting game\n");
	netemu_kaillera_start_game_async(connection, game_started_callback);
}

void game_started_callback(struct netemu_kaillera *info, struct game* game, struct game_start *start, void* args) {
	printf("game started man");
}

/**
 * Callback for player ready.
 */
void player_ready(struct netemu_kaillera *connection) {
	netemu_kaillera_send_player_ready(connection);
}

/**
 * Join a game.
 */
void join_game(struct netemu_kaillera* connection) {
	struct game** list;
	int count;
	list = netemu_kaillera_get_game_list(connection, &count);
	if (count > 0) {
		netemu_kaillera_join_game(connection, list[0]->id);
	}
}
