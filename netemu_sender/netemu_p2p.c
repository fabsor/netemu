/*
 * netemu_p2p.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#include "netemu_info.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_sender_collection.h"
#include "interface/netemu_p2p.h"
#include "protocol/application_p2p.h"
#include "network/netemu_tcp.h"
#include "netemu_list.h"


struct netemu_p2p_internal {
	struct netemu_tcp_listener *host;
	struct netemu_tcp_connection *connection;
	struct netemu_sender_collection *peers;
};

void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);

/**
 * Create a host connection.
 */
struct netemu_p2p_connection* netemu_p2p_new(char* username, char* emulatorname) {
	struct netemu_p2p_connection *p2p;
	struct netemu_sender_buffer *buffer;
	union netemu_sender_buffer_type *type;

	type = malloc(sizeof(union netemu_sender_buffer_type));
	type->collection = netemu_sender_collection_new();
	buffer = netemu_sender_buffer_new(BUFFER_SENDER_COLLECTION,type,5,20);
	p2p = malloc(sizeof(struct netemu_p2p_connection));
	p2p->info = netemu_server_connection_new(username,emulatorname,buffer);
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	p2p->_internal->peers = netemu_sender_collection_new();
	return p2p;
}

void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params) {
	struct netemu_p2p_connection *p2p;
	p2p = (struct netemu_p2p_connection*)params;
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers, connection);

}

void netemu_p2p_host(struct netemu_p2p_connection* p2p,struct netemu_sockaddr_in *addr, int addr_size, char* cloudname) {
	p2p->_internal->host = netemu_tcp_listener_new(netemu_prepare_net_addr(addr),addr_size);
	p2p->cloud_name = cloudname;
	netemu_tcp_listener_register_new_connection_fn(p2p->_internal->host,netemu_p2p_new_connection,p2p);
}
/**
 * Connect to a host.
 */
int netemu_p2p_connect(struct netemu_p2p_connection* p2p, struct netemu_sockaddr_in *in_addr, int in_addr_size,  struct netemu_sockaddr_in *connect_addr, int connect_addr_size) {
	int error;
	struct netemu_tcp_connection *connection;
	p2p->_internal->host = netemu_tcp_listener_new(netemu_prepare_net_addr(in_addr),in_addr_size);
	connection = netemu_tcp_connection_new(netemu_prepare_net_addr(connect_addr),connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	netemu_tcp_connection_register_recv_fn(connection, netemu_tcp_connection_receive, p2p->info);
	error = netemu_tcp_connection_connect(connection);
	netemu_p2p_login(p2p);
	return error;
}

void netemu_p2p_login(struct netemu_p2p_connection *p2p) {
	struct application_instruction *message, *success;
	time_t timestamp;
	message = netemu_application_create_message();
	netemu_application_p2p_login_request_add(message,p2p->_internal->host->addr,p2p->_internal->host->addr_len,p2p->info->username,p2p->info->emulator_name, 1);
	message->important = 1;
	timestamp = time(NULL);
	netemu_sender_buffer_add(p2p->info->_internal->send_buffer,message);
	success = netemu_packet_buffer_wait_for_instruction(p2p->info->_internal->receive_buffer, P2P_LOGIN_SUCCESS, timestamp);

}

int netemu_p2p_create_game(struct netemu_info *connection, char *gamename, struct game** result) {
	struct game* game;
	struct application_instruction* instruction;

	instruction = netemu_application_create_message();
	netemu_application_create_game_add(instruction,gamename);
	game = (struct game*)instruction->body;
	game->player_count = 1;
	game->id = 0;
	server_connection_add_player(game, connection->user);
}
