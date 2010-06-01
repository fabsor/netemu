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
#pragma once
#ifndef APPLICATION_P2P_INTERNAL_H_
#define APPLICATION_P2P_INTERNAL_H_

#include "application_p2p.h"
#include "netlib_thread.h"

/**
 * This struct holds internal stuff that can be interesting to
 * know about a user. It is used internally by the netemu_p2p
 * and application_p2p module
 * @ingroup application_p2p
 */
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
/**
 * This struct holds internal stuff that can be interesting to
 * know about a game. It is used internally by the netemu_p2p
 * and application_p2p module
 * @ingroup application_p2p
 */
struct p2p_game_internal {
	struct netemu_sender_collection *tcp_collection; /**< This is used for status update messages */
	struct netemu_sender_collection *udp_collection; /**< This is used for play values */
	NETEMU_WORD ready_count;
	NETEMU_BOOL sent_first_values; /**< We need to know that we haven't sent any values, and that we can't have any data to fetch from */
	NETEMU_BOOL all_values_received;
	netlib_mutex game_lock;
};

/**
 * Add a "Create game" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param gamename the name of the game.
 * @param appname the name of the application.
 * @param creator the user who created the game.
 * @param connection_quality the connection quality.
 * @param value_size the size of the player value sent between clients.
 */
void netemu_application_p2p_create_game_add(struct application_instruction *instruction, char* gamename, char* appname,
		struct p2p_user* creator, char connection_quality, NETEMU_WORD value_size);

/**
 * Add a "Leave Game" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 */
void netemu_application_p2p_leave_game_add(struct application_instruction* instruction);

/**
 * Parse a "Login success" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_login_success_parse(struct application_instruction *instruction, char *buffer);

/**
 * Add a "Login Success" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param host the user the client connected to.
 * @param users a list of users in the cloud.
 * @param games a list of games in the cloud.
 */
void netemu_application_p2p_login_success_add(struct application_instruction *instruction,struct p2p_user *host, struct netemu_list *users, struct netemu_list *games);

/**
 * Pack a "Login success" body and put it in the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to read from.
 * @param buffer the buffer to write the data to.
 */
void netemu_application_p2p_login_success_pack(struct application_instruction *instruction, char *buffer);

/**
 * Add a "User Join" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param user the user to add to the instruction.
 */
void netemu_application_p2p_user_join_add(struct application_instruction *instruction, struct p2p_user *user);

/**
 * Add a "Kick player" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @todo Fix this, should it even be here?
 */
void netemu_application_p2p_kick_player_add(struct application_instruction *instruction, char* player_name);

/**
 * Pack a "Login request" body and put it in the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to read from.
 * @param buffer the buffer to write the data to.
 */
void netemu_application_p2p_login_request_pack(struct application_instruction *instruction, char* buffer);

/**
 * Parse a "Login request" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_login_request_parse(struct application_instruction *instruction, char *buffer);

/**
 * Add a "Login request" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 */
void netemu_application_p2p_login_request_add(struct application_instruction *instruction, unsigned long addr, unsigned short port, char* username, char* appname, int connection);

/**
 * Add a "Player ready" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 */
void netemu_application_p2p_ready_add(struct application_instruction *instruction);

/**
 * Copy a user.
 * @ingroup application_p2p
 * @param target the target to which the data should be copied.
 * @param user the original from which the data should be copied
 * @return 0 if everything went OK, -1 otherwise.
 */
int netemu_application_p2p_copy_user(struct p2p_user *target, struct p2p_user *user);

/**
 * Copy a game.
 * @ingroup application_p2p
 * @param target the target to which the data should be copied.
 * @param user the original from which the data should be copied
 * @return 0 if everything went OK, -1 otherwise.
 */
int netemu_application_p2p_copy_game(struct p2p_game *target, struct p2p_game *game);

