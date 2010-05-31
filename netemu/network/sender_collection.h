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

struct netemu_sender_collection* netemu_sender_collection_create();

void netemu_sender_collection_add_udp_sender(struct netemu_sender_collection *collection, struct netemu_sender_udp *connection);

void netemu_sender_collection_add_tcp_sender(struct netemu_sender_collection *collection, struct netemu_tcp_connection* connection);

void netemu_sender_collection_remove_sender(struct netemu_sender_collection *collection, int index);

void netemu_sender_collection_destroy(struct netemu_sender_collection *collection, NETEMU_BOOL destroy_connections);

void netemu_sender_collection_send_data(struct netemu_sender_collection* collection, char* data, size_t size);


void netemu_sender_collection_clear(struct netemu_sender_collection *collection, NETEMU_BOOL destroy_connections);
/**
 * Disconnect a sender collection item.
 *
 * @ingroup netemu_sender_collection
 * @param item the item to be destroyed.
 * @param destroy_item Set this to true if you want to destroy the connection this item holds.
 */
void netemu_sender_collection_item_destroy(struct netemu_sender_collection_item *item, NETEMU_BOOL destroy_item);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_SENDER_COLLECTION_H_ */
