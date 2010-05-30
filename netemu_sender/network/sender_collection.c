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

#include "sender_collection.h"

struct netemu_sender_collection* netemu_sender_collection_create() {
	struct netemu_sender_collection *collection;
	collection = malloc(sizeof(struct netemu_collection*));
	collection->senders = netemu_list_create(10,1);
	return collection;
}

void netemu_sender_collection_add_udp_sender(struct netemu_sender_collection *collection, struct netemu_sender_udp *connection) {
	struct netemu_sender_collection_item *sender;
	sender = malloc(sizeof(struct netemu_sender_collection_item));
	sender->sender = malloc(sizeof(union netemu_sender_type));
	sender->sender->udp_sender = connection;
	sender->type = SENDER_UDP;
	netemu_list_add(collection->senders, sender);
}

void netemu_sender_collection_add_tcp_sender(struct netemu_sender_collection *collection, struct netemu_tcp_connection* connection) {
	struct netemu_sender_collection_item *sender;
	sender = malloc(sizeof(struct netemu_sender_collection_item));
	sender->sender = malloc(sizeof(union netemu_sender_type));
	sender->sender->tcp_sender = connection;
	sender->type = SENDER_TCP;
	netemu_list_add(collection->senders,sender);

}

void netemu_sender_collection_remove_sender(struct netemu_sender_collection *collection, int index) {
	netemu_list_remove_at(collection->senders,index);
}

void netemu_sender_collection_send_data(struct netemu_sender_collection* collection, char* data, size_t size) {
	struct netemu_sender_collection_item *sender;
	struct netemu_list_iterator *iterator;
	iterator = netemu_list_iterator_create(collection->senders);
	while((sender = netemu_list_iterator_next(iterator)) != NULL) {
		switch(sender->type) {
			case SENDER_TCP:
				netemu_tcp_connection_send(sender->sender->tcp_sender,data,size);
				break;
			case SENDER_UDP:
				netemu_sender_udp_send(sender->sender->udp_sender,data,size);
				break;
		}
	}
	netemu_list_iterator_destroy(iterator);
}
/**
 * Clear out the collection.
 */
void netemu_sender_collection_clear(struct netemu_sender_collection *collection, NETEMU_BOOL destroy_connections) {
	int i;
	for(i = 0; i < collection->senders->count; i++) {
		netemu_sender_collection_item_destroy(collection->senders->elements[i], destroy_connections);
	}
	netemu_list_clear(collection->senders);
}

/**
 * Disconnect a sender collection item.
 *
 * @ingroup netemu_sender_collection
 * @param item the item to be destroyed.
 * @param destroy_connection Set this to true if you want to destroy the connection this item holds.
 */
void netemu_sender_collection_item_destroy(struct netemu_sender_collection_item *item, NETEMU_BOOL destroy_connection) {
	struct netemu_tcp_connection *connection;
	struct netemu_sender_udp *udp;
	if(destroy_connection) {
		if(item->type == SENDER_TCP) {
			connection = item->sender->tcp_sender;
			netemu_tcp_connection_destroy(connection);
		}
		else if(item->type == SENDER_UDP){
			udp = item->sender->udp_sender;
			netemu_receiver_udp_destroy(udp);
		}
	}
	free(item);
}

void netemu_sender_collection_destroy(struct netemu_sender_collection *collection, NETEMU_BOOL destroy_connections) {
	netemu_sender_collection_clear(collection, destroy_connections);
	netemu_list_destroy(collection->senders);
	free(collection);
}
