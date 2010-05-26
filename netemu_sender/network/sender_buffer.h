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
#pragma once
#ifndef NETEMU_SENDER_BUFFER_H_
#define NETEMU_SENDER_BUFFER_H_

#ifdef	__cplusplus
extern "C" {
#endif
#include "net.h"
#include "../structures/netemu_list.h"
#include "sender_udp.h"
#include "tcp.h"
#include "sender_collection.h"
#include "netlib_thread.h"

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
	netlib_mutex send_lock;
	netlib_event event;
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
