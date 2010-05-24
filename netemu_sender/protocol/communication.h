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
#pragma once
#ifndef NETEMU_COMMUNICATION_H_
#define NETEMU_COMMUNICATION_H_

#include "netemu_socket.h"
#include "netlib_util.h"

enum {
	CONNECTION_ACCEPTED,
	CONNECTION_REJECTED_VER,
	CONNECTION_REJECTED_TOO,
	PING_RECEIVED
};

struct existing_game {
	char *gamename;
	char *address;
	char *player;
	char *clientname;
	int num_players;
	char *servername;
	char *location;
};

typedef struct _server_internal* server_internal;

struct server {
	char *name;
	char *address;
	char *players;
	int games;
	NETEMU_DWORD ping;
	char *version;
	char *location;
	server_internal _internal;
};

int netemu_communication_ping_server(struct server *server, void (* pingReceivedCallback)(struct server *server));

/**
 * Create an hello message that can be sent to the server.
 * @param int version the version of the protocol you want to tell the server that you have.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_hello_message(char* version);

/**
 * Create a ping message can be sent to the server.
 * @return an appropriately formatted hello message for the server.
 */
char* netemu_communication_create_ping_message();

/**
 * Parse incoming messages from the server.
 * @return an int describing the server response. it could be one of:
 * CONNECTION_ACCEPTED the connection has been accepted. You should use netemu_protocol_parse_server_accept port to get the port
 * CONNECTION_REJECTED_VER the connection was rejected because of version concerns.
 * CONNECTION_REJECTED_TOO the connection was rejected because this server is full.
 * PING_RECEIVED The sent ping was received.
 */
int netemu_communication_parse_server_message(char* server_message);

/**
 * Parse the accept port out of a CONNECTION_ACCEPTED message.
 * @return the port number or -1 if the parsing failed.
 */
int netemu_communication_parse_server_accept_port(char* server_message);

char* netemu_communication_http_get(char* host, char* path);

int netemu_communication_parse_http(NETEMU_SOCKET socket, struct existing_game ***games, int *gamecount, struct server ***servers, int *servercount);


#endif /* NETEMU_COMMUNICATION_H_ */
