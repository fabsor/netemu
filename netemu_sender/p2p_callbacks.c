/**
 * @file
 * This file contains functions handlig the p2p callbacks.
 */

#include "interface/netemu_p2p.h"
#include "netemu_info.h"

int netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable);

int netemu_p2p_register_callback(struct netemu_list *list, union p2p_callback_fn fn, int disposable) {
	struct p2p_callback *callback;

	if((callback = malloc(sizeof(struct p2p_callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->disposable = disposable;
	netemu_list_add(list,callback);
	return 0;
}

int netemu_p2p_register_play_values_received_callback(struct netemu_p2p_connection *connection, p2pValuesReceivedFn callback) {
	union p2p_callback_fn fn;
	fn.valuesFn = callback;
	netemu_p2p_register_callback(connection->_internal->play_values_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_user_joined_callback(struct netemu_p2p_connection *connection, p2pUserJoinedFn callback) {
	union p2p_callback_fn fn;
	fn.userJoinedFn = callback;
	netemu_p2p_register_callback(connection->_internal->join_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_game_created_callback(struct netemu_p2p_connection *connection, p2pGameCreatedFn callback) {
	union p2p_callback_fn fn;
	fn.gameCreatedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_created_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_game_started_callback(struct netemu_p2p_connection *connection, p2pGameStartedFn callback) {
	union p2p_callback_fn fn;
	fn.gameStartedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_started_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_player_joined_callback(struct netemu_p2p_connection *connection, p2pPlayerJoinedFn callback) {
	union p2p_callback_fn fn;
	fn.playerJoinedFn = callback;
	netemu_p2p_register_callback(connection->_internal->player_joined_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_player_ready_callback(struct netemu_p2p_connection *connection, p2pPlayerReadyFn callback) {
	union p2p_callback_fn fn;
	fn.playerReadyFn = callback;
	netemu_p2p_register_callback(connection->_internal->player_ready_callbacks, fn, 0);
	return 0;
}

int netemu_p2p_register_all_players_ready_callback(struct netemu_p2p_connection *connection, p2pAllReadyFn callback) {
	union p2p_callback_fn fn;
	fn.gameStartedFn = callback;
	netemu_p2p_register_callback(connection->_internal->game_started_callbacks, fn, 0);
	return 0;
}
