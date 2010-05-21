/*
 * test_util.h
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
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