/**
 * Parse a "Create game" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_create_game_parse(struct application_instruction *instruction, char *buffer);

/**
 * Parse a "User join" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_user_join_parse(struct application_instruction *instruction, char *buffer);

/**
 * Parse a "Player join" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_player_join_parse(struct application_instruction *instruction, char *buffer);

/**
 * Add a "Player join" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param user the user to that joined.
 */
void netemu_application_p2p_player_join_add(struct application_instruction *instruction, struct p2p_user *user);

/**
 * Add a "Join Host" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param user the user to that joined.
 */
void netemu_application_p2p_join_host_add(struct application_instruction *instruction, struct p2p_user *user);

/**
 * Parse a "Join host" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_join_host_parse(struct application_instruction *instruction, char *buffer);

/**
 * Add a "Start game" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param addr the address in network byte order.
 * @param port the port in network byte order.
 */
void netemu_application_p2p_start_game_add(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

/**
 * Add a start game to an instruction without changing anything but the body.
 */
void netemu_application_add_start_game(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

/**
 * Parse a "Start game" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_start_game_parse(struct application_instruction *instruction, char *buffer);

/**
 * Add a "Player leave" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param player_no the number of the player.
 */
void netemu_application_p2p_player_leave_add(struct application_instruction* instruction, struct p2p_user *user);

/**
 * Add a "Player ready" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param addr the address in network byte order.
 * @param port the port in network byte order.
 */
void netemu_application_p2p_player_ready_add(struct application_instruction *instruction, NETEMU_DWORD addr, unsigned short port);

/**
 * Add a "Cached Play values" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param player_no the player number.
 * @param index the index in the cache array.
 */
void netemu_application_p2p_cached_play_values_add(struct application_instruction *instruction, char player_no, char index);

/**
 * Add a "Cached Play values" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param player_no the player number.
 * @param size the size of the play value.
 * @param data the bloc of data
 * @todo size is no longer needed, it can be removed from this instruction.
 */
void netemu_application_p2p_buffered_play_values_add(struct application_instruction *instruction, char player_no, char size, void* data);

/**
 * Parse a "Play values" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_buffered_play_values_parse(struct application_instruction *instruction, char *data);

/**
 * Parse a "Cached play values" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_cached_play_values_parse(struct application_instruction *instruction, char *data);

/**
 * Copy buffered play values.
 */
void netemu_application_p2p_buffered_play_values_copy(struct p2p_buffered_play_values *target, struct p2p_buffered_play_values *values);

/**
 * Add a "request denied" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 */
void netemu_p2p_login_request_denied_add(struct application_instruction *instruction);

/**
 * Add a "Player join success" body to an instruction.
 * @ingroup application_p2p
 * @param instruction the instruction to add the body to.
 * @param game the game to send.
 */
void netemu_application_p2p_player_join_success_add(struct application_instruction *instruction, struct p2p_game *game);

/**
 * Parse a "Player join success" body from the buffer.
 * @ingroup application_p2p
 * @param instruction the instruction to save the body to.
 * @param buffer the buffer to read the data from.
 */
void netemu_application_p2p_player_join_success_parse(struct application_instruction *instruction, char *buffer);

/**
 * Create the internal struct of a game.
 * @ingroup application_p2p
 * @return an empty p2p_game_internal struct.
 */
struct p2p_game_internal *netemu_application_p2p_create_game_internal();

/**
 * Create the internal struct of a user.
 * @ingroup application_p2p
 * @return an empty p2p_user_internal struct.
 */
struct p2p_user_internal* netemu_application_p2p_create_user_internal();

void netemu_application_p2p_user_leave_add(struct application_instruction* instruction, struct p2p_user *user);

void netemu_application_p2p_user_leave_parse(struct application_instruction *instruction, char *buffer);

void netemu_application_p2p_destroy_game(struct p2p_game *game, NETEMU_BOOL free_ptr, NETEMU_BOOL free_internal, NETEMU_BOOL free_connections);

void netemu_application_p2p_destroy_user(struct p2p_user *user, NETEMU_BOOL free_ptr, NETEMU_BOOL free_internal);

#endif /* APPLICATION_P2P_INTERNAL_H_ */
