/*
 * test_util.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include "netemu_util.h"
#include "netlib_error.h"
#include <stdio.h>
/**
 * @file
 * This file contains some utilities used often in tests of the framework.
 */

/* #define SERVER_ADDRESS	netemu_inet_addr("192.168.106.222"); */
#define SERVER_ADDRESS	netemu_htonl(INADDR_LOOPBACK);

struct netemu_receiver_udp* netemu_util_prepare_receiver(int port,void (* listenerFn)(char*, size_t, struct netemu_receiver_udp*, void*), void* args) {
	netemu_sockaddr_in *addr_in;
	struct netemu_receiver_udp *receiver;

	if((addr_in = malloc(sizeof(netemu_sockaddr_in))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	addr_in->sin_addr.s_addr = netemu_htonl(INADDR_ANY);
	addr_in->sin_family = NETEMU_AF_INET;
	addr_in->sin_port = netemu_htons(port);

	receiver = netemu_receiver_udp_new((netemu_sockaddr*)addr_in,sizeof(netemu_sockaddr_in), 256);
	if(receiver == NULL) {
		free(addr_in);
		return NULL;
	}
	netemu_receiver_udp_register_recv_fn(receiver, listenerFn, args);
	netemu_receiver_udp_start_receiving(receiver);
	return receiver;
}

struct netemu_sender_udp* netemu_util_prepare_sender(int port) {
	netemu_sockaddr_in *addr_in;
	struct netemu_sender_udp* sender;
	addr_in = malloc(sizeof(netemu_sockaddr_in));
	addr_in->sin_addr.s_addr = SERVER_ADDRESS;
	addr_in->sin_family = NETEMU_AF_INET;
	addr_in->sin_port = netemu_htons(port);
	sender = netemu_sender_udp_new((netemu_sockaddr*)addr_in,sizeof(addr_in));
	return sender;
}

struct netemu_sender_udp* netemu_util_prepare_sender_on_socket(NETEMU_SOCKET socket, int port) {
	netemu_sockaddr_in* addr_in;
	struct netemu_sender_udp* sender;
	addr_in = malloc(sizeof(netemu_sockaddr_in));
	addr_in->sin_addr.s_addr = SERVER_ADDRESS;
	addr_in->sin_family = NETEMU_AF_INET;
	addr_in->sin_port = netemu_htons(port);
	sender = netemu_sender_udp_new_on_socket((netemu_sockaddr*)addr_in,socket,sizeof(addr_in));
	return sender;
}

struct netemu_sender_udp* netemu_util_prepare_sender_on_socket_at_addr(NETEMU_SOCKET socket, netemu_sockaddr_in *addr_in, int size) {
	struct netemu_sender_udp* sender;
	sender = netemu_sender_udp_new_on_socket(netemu_util_copy_addr((struct sockaddr*)addr_in,size),socket,size);
	return sender;
}

void netemu_util_send_data(struct netemu_sender_udp* sender, char* data) {
	int error;
	error = netemu_sender_udp_send(sender,data,strlen(data)+1);
	if(error < 0) {
		//printf("Send Error: %d\n", netemu_get_last_error());
	}
}

int netemu_util_copy_string(char** dest, char* src) {
	int size;
	size = sizeof(char)*strlen(src) + 1;
	if((*dest = malloc(size)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	strncpy(*dest, src, size);
	return size;
}

char* netemu_util_parse_string(char* data) {
	int str_len;
	char* string;

	str_len = strlen(data)+1;
	if((string = malloc(str_len)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	strncpy(string, data, str_len);

	return string;
}


netemu_sockaddr* netemu_util_copy_addr(netemu_sockaddr *addr, int addr_size) {
	netemu_sockaddr* copy;
	copy = malloc(addr_size);
	memcpy(copy,addr,addr_size);
	return copy;
}

int netemu_util_pack_str(char* buffer, char* str) {
	int size;
	size = sizeof(char)*(strlen(str)+1);
	memcpy(buffer,(void*)str,size);
	return size;
}
