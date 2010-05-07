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


