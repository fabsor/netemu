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
 * This file contains functions handlig the p2p callbacks.
 */

#include "../interface/netemu_p2p.h"
#include "netemu_p2p_internal.h"

int netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable, void *user_data);

int netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable, void *user_data) {
	struct p2p_callback *callback;

	if((callback = malloc(sizeof(struct p2p_callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->user_data = user_data;
	callback->disposable = disposable;
	netemu_list_add(list,callback);
	return 0;
}

int netemu_p2p_register_play_values_received_callback(struct netemu_p2p_connection *connection, p2pValuesReceivedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.valuesFn = callback;

	netemu_p2p_register_callback(connection->_internal->play_values_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_user_joined_callback(struct netemu_p2p_connection *connection, p2pUserJoinedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.userJoinedFn = callback;
	netemu_p2p_register_callback(connection->_internal->join_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_game_created_callback(struct netemu_p2p_connection *connection, p2pGameCreatedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.gameCreatedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_created_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_game_started_callback(struct netemu_p2p_connection *connection, p2pGameStartedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.gameStartedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_started_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_player_joined_callback(struct netemu_p2p_connection *connection, p2pPlayerJoinedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.playerJoinedFn = callback;
	netemu_p2p_register_callback(connection->_internal->player_joined_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_player_ready_callback(struct netemu_p2p_connection *connection, p2pPlayerReadyFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.playerReadyFn = callback;
	netemu_p2p_register_callback(connection->_internal->player_ready_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p_connection *connection, p2pAllReadyFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.gameStartedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_started_callbacks, fn, 0, user_data);
	return 0;
}
