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
 * This file contains functions for handling global resources.
 */

#include <stdlib.h>
#include "network/receiver_udp.h"
#include "resources.h"
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
	if(_netemu_resources_client == NULL)
		return NULL;
	return _netemu_resources_client->sender;
}

void netemu_resources_free_client() {
netemu_receiver_udp_destroy(_netemu_resources_client->receiver);
	netemu_sender_udp_free(_netemu_resources_client->sender);
	free(_netemu_resources_client);
}
