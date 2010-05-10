/*
 * netemu_receiver.h
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

#ifndef NETEMU_RECEIVER_H_
#define NETEMU_RECEIVER_H_
#include "netemu_socket.h"
#include "netemu_thread.h"
#include "netemu_list.h"
#include "netemu_packet_buffer.h"
#include "netemu_net.h"
#include "netlib_util.h"
#define NETEMU_RECEIVER_DEFAULT_BUFFER_SIZE	128

/*! This struct describes a receiver. */
struct netemu_receiver_udp{
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	netemu_mutex lock;
	struct netemu_packet_buffer *buffer;
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
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
struct netemu_receiver_udp* netemu_receiver_udp_new(netemu_sockaddr* addr, int addr_len);

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, parseReceivedDataFn fn, void* param);

void netemu_receiver_udp_stop_receiving(struct netemu_receiver_udp *receiver);

/**
 * Register a function that will act as a listener. The function will be called when data is received.
 * The function must be thread safe.
 */

void netemu_receiver_udp_free(struct netemu_receiver_udp* receiver);

#endif /* NETEMU_RECEIVER_H_ */
