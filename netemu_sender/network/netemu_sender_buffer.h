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

#include "netemu_thread.h"
#include "netemu_list.h"
#include "netemu_sender_udp.h"
#include "netemu_tcp.h"
#include "netemu_sender_collection.h"

#define LIST_START_SIZE	10
#define PACKET_SEND_INTERVAL 30

union netemu_sender_buffer_type{
	struct netemu_sender_udp *udp_sender;
	struct netemu_tcp_connection *connection;
	struct netemu_sender_collection *collection;
};

typedef enum {
	BUFFER_UDP_SENDER,
	BUFFER_TCP_SENDER,
	BUFFER_SENDER_COLLECTION
} netemu_sender_buffer_types;

struct netemu_sender_buffer {
	struct netemu_list* instructions;
	char preferred_no_packets;
	char preferred_delay_time;
	char send;
	char running;
	union netemu_sender_buffer_type *sender;
	netemu_sender_buffer_types type;
	netemu_mutex send_lock;
	netemu_event event;
	time_t last_send;
};

struct netemu_sender_buffer* netemu_sender_buffer_new(netemu_sender_buffer_types type, union netemu_sender_buffer_type *sender, const short preferred_no_packets,
		const short preferred_delay_time);

void netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_SENDER_BUFFER_H_ */
