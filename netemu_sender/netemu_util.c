/*
 * test_util.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include "netemu_util.h"
/**
 * @file
 * This file contains some utilities used often in tests of the framework.
 */

//#define SERVER_ADDRESS	netemu_inet_addr("192.168.106.222");
#define SERVER_ADDRESS	netemu_htonl(INADDR_LOOPBACK);

struct netemu_receiver* netemu_util_prepare_receiver(int port,void (* listenerFn)(char*, size_t, struct netemu_receiver*, void*), void* args) {
	struct netemu_sockaddr_in addr_in;
	netemu_sockaddr *addr;
	struct netemu_receiver *receiver;
	addr_in.addr = netemu_htonl(INADDR_ANY);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = netemu_htons(port);

	addr = netemu_prepare_net_addr(&addr_in);
	receiver = netemu_receiver_new(addr,sizeof(addr_in), 256);
	netemu_receiver_register_recv_fn(receiver, listenerFn, args);
	netemu_receiver_start_listening(receiver);
	return receiver;
}

struct netemu_sender* netemu_util_prepare_sender(int port) {
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	addr_in.addr = SERVER_ADDRESS;
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = netemu_htons(port);

	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new(addr,sizeof(addr_in));
	return sender;
}

struct netemu_sender* netemu_util_prepare_sender_on_socket(NETEMU_SOCKET socket, int port) {
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	addr_in.addr = SERVER_ADDRESS;
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = netemu_htons(port);
	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new_on_socket(addr,socket,sizeof(addr_in));
	return sender;
}

struct netemu_sender* netemu_util_prepare_sender_on_socket_at_addr(NETEMU_SOCKET socket, struct netemu_sockaddr_in *addr_in, int port) {
	netemu_sockaddr *addr;
	struct netemu_sender* sender;
	addr = netemu_prepare_net_addr(addr_in);
	sender = netemu_sender_new_on_socket(addr,socket,sizeof(*addr_in));
	return sender;
}

void netemu_util_send_data(struct netemu_sender* sender, char* data) {
	int error;
	error = netemu_sender_send(sender,data,strlen(data)+1);
	if(error < 0) {
		printf("Error: %d\n", netemu_get_last_error());
	}
}
