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
#pragma once
#ifndef NETEMU_SENDER_COLLECTION_H_
#define NETEMU_SENDER_COLLECTION_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "sender_udp.h"
#include "../structures/netemu_list.h"

typedef enum {
	SENDER_UDP,
	SENDER_TCP
} sender_type;

struct netemu_sender_collection_item {
	union netemu_sender_type *sender;
	sender_type type;
};

struct netemu_sender_collection {
	struct netemu_list *senders;
	struct netemu_list *sender_listeners;
};

union netemu_sender_type {
	struct netemu_sender_udp *udp_sender;
	struct netemu_tcp_connection *tcp_sender;
};

struct netemu_sender_collection* netemu_sender_collection_new();

void netemu_sender_collection_add_udp_sender(struct netemu_sender_collection *collection, struct netemu_sender_udp *connection);

void netemu_sender_collection_add_tcp_sender(struct netemu_sender_collection *collection, struct netemu_tcp_connection* connection);

void netemu_sender_collection_remove_sender(struct netemu_sender_collection *collection, int index);

void netemu_sender_collection_free_collection(struct netemu_sender_collection *collection);

void netemu_sender_collection_send_data(struct netemu_sender_collection* collection, char* data, size_t size);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_SENDER_COLLECTION_H_ */
