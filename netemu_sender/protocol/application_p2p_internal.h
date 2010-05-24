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
 * This header file contains functions and structs that are used by the p2p application layer and should not be exposed to the public.
 */

#ifndef APPLICATION_P2P_INTERNAL_H_
#define APPLICATION_P2P_INTERNAL_H_

#include "application_p2p.h"
#include "netemu_thread.h"
/* TODO: This can't be here but I don't have the time to play nice right now =)*/
struct p2p_user_internal {
	struct netemu_tcp_connection *connection;
	struct netemu_sender_udp *sender; /**< The sender for udp packages */
	struct netemu_receiver_udp *receiver; /**< The receiver for udp packages*/
	struct netemu_list *play_values; /**< Play values received since last time. */
	struct p2p_buffered_play_values cache[256];
	int current_index; /**< The currently seleced index. */
	NETEMU_BOOL values_received;
	int cache_index;
	int player_no;
};

struct p2p_game_internal {
	struct netemu_sender_collection *tcp_collection; /**< This is used for status update messages */
	struct netemu_sender_collection *udp_collection; /**< This is used for play values */
	NETEMU_WORD ready_count;
	NETEMU_BOOL sent_first_values; /**< We need to know that we haven't sent any values, and that we can't have any data to fetch from */
	NETEMU_BOOL all_values_received;
	netemu_mutex game_lock;
};

void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname,
		struct p2p_user* creator, char connection_quality, NETEMU_WORD value_size);

void netemu_application_p2p_leave_game_add(struct application_instruction* instruction);

void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_success_add(struct application_instruction *instruction,struct p2p_user *host, struct netemu_list *users, struct netemu_list *games);

void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name);

void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer);

void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_login_request_add(struct application_instruction *instruction, unsigned long addr, unsigned short port, char* username, char* appname, int connection);

void netemu_application_p2p_ready_add(struct application_instruction *instruction);

int netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user);

int netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game);

void netemu_application_p2p_create_game_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_user_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_join_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_join_host_add(struct application_instruction *instruction, struct p2p_user *user);

void netemu_application_p2p_join_host_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_start_game_add(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

void netemu_application_add_start_game(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

void netemu_application_p2p_start_game_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_player_ready_add(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

void netemu_application_p2p_cached_play_values_add(struct application_instruction *instruction, char player_no, char index);

void netemu_application_p2p_buffered_play_values_add(struct application_instruction *instruction, char player_no, char size, void* data);

void netemu_application_p2p_buffered_play_values_parse(struct application_instruction *instruction, char *data);

void netemu_application_p2p_cached_play_values_parse(struct application_instruction *instruction, char *data);

void netemu_application_p2p_buffered_play_values_copy(struct p2p_buffered_play_values *target, struct p2p_buffered_play_values *values);

void netemu_p2p_login_request_denied_add(struct application_instruction *instruction);

void netemu_application_p2p_player_join_success_add(struct application_instruction *instruction, struct p2p_game *game);

void netemu_application_p2p_player_join_success_parse(struct application_instruction *instruction, char *buffer);

struct p2p_game_internal *netemu_application_p2p_create_game_internal();

struct p2p_user_internal* netemu_application_p2p_create_user_internal();

#endif /* APPLICATION_P2P_INTERNAL_H_ */
