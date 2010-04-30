/*
 * callbacks.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */
#include "interface/netemu_kaillera.h"
#include "interface/netemu.h"
#include "netemu_info.h"
#include "netemu_list.h"

int netemu_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable);

int netemu_register_chat_callback(struct netemu_info *connection, chatFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	netemu_register_callback(connection->_internal->chat_callback, fn, 0);
	return 0;
}

int netemu_unregister_chat_callback(struct netemu_info *connection, chatFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	return netemu_list_remove(connection->_internal->chat_callback, fn);
}

int netemu_register_user_join_callback(struct netemu_info *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;
	netemu_register_callback(connection->_internal->join_callback, fn, 0);
	return 0;
}

int netemu_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable) {
	struct callback *callback;

	if((callback = malloc(sizeof(struct callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->disposable = disposable;
	netemu_list_add(list,callback);
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

int netemu_register_play_values_received_callback(struct netemu_info *connection, valuesReceivedFn callback) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}
	fn->valuesReceivedFn = callback;
	netemu_register_callback(connection->_internal->play_values_callback, fn, 0);
	return 0;
}

int server_connection_unregister_play_values_callback(struct netemu_info *connection, joinFn callback) {
	int error;
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}
	fn->join_fn = callback;
	error = netemu_list_remove(connection->_internal->join_callback, fn);
	free(fn);

	return error;
}


int netemu_unregister_user_join_callback(struct netemu_info *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;
	return _server_connection_unregister_callback(connection->_internal->join_callback, fn);
}

int netemu_register_user_leave_callback(struct netemu_info *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	netemu_register_callback(connection->_internal->leave_callback, fn, 0);
	return 0;
}


int netmeu_unregister_user_leave_callback(struct netemu_info *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	return netemu_list_remove(connection->_internal->leave_callback, fn);
}
