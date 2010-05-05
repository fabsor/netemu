/**
 * This module
 */

#ifndef NETEMU_SENDER_H_
#define NETEMU_SENDER_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netemu_sender_udp.h"
#include "netemu_tcp.h"
#include "netemu_list.h"

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


#endif /* NETEMU_SENDER_H_ */
