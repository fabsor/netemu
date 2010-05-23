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
 * This file contains some convenient utility function used in the
 * framework.
 */

/**
 * @defgroup netemu_util Various utilities
 * Some nice utility functions used in the API.
 */
#include "util.h"
#include "netlib_error.h"
#include <stdio.h>

struct netemu_sender_udp* netemu_util_prepare_sender_on_socket_at_addr(NETEMU_SOCKET socket, netemu_sockaddr_in *addr_in, int size) {
	struct netemu_sender_udp* sender;
	sender = netemu_sender_udp_new_on_socket(netemu_util_copy_addr((struct sockaddr*)addr_in,size),socket,size);
	return sender;
}

/**
 * Send a string.
 */
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

netemu_sockaddr_in* netemu_util_create_addr(NETEMU_DWORD address, unsigned short port, int *size) {
	netemu_sockaddr_in *addr;
	addr = malloc(sizeof(netemu_sockaddr_in));
	addr->sin_addr.s_addr = address;
	addr->sin_port = port;
	addr->sin_family = NETEMU_AF_INET;
	*size = sizeof(netemu_sockaddr_in);
	return addr;
}

/**
 * This function allocates memory, and if it fails it kills the application.
 * Handle with care!
 * @param size the size of the block to allocate.
 */
void* netemu_util_alloc_or_die(size_t size) {
	void *pointer;
	pointer = malloc(size);
	if(pointer == NULL) {
		exit(-1);
	}
	return pointer;
}

int netemu_util_pack_str(char* buffer, char* str) {
	int size;
	size = sizeof(char)*(strlen(str)+1);
	memcpy(buffer,(void*)str,size);
	return size;
}
