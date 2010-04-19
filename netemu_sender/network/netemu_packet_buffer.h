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
	#include "../protocol/application.h"
	#include "netemu_thread.h"


	#define PREFERRED_NO_PACKETS	5
	#define PREFERRED_SEND_DELAY	10

	typedef struct _netemu_packet_buffer_internal* netemu_packet_buffer_internal;

	struct netemu_packet_buffer {
		NETEMU_HASHTBL *table;
		netemu_packet_buffer_internal _internal;
	};
	typedef void (*bufferListenerFn)(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);

	void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction);
	struct netemu_list* netemu_packet_buffer_get(struct netemu_packet_buffer *buffer, char id);
	struct application_instruction* netemu_packet_buffer_peek(struct netemu_packet_buffer *buffer, char id);
	struct application_instruction* netemu_packet_buffer_pop(struct netemu_packet_buffer *buffer, char id);
	void netemu_packet_buffer_clear(struct netemu_packet_buffer *buffer);
	struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size);
	void netemu_packet_buffer_register_wakeup_on_instruction(struct netemu_packet_buffer *buffer, char instruction_id, time_t age, netemu_event eventhandle);
	void netemu_packet_buffer_add_instruction_received_fn(struct netemu_packet_buffer *buffer, int instruction, bufferListenerFn fn, void* arg);
#ifdef	__cplusplus
}
#endif

#endif
