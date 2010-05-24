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
 * This file defines global resources used by the whole library.
 */
#pragma once
#ifndef NETEMU_RESOURCES_H_
#define NETEMU_RESOURCES_H_

/* Common defined values. */

#define CLIENT_PORT 	35888


/**
 * This struct contains resources commonly used by all parts of
 * the program.
 */
struct netemu_client {
	struct netemu_receiver_udp *receiver;
	struct netemu_sender_udp *sender;
	struct netemu_tcp_connection *host;
	struct netemu_tcp_connection **clients;
};


/**
 * Get the client, containing a sender and receiver object.
 * @return the netemu_client that's currently in use.
 */
struct netemu_client* netemu_resources_get_client();

struct netemu_receiver_udp* netemu_resources_get_receiver();

struct netemu_sender_udp* netemu_resources_get_sender();

struct netemu_tcp_connection* netemu_resources_get_host();

struct netemu_tcp_connection** netemu_resources_get_clients();

/**
 * Free the client memory. This will free the memory of the
 * sender and receiver contained in the struct as well.
 */
void netemu_resources_free_client();

#endif /* NETEMU_RESOURCES_H_ */
