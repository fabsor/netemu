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

#define NETEMU_RECEIVER_DEFAULT_BUFFER_SIZE	128

/*! This struct describes a receiver. */
struct netemu_receiver{
	NETEMU_SOCKET socket;
	netemu_sockaddr* addr;
	netemu_mutex lock;
	struct netemu_receiver_fn *receiver_fn;
	int buffer_size;
	int fn_count;
	int error;
};

struct netemu_receiver_fn{
	void (* listenerFn)(char*, size_t, struct netemu_receiver*);
	struct netemu_receiver_fn *next;
};

/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
struct netemu_receiver* netemu_receiver_new(netemu_sockaddr* addr, int buffer_size);

/**
 * This function creates a new thread and starts listening for incoming
 * datagrams on the specified address and port.
 */
void netemu_receiver_start_listening(struct netemu_receiver* receiver);

/**
 * Register a function that will act as a listener. The function will be called when data is received.
 * The function must be thread safe.
 */
void netemu_receiver_register_recv_fn(struct netemu_receiver* receiver, void (* listenerFn)(char*, size_t, struct netemu_receiver*));

#endif /* NETEMU_RECEIVER_H_ */
