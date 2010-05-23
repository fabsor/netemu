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

#ifndef TEST_UTIL_H_
#define TEST_UTIL_H_

#include "network/receiver_udp.h"
#include "network/sender_udp.h"
#include "netemu_socket.h"

#define INTERNAL_PORT	27999
#define SERVER_PORT		27888
#define CLIENT_PORT 	35888
int netemu_util_copy_string(char** dest, char* src);
char* netemu_util_parse_string(char* data);
struct netemu_receiver_udp* netemu_util_prepare_receiver(int port,void (* listenerFn)(char*, size_t, struct netemu_receiver_udp*, void*), void* args);
struct netemu_sender_udp* netemu_util_prepare_sender(int port);
struct netemu_sender_udp* netemu_util_prepare_sender_on_socket(NETEMU_SOCKET socket, int port);
struct netemu_sender_udp* netemu_util_prepare_sender_on_socket_at_addr(NETEMU_SOCKET socket, netemu_sockaddr_in *addr_in, int size);
netemu_sockaddr* netemu_util_copy_addr(netemu_sockaddr *addr, int addr_size);
netemu_sockaddr_in* netemu_util_create_addr(NETEMU_DWORD addr, unsigned short port, int *size);
void netemu_util_send_data(struct netemu_sender_udp* sender, char* data);
void* netemu_util_alloc_or_die(size_t size);
int netemu_util_pack_str(char* buffer, char* str);
#endif /* TEST_UTIL_H_ */
