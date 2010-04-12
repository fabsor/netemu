/*
 * test_util.h
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */

#ifndef TEST_UTIL_H_
#define TEST_UTIL_H_

#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include "netemu_socket.h"

#define INTERNAL_PORT	27999
#define SERVER_PORT		27888
#define CLIENT_PORT 	35888

struct netemu_receiver* prepare_receiver(int port,void (* listenerFn)(char*, size_t, struct netemu_receiver*, void*));
struct netemu_sender* prepare_sender(int port);
struct netemu_sender* prepare_sender_on_socket(NETEMU_SOCKET socket, int port);
struct netemu_sender* prepare_sender_on_socket_at_addr(NETEMU_SOCKET socket, struct netemu_sockaddr_in *addr, int port);
void send_data(struct netemu_sender* sender, char* data);

#endif /* TEST_UTIL_H_ */
