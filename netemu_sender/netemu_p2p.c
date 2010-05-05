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
void netemu_p2p_send_login_success(struct netemu_p2p_connection *info, struct netemu_tcp_connection *recipient);
void netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);
void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_send_user_joined(struct netemu_p2p_connection *connection, struct p2p_user *user);
void netemu_p2p_respond_to_player_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
void netemu_p2p_respond_to_game_created(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg);
int _netemu_p2p_addr_compare(const void *arg1, const void *arg2);
int _netemu_p2p_user_compare(const void *arg1, const void *arg2);
int _netemu_p2p_game_compare(const void *arg1, const void *arg2);
struct netemu_tcp_connection *netemu_p2p_connect_to(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size);
void _netemu_p2p_add_player(struct p2p_game * game, struct p2p_user *player);

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
	p2p->user->connection = 1;
	p2p->user->ping = 0;
	p2p->user->_internal = malloc(sizeof(struct p2p_user_internal));
	p2p->current_game = NULL;
	p2p->info = netemu_server_connection_new(username,emulatorname,buffer);
	netemu_list_add(p2p->info->_internal->users, p2p->user);
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	netemu_list_register_sort_fn(p2p->info->_internal->games, _netemu_p2p_game_compare);
	netemu_list_register_sort_fn(p2p->info->_internal->users, _netemu_p2p_user_compare);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_LOGIN_REQUEST, netemu_p2p_respond_to_login_request, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_LOGIN_SUCCESS, netemu_p2p_respond_to_login_success, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, P2P_USER_JOIN, netemu_p2p_respond_to_user_join, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, CREATE_P2P_GAME, netemu_p2p_respond_to_game_created, p2p);
	netemu_packet_buffer_add_instruction_received_fn(p2p->info->_internal->receive_buffer, JOIN_P2P_GAME, netemu_p2p_respond_to_player_join, p2p);
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

void netemu_p2p_host(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *addr, int addr_size, char* cloudname) {
	p2p->_internal->host = netemu_tcp_listener_new(netemu_util_copy_addr((struct sockaddr*)addr,addr_size),addr_size);
	p2p->cloud_name = cloudname;
	p2p->user->addr = p2p->_internal->host->addr;
	p2p->user->addr_size = p2p->_internal->host->addr_len;

	netemu_tcp_listener_register_new_connection_fn(p2p->_internal->host,netemu_p2p_new_connection,p2p);
	netemu_tcp_listener_start_listening(p2p->_internal->host);
}
/**
 * Connect to a host.
 */
