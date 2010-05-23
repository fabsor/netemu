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

/**
 * @file
 * This file contains some structs that are being used by both the transport and application files.
 * It is here to avoid cross references.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifdef	__cplusplus
extern "C" {
#endif

struct transport_instruction {
	/* ? */
	NETEMU_WORD serial;
	NETEMU_WORD length;
	long p2p_id;
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

/*! A message to be sent to the server. */
struct application_instruction {
	char id; /* 1...23 */
	char *user;
	void *body;
	int body_size;
	/* Since we dont know the actual size, this is probably the best option, unfortunately. */
	void (*packBodyFn)(struct application_instruction* instruction, char* buffer);
	void* (*copyBodyFn)(struct application_instruction* instruction);
	void (*freeBodyFn)(struct application_instruction* instruction);
	int important; /**< Set this to > 0 and it will cause the buffer containing it to pack all buffered packets up and send them directly.*/
	time_t timestamp;
};

#ifdef	__cplusplus
}
#endif


#endif /* PROTOCOL_H_ */
