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
 * This file defines the interface of functions concerned
 * with the kaillera communication layer.
 */

/**
 * @defgroup netemu_communication Kaillera Communication layer
 * Functions concerned with the Kaillera communication layer.
 */
#pragma once
#ifndef NETEMU_COMMUNICATION_H_
#define NETEMU_COMMUNICATION_H_

#include "netlib_network.h"
#include "netlib_util.h"

/**
 * This enum contains one constant for each different response you
 * can get from a server on the communication layer.
 * @ingroup netemu_communication
 */
enum {
	CONNECTION_ACCEPTED,
	CONNECTION_REJECTED_VER,
	CONNECTION_REJECTED_TOO,
	PING_RECEIVED
};

/**
 * This struct describes an existing game that is
 * sent from the kaillera master server.
 * @ingroup netemu_communication
 */
struct existing_game {
	char *gamename;
	char *address;
	char *player;
	char *clientname;
	int num_players;
	char *servername;
	char *location;
};
/**
 * This struct describes a server, that is sent
 * from the kaillera master server.
 * @ingroup netemu_communication
 */
struct server {
	char *name;
	char *address;
	char *players;
	int games;
	NETEMU_DWORD ping;
	char *version;
	char *location;
	struct _server_internal* _internal;
};

/**
 * Ping a server. This is not a blocking call.
 * @ingroup netemu_communication
 * @param server the server to ping.
 * @param pingReceivedCallback the callback which should be called after the server has been pinged.
 * @return 0 if everything went fine, -1 on failure.
 * @todo This function does not work yet. We need a clever way to ping servers.
 */
int netemu_communication_ping_server(struct server *server, void (* pingReceivedCallback)(struct server *server));

/**
 * Create an hello message that can be sent to the server.
 * @ingroup netemu_communication
 * @param int version the version of the protocol you want to tell the server that you have.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_hello_message(char* version);

/**
 * Create a ping message can be sent to the server.
 * @ingroup netemu_communication
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_ping_message();

/**
 * Parse incoming messages from the server.
 * @ingroup netemu_communication
 * @return an int describing the server response. it could be one of:
 * CONNECTION_ACCEPTED the connection has been accepted. You should use netemu_protocol_parse_server_accept port to get the port
 * CONNECTION_REJECTED_VER the connection was rejected because of version concerns.
 * CONNECTION_REJECTED_TOO the connection was rejected because this server is full.
 * PING_RECEIVED The sent ping was received.
 */
int netemu_communication_parse_server_message(char* server_message);

/**
 * Parse the accept port out of a CONNECTION_ACCEPTED message.
 * @ingroup netemu_communication
 * @return the port number or -1 if the parsing failed.
 */
int netemu_communication_parse_server_accept_port(char* server_message);

/**
 * Create a http get request.
 * @ingroup netemu_communication
 * @param host the host
 * @param path the path
 * @return the http get request.
 */
char* netemu_communication_http_get(char* host, char* path);

/**
 * Parse an http response
 * @ingroup netemu_communication
 * @param socket the socket from which the request should be received.
 * @param games the active games. Just send in a pointer here, and it will be filled automaticly.
 * @param gamecount the number of games. It will be filled by the function.
 * @param servers The actual server. The pointer will be directed to a block containing the server.
 * @param servercount the number of servers. It will be filled with the appropriate number.
 * @return 0 if everything went OK, -1 on failure.
 */
int netemu_communication_parse_http(NETLIB_SOCKET socket, struct existing_game ***games, int *gamecount, struct server ***servers, int *servercount);

#endif /* NETEMU_COMMUNICATION_H_ */
