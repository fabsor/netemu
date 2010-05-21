/*
 * callbacks.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */
#include "../interface/netemu_kaillera.h"
#include "kaillera_internal.h"
#include "netemu_list.h"
#include "netlib_error.h"

int netemu_kaillera_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable, void *user_data);

int netemu_register_chat_callback(struct netemu_kaillera *connection, chatFn callback, void *user_data) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	netemu_kaillera_register_callback(connection->_internal->chat_callback, fn, 0, user_data);
	return 0;
}

int netemu_unregister_chat_callback(struct netemu_kaillera *connection, chatFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	return netemu_list_remove(connection->_internal->chat_callback, fn);
}

int netemu_register_user_join_callback(struct netemu_kaillera *connection, joinFn callback, void *user_data) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;

	return netemu_kaillera_register_callback(connection->_internal->join_callback, fn, 0, user_data);
}

int netemu_register_game_status_updated_callback(struct netemu_kaillera *connection, gameStatusUpdatedFn callback, void *user_data) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->status_update_fn = callback;
	netemu_kaillera_register_callback(connection->_internal->game_status_updated_callbacks, fn, 0, user_data);
	return 0;
}

int netemu_kaillera_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable, void *user_data) {
	struct callback *callback;
	int error;
	if((callback = malloc(sizeof(struct callback))) == NULL ) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	callback->fn = fn;
	callback->disposable = disposable;
	callback->user_data = user_data;
	error = netemu_list_add(list,callback);
	if(error != 0) {
		free(callback);
		return -1;
	}
	return 0;
}

int _server_connection_unregister_callback(struct netemu_list *list, union callback_fn *fn) {
	struct callback *callback;

	if((callback = malloc(sizeof(struct callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->disposable = 0;
	return netemu_list_remove(list,callback);

}

int netemu_register_play_values_received_callback(struct netemu_kaillera *connection, valuesReceivedFn callback, void *user_data) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}

	fn->values_received_fn = callback;

	return netemu_kaillera_register_callback(connection->_internal->play_values_callback, fn, 0, user_data);
}

int netemu_register_player_ready_callback(struct netemu_kaillera *connection, playerReadyFn callback, void *user_data) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}

	fn->player_ready_fn = callback;
	netemu_kaillera_register_callback(connection->_internal->player_ready_callback, fn, 0, user_data);
	return 0;
}

int netemu_register_player_join_callback(struct netemu_kaillera *connection, playerJoinFn callback, void *user_data) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}
	fn->player_join_fn = callback;
	netemu_kaillera_register_callback(connection->_internal->player_join_callback, fn, 0, user_data);
	return 0;
}


int netemu_unregister_play_values_callback(struct netemu_kaillera *connection, valuesReceivedFn callback) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	fn->values_received_fn = callback;
	netemu_list_remove(connection->_internal->play_values_callback, (void*)fn);
	free(fn);
	return 0;
}

int netemu_register_cached_values_received_callback(struct netemu_kaillera *connection, cachedValuesReceivedFn callback, void *user_data) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	fn->cached_values_received_fn = callback;

	return netemu_kaillera_register_callback(connection->_internal->cached_values_callback, fn, 0, user_data);
}

int netemu_unregister_cached_values_callback(struct netemu_kaillera *connection, cachedValuesReceivedFn callback) {
	union callback_fn *fn;
	int error;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	fn->cached_values_received_fn = callback;
	error = netemu_list_remove(connection->_internal->game_created_callback, fn);
	free(fn);

	return error;
}

int netemu_unregister_user_join_callback(struct netemu_kaillera *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	return _server_connection_unregister_callback(connection->_internal->join_callback, fn);
}



int netemu_register_user_leave_callback(struct netemu_kaillera *connection, leaveFn callback, void *user_data) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	netemu_kaillera_register_callback(connection->_internal->leave_callback, fn, 0, user_data);
	return 0;
}


int netmeu_unregister_user_leave_callback(struct netemu_kaillera *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	return netemu_list_remove(connection->_internal->leave_callback, fn);
}

int netemu_register_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn callback, void *user_data) {
	union callback_fn *fn;
	int error;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	fn->game_created_fn = callback;

	error = netemu_kaillera_register_callback(connection->_internal->game_created_callback, fn, 0, user_data);
	if(error != 0) {
		free(fn);
		return -1;
	}

	return 0;
}

int netemu_unregister_game_created_callback(struct netemu_kaillera *connection, kailleraGameCreatedFn callback) {
	union callback_fn *fn;
	int error;
	fn = malloc(sizeof(union callback_fn));
	if(fn == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	fn->game_created_fn = callback;
	error = netemu_list_remove(connection->_internal->game_created_callback, fn);
	free(fn);
	return error;
}

int netemu_register_game_started_callback(struct netemu_kaillera *connection, gameStartedFn callback, void *user_data) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->game_started_fn = callback;
	return netemu_kaillera_register_callback(connection->_internal->game_started_callbacks, fn, 0, user_data);
}
