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
#pragma once
#ifndef NETEMU_APPLICATION_KAILLERA_H_
#define NETEMU_APPLICATION_KAILLERA_H_

#include "transport.h"
#include "netlib_util.h"
#include "application.h"
#include "netlib_network.h"
#include <time.h>
/* Size of the application_instruction struct excluding the body. */
#define APPLICATION_INSTRUCTION_SIZE	33;

struct login_request {
	char *name;
	char connection;
};

struct user {
	char *username;
	NETEMU_DWORD ping;
	char status;
	NETEMU_WORD id;
	char connection;
};

struct player {
	char *username;
	NETEMU_DWORD ping;
	NETEMU_WORD id;
	char connection;
};

struct login_success {
	NETEMU_DWORD users_count;
	NETEMU_DWORD games_count;
	struct user **users;
	struct game **games;
};

struct login_status {
	NETEMU_WORD id;
	char message[128];
};
/*
struct ping {
	int pbody[3];
};
*/
struct pong {
	char pbody[16];
};

struct user_joined {
	NETEMU_WORD id;
	NETEMU_DWORD ping;
	char connection;
};

struct game {
	char* name;
	char* app_name;
	char* users_count;
	char status;
	int player_count;
	struct user *creator;
	struct existing_player_list *players;
	/* Used in kaillera */
	NETEMU_DWORD id;
};

struct user_left {
	NETEMU_WORD id;
	char *exit_message;
};

struct game_created {
	char *gameName;
	char *appName;
	NETEMU_DWORD id;
	/* Nobody has any idea what this thing is supposed to do, but it needs to be included. */
	NETEMU_DWORD wtf;
};

struct game_closed {
	unsigned int id;
};

struct game_status_update {
	NETEMU_DWORD id;
	char status;
	char num_players;
	char max_players;
};

struct player_joined {
	NETEMU_DWORD game_id;
	char* username;
	NETEMU_DWORD ping;
	NETEMU_WORD user_id;
	char connection;
};

struct player_dropped {
	char player_number;
};

struct player_left {
	NETEMU_WORD user_id;
};

struct existing_player_list {
	NETEMU_DWORD players_count;
	struct player *players;
};

struct kick_player {
	NETEMU_WORD user_id;
};

struct game_start {
	NETEMU_WORD time_band;
	char player_no;
	char max_players;
};

struct buffered_play_values {
	NETEMU_WORD size;
	char *values;
};

struct intelligently_cached_buffered_play_values {
	char index;
};

struct chat {
	char *message;
};

void netemu_application_buffered_play_values_add(struct application_instruction *instruction, NETEMU_WORD size, char *data);

void netemu_application_buffered_play_values_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_buffered_play_values_parse(struct application_instruction *instruction, char *data);

int netemu_application_chat_game_add(struct application_instruction *instruction, char *data, char *user);

int netemu_application_chat_partyline_add(struct application_instruction *instruction, char *data, char *user);

void netemu_application_chat_pack(struct application_instruction *instruction, char *buffer);

int netemu_application_chat_parse(struct application_instruction *instruction, char *data);

void netemu_application_create_game_add(struct application_instruction *instruction, char* gamename);

void netemu_application_create_game_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_create_game_parse(struct application_instruction *instruction, char* buffer);

void netemu_application_existing_players_list_parse(struct application_instruction *instruction, char *data);

void netemu_application_game_status_parse(struct application_instruction *instruction, char* buffer);

void netemu_application_intelligently_cached_play_values_add(struct application_instruction *instruction, char index);

void netemu_application_intelligently_cached_play_values_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_intelligently_cached_play_values_parse(struct application_instruction *instruction, char *data);

void netemu_application_join_game_add(struct application_instruction *instruction, NETEMU_DWORD game_id, char connection);

void netemu_application_join_game_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_join_game_parse(struct application_instruction *instruction, char *data);

void netemu_application_kick_player_add(struct application_instruction* instruction, NETEMU_WORD user_id);

void netemu_application_kick_player_pack(struct application_instruction* instruction, char* buffer);

void netemu_application_kick_player_parse(struct application_instruction* instruction, char* buffer);

void netemu_application_leave_add(struct application_instruction* instruction, char* exit_message);

void netemu_application_leave_pack(struct application_instruction *instruction, char *buffer);

int netemu_application_login_request_add(struct application_instruction* instruction, char* appName, char* user, char connection);

void netemu_application_login_request_pack(struct application_instruction *instruction, char *buffer);

int netemu_application_login_success_parse(struct application_instruction *instruction, char* buffer);

void netemu_application_player_dropped_pack(struct application_instruction *instruction, char *data);

void netemu_application_player_dropped_parse(struct application_instruction *instruction, char *data);

void netemu_application_player_left_add(struct application_instruction* instruction);

void netemu_application_player_left_pack(struct application_instruction* instruction, char* buffer);

int netemu_application_player_left_parse(struct application_instruction *instruction, char* buffer);

void netemu_application_player_joined_parse(struct application_instruction* instruction, char* buffer);

void netemu_application_player_ready_add(struct application_instruction* instruction);

void netemu_application_pong_add(struct application_instruction* instruction);

void netemu_application_pong_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_start_game_add(struct application_instruction* instruction);

void netemu_application_start_game_pack(struct application_instruction* instruction, char* buffer);

void netemu_application_start_game_parse(struct application_instruction* instruction, char* buffer);

void netemu_application_user_joined_parse(struct application_instruction *instruction, char* buffer);

void netemu_application_user_leave_add(struct application_instruction* instruction, char* exit_message);

void netemu_application_user_leave_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_user_leave_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_client_timeout_request_add(struct application_instruction* instruction);

void netemu_application_buffered_play_values_copy(struct buffered_play_values *target, struct buffered_play_values *values);
#endif /* NETEMU_APPLICATION_KAILLERA_H_ */
