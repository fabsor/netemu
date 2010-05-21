/**
 * @file
 * This file contains the interface for the netemu_receiver_udp module.
 */

/**
 * @defgroup netemu_receiver_udp Netemu Receiver UDP
 * The netemu Receiver UDP module can be used to receive
 * packets at a designated port.
 */

#ifndef NETEMU_RECEIVER_H_
#define NETEMU_RECEIVER_H_
#include "netemu_socket.h"
#include "netemu_list.h"
#include "netemu_thread.h"
#include "net.h"


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
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
struct netemu_receiver_udp* netemu_receiver_udp_create(netemu_sockaddr* addr, int addr_len);

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_udp_start_receiving(struct netemu_receiver_udp* receiver, parseReceivedDataFn fn, void* param);

void netemu_receiver_udp_stop_receiving(struct netemu_receiver_udp *receiver);

/**
 * Free the memory that this receiver is using.
 */
void netemu_receiver_udp_destroy(struct netemu_receiver_udp* receiver);

#endif /* NETEMU_RECEIVER_H_ */
