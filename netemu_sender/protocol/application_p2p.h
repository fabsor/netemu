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
 * This file contains structs used when handling p2p instructions.
 */

/**
 * @defgroup application_p2p Netemu p2p instructions
 * Functions that handle p2p instructions.
 */
#pragma once
#ifndef NETEMU_APPLICATION_P2P_H_
#define NETEMU_APPLICATION_P2P_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "netlib_util.h"

/**
 * This struct represents a player. It also contains everything that
 * is included in a player instruction.
 * @ingroup application_p2p
 */
struct p2p_user {
	char *name; /**< The name of the player. This is included in the instructions all sends users. */
	char *app_name; /**< The name of the application the player is using. This is included in all instructions that sends users. */
	NETEMU_DWORD addr; /**< The address to the player in network byte order. This is included in all instructions that sends users.  */
	NETEMU_WORD port; /**< The port to the player in network byte order. This is included in all instructions that sends users.  */
	NETEMU_DWORD ping; /**< The player ping. This is included in all instructions that sends users.  */
	char connection; /**< The connection quality. This is included in all instructions that sends users.  */
	struct p2p_user_internal *_internal; /**< A struct containing private values that are being used by the internal structure of the program. Nothing in this struct is included in any instruction. Do not try to modify this.*/
};
/**
 * This struct represents a game. It also contains everything that is included in a game instruction.
 * @ingroup application_p2p
 */
struct p2p_game {
	char* name; /**< The name of game */
	char* app_name; /**< The application the game uses */
	NETEMU_WORD user_count; /**< The number of users participating in this game. */
	struct p2p_user* creator; /**< The creator of the game. */
	struct p2p_user* players; /**< A list of players. */
	NETEMU_BOOL started; /**< Determines if the game has started or not. You cannot join started games. */
	NETEMU_BOOL received_start_signal; /**< Determines if this game has received a start signal, meaning that the game_started instruction has been received.  */
	char connection_quality; /**< Connection quality. This determines how often packets will be sent in this game.  */
	NETEMU_WORD emulator_value_size; /**< The size of values sent over the network for ONE player.  */
	struct p2p_game_internal *_internal; /**< A struct containing private values that are being used by the internal structure of the program. Nothing in this struct is included in any instruction. Do not try to modify this.*/
};

/**
 * This struct represents a p2p start game and a player_join instruction.
 * @ingroup application_p2p
 */
struct p2p_start_game {
	NETEMU_DWORD addr; /**< The address in network byte order. This is the address a player is supposed to send values to.  */
	unsigned short port; /**< The port in network byte order. This is the port a player is supposed to send values to.  */
};
/**
 * This struct represents a p2p login success instruction.
 * @ingroup application_p2p
 */
struct p2p_login_success {
	NETEMU_WORD users_count; /**< The number of users that exist in the cloud. */
	NETEMU_WORD games_count; /**< The number of games that exist in the cloud. */
	struct p2p_user *users; /**< An array of users that exists in the cloud. */
	struct p2p_game *games; /**< A array of games taht exists in the cloud. */
};

/**
 * This struct represents buffered_play_values
 * @ingroup application_p2p
 */
struct p2p_buffered_play_values {
	char player_no; /**< The player number.  */
	NETEMU_WORD size; /**< The size of the values */
	char *values; /**< The data in the instruction. */
};

/**
 * This truct represents p2p cached buffered values.
 * @ingroup application_p2p
 */
struct p2p_cached_buffered_play_values {
	char player_no; /**< The number of the plaeyr */
	char index; /**< The index in the players cache */
};

/**
 * This struct represents a player leave instruction.
 * @ingroup application_p2p
 */
struct p2p_leave_player {
	struct p2p_user *player; /**< The player number. */
};

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_APPLICATION_P2P_H_ */
