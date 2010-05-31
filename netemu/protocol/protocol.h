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

/**
 * @defgroup netemu_protocol Protocol structures
 * Some structures that both the kaillera and netemu p2p protocols shares in common.
 */
#pragma once
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * This struct represents a transport instruction
 * in the netemu p2p and kaillera protocols.
 * @ingroup netemu_protocol
 */
struct transport_instruction {
	/* ? */
	NETEMU_WORD serial;
	NETEMU_WORD length;
	long p2p_id;
	void* instruction;
};
/**
 * This struct represents a packet in the
 * netemu_p2p and kaillera protocols.
 * @ingroup netemu_protocol
 */
struct transport_packet {
	char count;
	struct transport_instruction **instructions;
};

/**
 * This struct represents data that
 * is ready to be sent over the network.
 * @ingroup netemu_protocol
 */
struct transport_packet_buffer {
	int size; /**< The size of the data block. */
	char* data; /**< The actual data block. */
};

/**
 * This struct represents an application-layer instruction
 * for both the netemu p2p and the kaillera protocol
 * @ingroup netemu_protocol
 */
struct application_instruction {
	char id; /**< The application ID */
	char *user; /**< The user name. This is primarily used by the kaillera protocol. */
	void *body; /**< The instruction body. */
	int body_size; /**< The size of the body. */
	void (*packBodyFn)(struct application_instruction* instruction, char* buffer); /**< This is a function pointer
	that points to a function that can pack the data in this instruction to something that can be sent over the network (serializing) */
	void* (*copyBodyFn)(struct application_instruction* instruction); /**< This is a function pointer that points to a function that can copy the
	instruction body. */
	void (*freeBodyFn)(struct application_instruction* instruction); /**< This is a function pointer that points to a function that can free the memory
	allocated in the function body */
	int important; /**< Set this to > 0 and it will cause the buffer containing it to pack all buffered packets up and send them directly.*/
	time_t timestamp;
};

#ifdef	__cplusplus
}
#endif


#endif /* PROTOCOL_H_ */