int netemu_p2p_connect(struct netemu_p2p_connection* p2p, netemu_sockaddr_in *in_addr, int in_addr_size,  netemu_sockaddr_in *connect_addr, int connect_addr_size) {
	int error;
	struct netemu_tcp_connection *connection;
	error = 0;

	p2p->_internal->host = netemu_tcp_listener_new(netemu_util_copy_addr((netemu_sockaddr*)in_addr,in_addr_size),in_addr_size);
	p2p->user->addr = p2p->_internal->host->addr;
	p2p->user->addr_size = p2p->_internal->host->addr_len;
	connection = netemu_p2p_connect_to(p2p, netemu_util_copy_addr((netemu_sockaddr*)connect_addr,connect_addr_size), connect_addr_size);
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

struct netemu_tcp_connection *netemu_p2p_connect_to(struct netemu_p2p_connection *p2p, netemu_sockaddr *connect_addr, int connect_addr_size) {
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	int error;

	connection = netemu_tcp_connection_new(connect_addr,connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	netemu_tcp_connection_register_recv_fn(connection, netemu_tcp_connection_receive, p2p->info);
	error = netemu_tcp_connection_connect(connection);
	netemu_tcp_connection_start_receiving(connection);
	timestamp = time(NULL);
	netemu_packet_buffer_wait_for_instruction(p2p->info->_internal->receive_buffer, P2P_READY, timestamp);
	return connection;
}

void netemu_p2p_send_login_success(struct netemu_p2p_connection *info, struct netemu_tcp_connection *recipient) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	type.connection = recipient;
	instruction = netemu_application_create_message();
	netemu_application_p2p_login_success_add(instruction,info->user, info->info->_internal->users,info->info->_internal->games);
	netemu_sender_buffer_add(info->info->_internal->send_buffer,instruction,TCP_CONNECTION,type);
}


int netemu_p2p_create_game(struct netemu_p2p_connection *connection, char *gamename, struct game** result) {
	struct p2p_game* game;
	struct application_instruction* instruction;
	union netemu_connection_type type;
	type.collection = connection->_internal->peers;
	instruction = netemu_application_create_message();
	netemu_application_p2p_create_game_add(instruction,gamename,connection->info->emulator_name,connection->user);
	game = (struct p2p_game *)instruction->body;
	connection->current_game = game;
	netemu_list_add(connection->info->_internal->games, game);
	netemu_sender_buffer_add(connection->info->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 1;
}

int netemu_p2p_join_game(struct netemu_p2p_connection *connection, struct p2p_user *creator) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	if(_netemu_p2p_user_compare(connection->user,creator) == 0) {
		return -1;
	}
	if(creator->_internal == NULL || (creator->_internal->connection == NULL)) {
		creator->_internal = netemu_application_p2p_create_user_internal();
		creator->_internal->connection = netemu_p2p_connect_to(connection,creator->addr,creator->addr_size);

	}
	type.connection = creator->_internal->connection;
	instruction = netemu_application_create_message();
	netemu_application_p2p_player_join_add(instruction, connection->user);
	netemu_sender_buffer_add(connection->info->_internal->send_buffer, instruction, TCP_CONNECTION, type);

	return 1;
}

struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p_connection* info, int *count) {
	struct p2p_game** games;
	netemu_list_copy(info->info->_internal->games,&games);
	*count = info->info->_internal->games->count;
	return games;
}

struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p_connection* info, int *count) {
	struct p2p_user** users;
	netemu_list_copy(info->info->_internal->users,&users);
	*count = info->info->_internal->users->count;
	return users;
}

void netemu_p2p_respond_to_join_game(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;

	connection = (struct netemu_p2p_connection *)arg;
	if(connection->current_game != NULL) {
		type.collection = connection->_internal->peers;
		user = malloc(sizeof(struct p2p_user));

		netemu_application_p2p_copy_user(user,(struct p2p_user*)item->instruction->body);
		connection->info->user_count++;
		netemu_list_add(connection->info->_internal->users,user);
	}
}

void netemu_p2p_respond_to_login_success(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct netemu_p2p_connection *info;
	struct p2p_login_success *success;
	struct p2p_user *user;
	struct p2p_game *game;
	int i;
	info = (struct netemu_p2p_connection *)arg;
	success = (struct p2p_login_success*)item->instruction->body;
	for(i = 0; i < success->users_count; i++) {
		user = malloc(sizeof(struct p2p_user));
		netemu_application_p2p_copy_user(user, &success->users[i]);
		netemu_list_add(info->info->_internal->users,user);

		/* The first user in the list is always the host, so lets add the contact information we know is here. */
		if(i == 0 && item->type == TCP_CONNECTION) {
			user->_internal = netemu_application_p2p_create_user_internal();
			user->_internal->connection = item->connection.connection;
		}
	}
	for(i = 0; i < success->games_count; i++) {
		game = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(game, &success->games[i]);
		netemu_list_add(info->info->_internal->users,user);
	}
}

void netemu_p2p_respond_to_login_request(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;

	connection = (struct netemu_p2p_connection *)arg;
	type.collection = connection->_internal->peers;
	user = malloc(sizeof(struct p2p_user));
	netemu_application_p2p_copy_user(user,(struct p2p_user*)item->instruction->body);
	netemu_util_copy_string(&user->name, item->instruction->user);
	connection->info->user_count++;
	user->_internal = netemu_application_p2p_create_user_internal();
	switch(item->type) {
		case TCP_CONNECTION:
			user->_internal->connection = item->connection.connection;
			break;
		case UDP_CONNECTION:
			user->_internal->sender = item->connection.udp_sender;
			break;
		default:
			break;
	}

	netemu_p2p_send_login_success(connection,item->connection.connection);
	netemu_p2p_send_user_joined(connection, user);
	netemu_list_add(connection->info->_internal->users,user);
}

