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

#include "netemu_kaillera.h"
#include "netemu_thread.h"
#include "constants.h"
#include <stdio.h>
void register_kaillera_game_creator_callbacks(struct netemu_kaillera *info);
void kaillera_game_creator_user_joined(struct netemu_kaillera *info, char *user, NETEMU_DWORD ping, char connection, void *user_data);
void kaillera_game_creator_player_joined(struct netemu_kaillera *info, struct player_joined *result);
void kaillera_game_creator_game_status(struct netemu_kaillera *info, struct game *game, struct game_status_update *update, void *user_data);
void kaillera_game_creator_values_received(struct netemu_kaillera *info, struct buffered_play_values *result, void *user_data);
int kaillera_creator_test = 0;
int user_joined = 0;
int player_joined = 0;
int n = 0;
netemu_event game_creator_event;
void run_kaillera_game_creator_test(int no_instructions) {
	struct netemu_kaillera *info;
	struct game* game;
	int i;
	char *data;
	long int elapsed;
	netemu_kaillera_initialize();
	game_creator_event = netemu_thread_event_create();
	printf("Trying to connect to server...");
	info = netemu_kaillera_create(PLAYERNAME, EMUNAME, 1);
	register_kaillera_game_creator_callbacks(info);
	netemu_kaillera_connect(info,BIND_ADDR, 0, ADDR, PORT);
	printf("OK!\n Waiting for other users to join.");
	while(user_joined == 0) {
		netemu_thread_event_wait(game_creator_event, NETEMU_INFINITE);
	}
	printf("Other users have joined! Let's start a game...");
	netemu_kaillera_create_game(info, "gamename", &game);
	printf("OK!\n Now we wait for other players...");
	while(player_joined == 0) {
		netemu_thread_event_wait(game_creator_event, NETEMU_INFINITE);
	}
	printf("OK!\n Let's start the game...");
	/* We have liftoff! Let's start the game. */
	netemu_kaillera_start_game(info);
	printf("OK!\n Sending player ready...");
	netemu_kaillera_send_player_ready(info);
	printf("OK!\n We're ready for action!");
	data = malloc(512);

	for (i = 0; i < no_instructions; i++) {
		data = VALUE;
		netemu_kaillera_send_play_values(info, strlen(VALUE)+1, data);
	}
}

void register_kaillera_game_creator_callbacks(struct netemu_kaillera *info) {
	netemu_register_user_join_callback(info, kaillera_game_creator_user_joined, NULL);
	netemu_register_game_status_updated_callback(info, kaillera_game_creator_game_status, NULL);
}

void kaillera_game_creator_user_joined(struct netemu_kaillera *info, char *user, NETEMU_DWORD ping, char connection, void *user_data) {
	/* Let's create a game once another user has joined. */
	if(info->user_count > 1 && !user_joined) {
		user_joined = 1;
		netemu_thread_event_signal(game_creator_event);
	}

}
void kaillera_game_creator_game_status(struct netemu_kaillera *info, struct game *game, struct game_status_update *update, void *user_data) {
	if(update->num_players > 1 && !player_joined) {
		player_joined = 1;
		netemu_thread_event_signal(game_creator_event);
	}
}

void kaillera_game_creator_player_ready(struct netemu_kaillera *info) {
	/* We are ready, let's go. */
	netemu_kaillera_send_play_values(info, strlen(VALUE)+1, VALUE);

}
