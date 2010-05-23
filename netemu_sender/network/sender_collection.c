/*
 * netemu_sender.c
 *
 *  Created on: 28 apr 2010
 *      Author: fabian
 */
#include "sender_collection.h"

struct netemu_sender_collection* netemu_sender_collection_new() {
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
	int i;
	struct netemu_sender_collection_item *sender;
	for(i = 0; i < collection->senders->count; i++) {
		sender = (struct netemu_sender_collection_item*)collection->senders->elements[i];
		switch(sender->type) {
			case SENDER_TCP:
				netemu_tcp_connection_send(sender->sender->tcp_sender,data,size);
				break;
			case SENDER_UDP:
				netemu_sender_udp_send(sender->sender->udp_sender,data,size);
				break;
		}
	}
}

void netemu_sender_collection_free_collection(struct netemu_sender_collection *collection) {
	netemu_list_destroy(collection->senders);
	free(collection);
}