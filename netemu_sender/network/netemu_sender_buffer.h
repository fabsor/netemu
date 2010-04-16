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

#define LIST_START_SIZE	10

struct netemu_sender_buffer {
	struct netemu_list* instructions;
	char preferred_no_packets;
	char preferred_delay_time;
	char send;
	char running;
	netemu_mutex send_lock;
};

struct netemu_sender_buffer* netemu_sender_buffer_new(const short preferred_no_packets,
		const short preferred_delay_time);

void netemu_sender_buffer_add(struct netemu_sender_buffer* buffer,
		struct application_instruction* instruction);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_SENDER_BUFFER_H_ */
