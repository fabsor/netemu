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

int _netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable, void *user_data);

/**
 * Register a callback in the right place. This is a helper function.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param list The list to add the callback to.
 * @param fn the callback function
 * @param disposable the if the function is disposable, meaning that it will be removed after it has
 * been called.
 * @param user_data arbitrary data the user provided.
 * @return 0 if everything went fine, -1 if a memory error occured.
 */
int _netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable, void *user_data) {
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

/**
 * Register a callback to be called when play values have been recieved.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_play_values_received_callback(struct netemu_p2p *connection, p2pValuesReceivedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.valuesFn = callback;

	_netemu_p2p_register_callback(connection->_internal->play_values_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when a user has joined the cloud.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_user_joined_callback(struct netemu_p2p *connection, p2pUserJoinedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.userJoinedFn = callback;
	_netemu_p2p_register_callback(connection->_internal->join_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when a game has been created.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_game_created_callback(struct netemu_p2p *connection, p2pGameCreatedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.gameCreatedFn = callback;
	_netemu_p2p_register_callback(connection->_internal->game_created_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when a game has started.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_game_started_callback(struct netemu_p2p *connection, p2pGameStartedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.gameStartedFn = callback;
	_netemu_p2p_register_callback(connection->_internal->game_started_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when a player has joined the game you're in.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_player_joined_callback(struct netemu_p2p *connection, p2pPlayerJoinedFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.playerJoinedFn = callback;
	_netemu_p2p_register_callback(connection->_internal->player_joined_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when a player is ready.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_player_ready_callback(struct netemu_p2p *connection, p2pPlayerReadyFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.playerReadyFn = callback;
	_netemu_p2p_register_callback(connection->_internal->player_ready_callbacks, fn, 0, user_data);
	return 0;
}

/**
 * Register a callback to be called when all players are ready to play.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_callbacks
 * @param connection an instance of the netemu_p2p module.
 * @param callback the callback to register.
 * @param user_data data to send to the function after it is called. Set this to NULL if you're not interested.
 * @return 0 if everything went well, -1 otherwise.
 */
int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p *connection, p2pAllReadyFn callback, void *user_data) {
	union p2p_callback_fn fn;
	fn.allReadyFn = callback;
	_netemu_p2p_register_callback(connection->_internal->all_ready_callbacks, fn, 0, user_data);
	return 0;
}
