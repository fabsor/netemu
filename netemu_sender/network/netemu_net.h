/**
 * @file
 * This header file contains shared structs across different network resources in netemu.
 */

#ifndef NETEMU_NET_H_
#define NETEMU_NET_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netemu_socket.h"
#include "netemu_sender_collection.h"
#include "netemu_sender_udp.h"

union netemu_connection_type{
	struct netemu_sender_udp *udp_sender;
	struct netemu_tcp_connection *connection;
	struct netemu_sender_collection *collection;
};

typedef enum {
	UDP_CONNECTION,
	TCP_CONNECTION,
	CONNECTION_COLLECTION
} netemu_connection_types;

typedef int (*parseReceivedDataFn)(NETEMU_SOCKET, netemu_connection_types, union netemu_connection_type, void*);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_NET_H_ */
