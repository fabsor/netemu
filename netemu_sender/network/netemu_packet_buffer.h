/**
 * @file
 * This header represents a buffer module that can be used for packets.
 */

#ifndef _NETEMU_PACKET_BUFFER_H
#define _NETEMU_PACKET_BUFFER_H

#ifdef	__cplusplus
extern "C" {
#endif
	#include "netemu_hashtbl.h"
	#include "application.h"
	
	typedef netemu_packet_buffer_internal* netemu_packet_buffer;

	struct netemu_packet_buffer_internal {
		NETEMU_HASHTBL table;
	};

	void netemu_packet_buffer_add(netemu_packet_buffer buffer, struct application_instruction *instruction);
	struct netemu_list* netemu_packet_buffer_get(netemu_packet_buffer buffer, int id);
	struct application_instruction* netemu_packet_buffer_peek(netemu_packet_buffer buffer);
	struct application_instruction* netemu_packet_buffer_pop(netemu_packet_buffer buffer);
	struct application_instruction* netemu_packet_buffer_clear();

#ifdef	__cplusplus
}
#endif

#endif