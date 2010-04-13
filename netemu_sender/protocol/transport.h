/*
 * transport.h
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "netlib_util.h"
#include "application.h"
struct transport_instruction {
	/* ? */
	NETEMU_WORD serial;
	NETEMU_WORD length;
	void* instruction;
};

struct transport_packet {
	char count;
	struct transport_instruction **instructions;
};

struct transport_packet_buffer {
	int size;
	char* data;
};

struct transport_packet_buffer netemu_transport_pack(struct application_instruction **messages, char count);
struct transport_packet* netemu_transport_unpack(char* data);
void netemu_transport_free_packet_buffer(struct transport_packet_buffer* buffer);

#endif /* TRANSPORT_H_ */
