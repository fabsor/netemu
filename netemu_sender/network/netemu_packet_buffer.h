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
	
	typedef struct _netemu_packet_buffer_internal* netemu_packet_buffer_internal;

	struct netemu_packet_buffer {
		NETEMU_HASHTBL *table;
		netemu_packet_buffer_internal _internal;
	};

	void netemu_packet_buffer_add(struct netemu_packet_buffer *buffer, struct application_instruction *instruction);
	struct netemu_list* netemu_packet_buffer_get(struct netemu_packet_buffer *buffer, int id);
	struct application_instruction* netemu_packet_buffer_peek(struct netemu_packet_buffer *buffer, int id);
	struct application_instruction* netemu_packet_buffer_pop(struct netemu_packet_buffer *buffer, int id);
	void netemu_packet_buffer_clear(struct netemu_packet_buffer *buffer);
	struct netemu_packet_buffer *netemu_packet_buffer_new(hash_size size);

#ifdef	__cplusplus
}
#endif

#endif