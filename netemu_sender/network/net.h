/**
 * @file
 * This header file contains shared structs across different network resources in netemu.
 */

/**
 * @defgroup netemu_net General network tools
 * General network stuff.
 */
#ifndef NETEMU_NET_H_
#define NETEMU_NET_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "netemu_socket.h"
#include "sender_collection.h"

/**
 * This union contains all the different types
 * of connectivity modules.
 * @ingroup netemu_net
 */
union netemu_connection_type{
	struct netemu_sender_udp *udp_sender;
	struct netemu_receiver_udp *udp_receiver;
	struct netemu_tcp_connection *connection;
	struct netemu_sender_collection *collection;
};
/**
 * This enum represents each type in
 * the netemu_connection_type union
 * @see netemu_connection_type
 * @ingroup netemu_net
 */
typedef enum {
	UDP_SENDER,
	UDP_RECEIVER,
	TCP_CONNECTION,
	CONNECTION_COLLECTION
} netemu_connection_types;

/**
 * This function pointer describes a function capable of parsing data coming from an UDP or TCP socket.
 * @ingroup netemu_net
 * @param socket the socket from which the parsing should be done.
 * @param type The type of socket
 * @param connection union containing the actual connection module instance
 * @param arg a user defined argument.
 */
typedef int (*parseReceivedDataFn)(NETEMU_SOCKET, netemu_connection_types, union netemu_connection_type, void*);

#ifdef	__cplusplus
}
#endif


#endif /* NETEMU_NET_H_ */