void netemu_p2p_send_user_joined(struct netemu_p2p_connection *connection, struct p2p_user *user) {
	struct application_instruction *instruction;
	union netemu_connection_type type;

	type.collection = connection->_internal->peers;
	instruction = netemu_application_create_message();
	netemu_application_p2p_user_join_add(instruction, user);
	netemu_sender_buffer_add(connection->info->_internal->send_buffer,instruction,CONNECTION_COLLECTION,type);
}


void netemu_p2p_respond_to_user_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if(netemu_list_contains(connection->info->_internal->users,user) == -1) {
		type.collection = connection->_internal->peers;
		connection->info->user_count++;
		user->name = item->instruction->user;
		netemu_list_add(connection->info->_internal->users,user);
		netemu_sender_buffer_add(connection->info->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);
	}
}

void netemu_p2p_respond_to_player_join(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if(connection->current_game != NULL) {
		_netemu_p2p_add_player(connection->current_game,user);
	}

	/*TODO: Send this join game to the other players.*/
}

void _netemu_p2p_add_player(struct p2p_game * game, struct p2p_user *player) {
	struct p2p_user* new_player_list;
	int i;
	game->user_count++;
	new_player_list = malloc(game->user_count*sizeof(struct p2p_user));
	for(i = 0; i < game->user_count-2; i++) {
		netemu_application_p2p_copy_user(&new_player_list[i],&game->players[i]);
	}
	netemu_application_p2p_copy_user(&new_player_list[game->user_count-2],player);
	free(game->players);
	game->players = new_player_list;
}

void netemu_p2p_respond_to_game_created(struct netemu_packet_buffer* buffer, struct netemu_packet_buffer_item *item, void* arg) {
	struct p2p_game *game;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int index;
	connection = (struct netemu_p2p_connection*)arg;
	game = (struct p2p_game*)item->instruction->body;

	if(netemu_list_contains(connection->info->_internal->games,game) == -1) {
		type.collection = connection->_internal->peers;
		connection->info->game_count++;

		if((index = netemu_list_contains(connection->info->_internal->users,game->creator)) != -1) {
			game->creator->_internal = ((struct p2p_user*)connection->info->_internal->users->elements[index])->_internal;
		}

		netemu_list_add(connection->info->_internal->games,game);
		netemu_sender_buffer_add(connection->info->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);
	}
}

int _netemu_p2p_user_compare(const void *arg1, const void *arg2) {
	const struct p2p_user* user1, *user2;
	int cmp, i;
	user1 = (const struct p2p_user*)arg1;
	user2 = (const struct p2p_user*)arg2;

	if((cmp = user1->addr_size-user2->addr_size) != 0)
		return cmp;

	for(i = 0; i < 14; i++) {
		cmp = user1->addr->sa_data[i]-user2->addr->sa_data[i];
		if(cmp != 0)
			return cmp;
	}

	return cmp;
}

int _netemu_p2p_game_compare(const void *arg1, const void *arg2) {
	const struct p2p_game* game1, *game2;
	int cmp, i;
	game1 = (const struct p2p_game*)arg1;
	game2 = (const struct p2p_game*)arg2;

	if((cmp = game1->creator->addr_size-game2->creator->addr_size) != 0)
		return cmp;

	for(i = 0; i < 14; i++) {
		cmp = game1->creator->addr->sa_data[i]-game2->creator->addr->sa_data[i];
		if(cmp != 0)
			return cmp;
	}

	return cmp;
}

int _netemu_p2p_addr_compare(const void *arg1, const void *arg2) {
	const netemu_sockaddr* addr1, *addr2;
	int cmp, i;
	addr1 = (const netemu_sockaddr*)arg1;
	addr2 = (const netemu_sockaddr*)arg2;

	cmp = addr1->sa_family-addr2->sa_family;

	if(cmp != 0)
		return cmp;

	for(i = 0; i < 14; i++) {
		cmp = addr1->sa_data[i]-addr2->sa_data[i];
		if(cmp != 0)
			return cmp;
	}

	return cmp;
}
