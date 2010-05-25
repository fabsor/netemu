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
 * This file contains the interface for the netemu_receiver_udp module.
 */

/**
 * @defgroup netemu_receiver_udp Netemu Receiver UDP
 * The netemu Receiver UDP module can be used to receive
 * packets at a designated port.
 */
#pragma once
#ifndef NETEMU_RECEIVER_UDP_H_
#define NETEMU_RECEIVER_UDP_H_
#include "netemu_socket.h"
#include "../structures/netemu_list.h"
#include "net.h"
#include "netemu_thread.h"


/*! This struct describes a receiver. */
struct netemu_receiver_udp{
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	netemu_mutex lock;
	parseReceivedDataFn fn;
	NETEMU_BOOL listening;
	void* received_data_param;
	int addr_len;
	int fn_count;
	int error;
};

struct netemu_receiver_udp_fn{
	void (* listenerFn)(char*, size_t, struct netemu_receiver_udp*, void*);
	void* params;
	struct netemu_receiver_udp_fn *next;
};

/**
 * Creates a new instance of the netemu_receiver_udp module
 * @ingroup netemu_receiver_udp
 * @param addr The address to bind to.
 * @param addr_len the size of the address.
 * @return NULL if something went wrong, 0 otherwise. Call netlib_get_last_error to get information
 * about the occurred error. Errors that can occur includes:
 * - NETEMU_ENOTENOUGHMEMORY
 * - NETEMU_INVALID_SOCKET
 * - All errors that can occur with netemu_bind @see netemu_bind
 */
struct netemu_receiver_udp* netemu_receiver_udp_create(netemu_sockaddr* addr, int addr_len);

/**
 * Start receiving data with this instance of netemu_receiver.
 * @param receiver the instance of netemu_receiver_udp
 * @param fn the function which should handle and parse data recieved.
 * @param param any value you want to pass on to the responder function. Set it to NULL if you're not interested.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, parseReceivedDataFn fn, void* param);

/**
 * Stop receiving data on this receiver. This function will block for a short time in order to let the receiving thread finish.
 * @param receiver the instance of netemu_receiver_udp
 */
void netemu_receiver_udp_stop_receiving(struct netemu_receiver_udp *receiver);

/**
 * Free resources used by this receiver and destroy it. You cannot use this
 * receiver after destroying it!!
 * @param receiver the instance of netemu_receiver_udp
 */
void netemu_receiver_udp_destroy(struct netemu_receiver_udp* receiver);

#endif /* NETEMU_RECEIVER_UDP_H_ */
