/*
 * netemu_sender_buffer.h
 *
 *  Created on: 16 apr 2010
 *      Author: fabian
 */

#ifndef NETEMU_SENDER_BUFFER_H_
#define NETEMU_SENDER_BUFFER_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "net.h"
#include "netemu_thread.h"
#include "netemu_list.h"
#include "sender_udp.h"
#include "tcp.h"
#include "sender_collection.h"

#define LIST_START_SIZE	10
#define PACKET_SEND_INTERVAL 30

struct netemu_sender_buffer {
	struct netemu_hashtbl* instructions;
	struct netemu_list* itemsToAdd;
	char preferred_no_packets;
	char preferred_delay_time;
	char send;
	char running;
	union netemu_connection_type *sender;
	netemu_connection_types type;
	netemu_mutex send_lock;
	netemu_event event;
	time_t last_send;
};

struct netemu_sender_buffer* netemu_sender_buffer_new(const short preferred_no_packets,
		const short preferred_delay_time);

int netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction, netemu_connection_types type, union netemu_connection_type recipient);

void netemu_sender_buffer_add_with_blacklist(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction, struct netemu_sender_collection *recipients, netemu_connection_types blacklist_type, union netemu_connection_type blacklist);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_SENDER_BUFFER_H_ */
