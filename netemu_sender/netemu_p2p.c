/*
 * netemu_p2p.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#include "netemu_info.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_packet_buffer.h"
#include "network/netemu_sender_collection.h"
#include "network/netemu_packet_buffer.h"
#include "interface/netemu_p2p.h"
#include "netemu_info.h"
#include "protocol/application_p2p.h"
#include "network/netemu_tcp.h"
#include "netemu_list.h"


struct netemu_p2p_internal {
	struct netemu_tcp_listener *host;
	struct netemu_tcp_connection *connection;
	struct netemu_sender_collection *peers;
};

void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection);
void netemu_p2p_send_login_success(struct netemu_info *info, struct netemu_tcp_connection *recipient);
void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);
void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
/**
 * Create a host connection.
 */
struct netemu_p2p_connection* netemu_p2p_new(char* username, char* emulatorname) {
	struct netemu_p2p_connection *p2p;
	struct netemu_sender_buffer *buffer;
	union netemu_connection_type *type;

	type = malloc(sizeof(union netemu_connection_type));
	type->collection = netemu_sender_collection_new();
	buffer = netemu_sender_buffer_new(5,20);
	p2p = malloc(sizeof(struct netemu_p2p_connection));
	p2p->user = malloc(sizeof(struct p2p_user));
	p2p->user->name = username;
	p2p->user->app_name = emulatorname;
	p2p->info = netemu_server_connection_new(username,emulatorname,buffer);
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_LOGIN_REQUEST, netemu_p2p_respond_to_login_request, p2p->info);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_LOGIN_SUCCESS, netemu_p2p_respond_to_login_success, p2p->info);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_PLAYER_JOIN, netemu_p2p_respond_to_user_join, p2p->info);
	p2p->_internal->peers = type->collection;
	return p2p;
}

void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params) {
	struct netemu_p2p_connection *p2p;
	p2p = (struct netemu_p2p_connection*)params;
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers, connection);
	netemu_tcp_connection_start_receiving(connection);
	netemu_tcp_connection_register_recv_fn(connection,netemu_tcp_connection_receive,p2p->info);
	_netemu_p2p_send_ready(p2p->info->_internal->send_buffer,connection);
}

void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	instruction = netemu_application_create_message();
	netemu_application_p2p_ready_add(instruction);
	type.connection = connection;
	netemu_sender_buffer_add(buffer, instruction,TCP_CONNECTION, type);
}

void netemu_p2p_host(struct netemu_p2p_connection* p2p,struct netemu_sockaddr_in *addr, int addr_size, char* cloudname) {
	p2p->_internal->host = netemu_tcp_listener_new(netemu_prepare_net_addr(addr),addr_size);
	p2p->cloud_name = cloudname;
	netemu_tcp_listener_register_new_connection_fn(p2p->_internal->host,netemu_p2p_new_connection,p2p);
	netemu_tcp_listener_start_listening(p2p->_internal->host);
}
/**
 * Connect to a host.
 */
int netemu_p2p_connect(struct netemu_p2p_connection* p2p, struct netemu_sockaddr_in *in_addr, int in_addr_size,  struct netemu_sockaddr_in *connect_addr, int connect_addr_size) {
	int error;
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	p2p->_internal->host = netemu_tcp_listener_new(netemu_prepare_net_addr(in_addr),in_addr_size);
	connection = netemu_tcp_connection_new(netemu_prepare_net_addr(connect_addr),connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	netemu_tcp_connection_register_recv_fn(connection, netemu_tcp_connection_receive, p2p->info);
	error = netemu_tcp_connection_connect(connection);
	netemu_tcp_connection_start_receiving(connection);
	timestamp = time(NULL);
	netemu_packet_buffer_wait_for_instruction(p2p->info->_internal->receive_buffer, P2P_READY, timestamp);
	netemu_p2p_login(p2p);
	return error;
}

void netemu_p2p_login(struct netemu_p2p_connection *p2p) {
	struct application_instruction *message;
	struct netemu_packet_buffer_item *item;
	union netemu_connection_type type;
	time_t timestamp;
	type.collection = p2p->_internal->peers;
	message = netemu_application_create_message();
	netemu_application_p2p_login_request_add(message,p2p->_internal->host->addr,p2p->_internal->host->addr_len,p2p->info->username,p2p->info->emulator_name, 1);
	message->important = 1;
	timestamp = time(NULL);

	netemu_sender_buffer_add(p2p->info->_internal->send_buffer,message,CONNECTION_COLLECTION,type);
	item = netemu_packet_buffer_wait_for_instruction(p2p->info->_internal->receive_buffer, P2P_LOGIN_SUCCESS, timestamp);
}

void netemu_p2p_send_login_success(struct netemu_info *info, struct netemu_tcp_connection *recipient) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	type.connection = recipient;
	instruction = netemu_application_create_message();
	netemu_application_p2p_login_success_add(instruction,info->_internal->users,info->_internal->games);
	netemu_sender_buffer_add(info->_internal->send_buffer,instruction,TCP_CONNECTION,type);
}


int netemu_p2p_create_game(struct netemu_p2p_connection *connection, char *gamename, struct game** result) {
	struct game* game;
	struct application_instruction* instruction;
	union netemu_connection_type type;
	type.collection = connection->_internal->peers;
	instruction = netemu_application_create_message();
	netemu_application_p2p_create_game_add(instruction,gamename,connection->info->emulator_name,connection->user);
	game = (struct game*)instruction->body;
	game->player_count = 1;
	game->id = 0;
	netemu_list_add(connection->info->_internal->games, game);
	netemu_sender_buffer_add(connection->info->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 1;
}

void netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_info *info;
	struct p2p_login_success *success;
	struct p2p_user *user;
	struct p2p_game *game;
	int i;
	info = (struct netemu_info *)arg;
	success = (struct p2p_login_success*)item->instruction->body;
	for(i = 0; i < success->users_count; i++) {
		user = malloc(sizeof(struct p2p_user));
		netemu_application_p2p_copy_user(user, &success->users[i]);
		netemu_list_add(info->_internal->users,user);
	}

	for(i = 0; i < success->games_count; i++) {
		game = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(game, &success->games[i]);
		netemu_list_add(info->_internal->users,user);
	}
	info = (struct netemu_info *)arg;
}

void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_info* connection;
	connection = (struct netemu_info *)arg;
	user = (struct p2p_user*)item->instruction->body;
	connection->user_count++;
	user->name = item->instruction->user;
	netemu_list_add(connection->_internal->users,user);
	netemu_p2p_send_login_success(connection,item->connection.connection);
}

void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_info* connection;
	connection = (struct netemu_info *)arg;
	user = (struct p2p_user*)item->instruction->body;
	connection->user_count++;
	user->name = item->instruction->user;
	netemu_list_add(connection->_internal->users,user);
	netemu_p2p_send_login_success(connection,item->connection.connection);

}
