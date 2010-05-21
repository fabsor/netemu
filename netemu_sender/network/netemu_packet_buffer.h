/**
 * @file
 * This header represents a buffer module that can be used for packets.
 */

#ifndef _NETEMU_PACKET_BUFFER_H
#define _NETEMU_PACKET_BUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif
	#include "../structures/netemu_hashtbl.h"
	#include "netemu_thread.h"
	#include "netemu_net.h"

	#define PREFERRED_NO_PACKETS	5
	#define PREFERRED_SEND_DELAY	10

	typedef struct _netemu_packet_buffer_internal* netemu_packet_buffer_internal;

	struct netemu_packet_buffer_item {
		struct application_instruction *instruction;
		union netemu_connection_type connection;
		netemu_connection_types type;
	};


	/* TODO: Swap this against a clean typedef instead. */
	struct netemu_packet_buffer {
		netemu_packet_buffer_internal _internal;
	};

	typedef void (*bufferListenerFn)(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);

	void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, netemu_connection_types type,  union netemu_connection_type connection);
	void netemu_packet_buffer_clear(struct netemu_packet_buffer *buffer);
	void netemu_packet_buffer_add_instruction_received_fn(struct netemu_packet_buffer *buffer, char instruction, bufferListenerFn fn, void* arg);
	struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size);
	struct netemu_packet_buffer_item* netemu_packet_buffer_wait_for_instruction(struct netemu_packet_buffer* buffer, int instruction_id, time_t timestamp);
#ifdef	__cplusplus
}
#endif

#endif
