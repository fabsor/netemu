/*
 * netemu_resources.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabian
 */
#include <stdlib.h>
#include "network/netemu_sender_udp.h"
#include "network/netemu_receiver.h"
#include "netemu_resources.h"
#include "netlib_error.h"

/*! Global variable containing the shared client. */
struct netemu_client* _netemu_resources_client = NULL;

struct netemu_client* netemu_resources_get_client() {
	if (_netemu_resources_client == NULL) {
		if((_netemu_resources_client = malloc(sizeof(struct netemu_client))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
			return NULL;
		}
	}
	return _netemu_resources_client;
}

struct netemu_receiver_udp* netemu_resources_get_receiver() {
	if(_netemu_resources_client == NULL)
		return NULL;
	return _netemu_resources_client->receiver;
}

struct netemu_sender_udp* netemu_resources_get_sender() {
	return _netemu_resources_client->sender;
}

void netemu_resources_free_client() {
netemu_receiver_udp_free(_netemu_resources_client->receiver);
	netemu_sender_udp_free(_netemu_resources_client->sender);
	free(_netemu_resources_client);
}
