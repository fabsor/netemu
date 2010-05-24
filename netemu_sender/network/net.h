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
 * This header file contains shared structs across different network resources in netemu.
 */

/**
 * @defgroup netemu_net General network tools
 * General network stuff.
 */
#pragma once
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
