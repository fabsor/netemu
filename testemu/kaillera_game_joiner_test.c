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
void register_kaillera_game_joiner_callbacks(struct netemu_kaillera *info);
void kaillera_game_joiner_game_created(struct netemu_kaillera *info, struct game* new_game, void *user_data);
void kaillera_game_creator_user_joined(struct netemu_kaillera *info, char *user, NETEMU_DWORD ping, char connection, void *user_data);
void kaillera_game_creator_player_joined(struct netemu_kaillera *info, struct player_joined *result);
void kaillera_game_creator_game_status(struct netemu_kaillera *info, struct game *game, struct game_status_update *update, void *user_data);
void kaillera_game_creator_values_received(struct netemu_kaillera *info, struct buffered_play_values *result, void *user_data);
void kaillera_game_joiner_game_start(struct netemu_kaillera *info, struct game* game, struct game_start *start, void* user_data);
int game_exists = 0;
int join_game_created = 0;
int join_game_started = 0;
netemu_event game_join_event;
void run_kaillera_game_joiner_test(no_instructions) {
	struct netemu_kaillera *info;
	struct game** games;
	int i, count;
	char *data;
	netemu_kaillera_initialize();
	game_join_event = netemu_thread_event_create();
	printf("Trying to connect to server...");
	info = netemu_kaillera_create("Christophe", EMUNAME, 1);
	register_kaillera_game_joiner_callbacks(info);
	netemu_kaillera_connect(info,BIND_ADDR, 0, ADDR, PORT);
	printf("OK!\nLooking for a game to join...");
	while(info->game_count == 0) {
		netemu_thread_event_wait(game_join_event, NETEMU_INFINITE);
	}
	printf("OK!\nJoining game...");
	games = netemu_kaillera_get_game_list(info,&count);
	netemu_kaillera_join_game(info, games[0]->id);
	printf("OK!\nWaiting for game start...");
	while(join_game_started == 0) {
		netemu_thread_event_wait(game_join_event, NETEMU_INFINITE);
	}
	printf("OK!\n Sending player ready...");
	netemu_kaillera_send_player_ready(info);
	printf("OK!\n We're ready for action!");
	data = malloc(512);
	for (i = 0; i < no_instructions; i++) {
		data = VALUE;
		netemu_kaillera_send_play_values(info, strlen(VALUE)+1, data);
	}

}

void register_kaillera_game_joiner_callbacks(struct netemu_kaillera *info) {
	//netemu_register_play_values_received_callback(info, kaillera_game_creator_values_received, NULL);
	netemu_register_game_created_callback(info, kaillera_game_joiner_game_created, NULL);
	netemu_register_game_started_callback(info, kaillera_game_joiner_game_start, NULL);
}

void kaillera_game_joiner_game_created(struct netemu_kaillera *info, struct game* new_game, void *user_data) {
	if(join_game_created == 0) {
		join_game_created = 1;
		netemu_thread_event_signal(game_join_event);
	}
}

void kaillera_game_joiner_game_start(struct netemu_kaillera *info, struct game* game, struct game_start *start, void* user_data) {
	if(join_game_started == 0) {
		join_game_started = 1;
		netemu_thread_event_signal(game_join_event);

	}
}
