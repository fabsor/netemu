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
 * This file implements the netemu_p2p interface.
 * Basicly this file contains that glues the underlying packet handling
 * and the interface functions together.
 */

#include "../network/sender_buffer.h"
#include "../network/receiver_buffer.h"
#include "../network/sender_collection.h"
#include "../interface/netemu_p2p.h"
#include "netemu_p2p_internal.h"
#include "netlib_error.h"
#include "../protocol/application_p2p.h"
#include "../network/tcp.h"
#include "../structures/netemu_list.h"
#include "../protocol/application.h"
#include "../protocol/application_p2p_internal.h"

/* Function declarations */
void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection);
void _netemu_p2p_send_login_success(struct netemu_p2p *info, struct netemu_tcp_connection *recipient);
void _netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params);
void netemu_p2p_send_user_joined(struct netemu_p2p *connection, struct p2p_user *user);
int _netemu_p2p_addr_compare(const void *arg1, const void *arg2);
int _netemu_p2p_user_compare(const void *arg1, const void *arg2);
int _netemu_p2p_game_compare(const void *arg1, const void *arg2);
struct netemu_tcp_connection *_netemu_p2p_connect_to(struct netemu_p2p *p2p, netlib_sockaddr *connect_addr, int connect_addr_size);
void _netemu_p2p_add_player(struct p2p_game * game, struct p2p_user *player);
void netemu_p2p_join_host(struct netemu_p2p *p2p, struct p2p_user *user, struct netemu_tcp_connection *connection);
void _netemu_p2p_send_login_request(struct netemu_p2p *p2p);
struct netemu_tcp_connection *_netemu_p2p_connect_to_async(struct netemu_p2p *p2p, netlib_sockaddr *connect_addr, int connect_addr_size);
NETEMU_BOOL _netemu_p2p_player_exists(struct p2p_game *game, struct p2p_user *player);
void _netemu_process_user_value(struct netemu_p2p *info, struct p2p_user *player);
int _netemu_p2p_receive_play_values(struct netemu_p2p *info);
void netemu_p2p_respond_to_cached_play_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg);
void _netemu_p2p_register_responders(struct netemu_p2p *p2p);
void _netemu_p2p_join_game_continue(struct netemu_p2p *connection, struct p2p_game *game);
void _netemu_p2p_join_host_continue(struct netemu_p2p *connection);
void _netemu_p2p_create_callbacks(struct netemu_p2p *p2p);
void _netemu_p2p_create_user(struct netemu_p2p *p2p, char* username, char* emulatorname, char connection_quality);
void _netemu_p2p_create_callbacks(struct netemu_p2p *p2p);
void _netemu_p2p_send_join_game(struct netemu_p2p *connection, struct p2p_game *game);
void _netemu_p2p_remove_connection(struct netemu_sender_collection *collection, NETEMU_DWORD address, unsigned short port);
int _netemu_p2p_remove_player(struct p2p_game *game, struct p2p_user *player);
void _netemu_p2p_clear_users(struct netemu_list *users);
void _netemu_p2p_clear_games(struct netemu_list *games);
/* End of function declarations */


/**
 * Disconnect from a cloud.
 */
void netemu_p2p_disconnect(struct netemu_p2p *p2p) {
	struct application_instruction *instruction;
	struct transport_packet_buffer buffer;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_user_leave_add(instruction, p2p->user);
	/* It's time to lock down the sender buffer, no more packets will come in or out. */
	netemu_sender_buffer_lock(p2p->_internal->send_buffer);

	/* We can't rely on the sender buffer, since it will try to send thee data on a different thread.
	 * We will send this data directly to all peers in this thread,
	 * that way we know we can free the resources afterwards. */
	buffer = netemu_transport_pack(&instruction, 1);

	netemu_sender_collection_send_data(p2p->_internal->peers, buffer.data, buffer.size);
	netemu_sender_collection_clear(p2p->_internal->peers, TRUE);
	if(p2p->current_game != NULL) {
		netemu_sender_collection_clear(p2p->current_game->_internal->tcp_collection, TRUE);
		netemu_sender_collection_clear(p2p->current_game->_internal->udp_collection, TRUE);
	}
	p2p->cloud_name = NULL;
	_netemu_p2p_clear_games(p2p->_internal->games);
	_netemu_p2p_clear_users(p2p->_internal->users);
}

void _netemu_p2p_clear_games(struct netemu_list *games) {
	int i;
	for(i = 0; i < games->count; i++) {
		netemu_application_p2p_destroy_game(games->elements[i], TRUE, TRUE, FALSE);
	}
	netemu_list_clear(games);
}

void _netemu_p2p_clear_users(struct netemu_list *users) {
	/*
	 * TODO: Figure out how to handle the memory, then make sure this happens.
	int i;
	for(i = 0; i < users->count; i++) {
		netemu_application_p2p_destroy_game(games->elements[i], TRUE, TRUE, FALSE);
	}
	*/
	netemu_list_clear(users);

}

/**
 * Call this function to initialize the network.
 * You only need to call this once.
 * @ingroup netemu_p2p
 */
int netemu_p2p_initialize() {
	return netlib_init_network();
}

/**
 * Create a new netemu_p2p instance.
 * This function creates a new p2p instance.
 * @ingroup netemu_p2p
 * @param username the username that you want to use in this p2p instance.
 * @param application name the name of the application.
 * @param connection_quality The connection quality that you want to use. The connection quality
 * decides how fast you send data packets.\n <b>Example:</b>\n
 * If you have a connection_quality of 1, then you will send one play value, then wait for all others.\n
 * If you have a connection quality of 2, then you will send two play values, then wait for all others.\n
 * etc.
 * @return An instance of the netemu_p2p module if everything went fine, NULL otherwise.
 * @todo This function needs error handling.
 */
struct netemu_p2p* netemu_p2p_create(char* username, char* emulatorname, char connection_quality) {
	struct netemu_p2p *p2p;
	union netemu_connection_type *type;

	type = malloc(sizeof(union netemu_connection_type));
	type->collection = netemu_sender_collection_create();
	p2p = malloc(sizeof(struct netemu_p2p));

	_netemu_p2p_create_user(p2p,username, emulatorname, connection_quality);

	p2p->current_game = NULL;
	p2p->_internal = malloc(sizeof(struct netemu_p2p_internal));
	p2p->_internal->users = netemu_list_create(10,TRUE);
	p2p->_internal->games = netemu_list_create(10,TRUE);
	p2p->_internal->send_buffer = netemu_sender_buffer_new(5,20);
	p2p->_internal->receive_buffer = netemu_receiver_buffer_create(100);
	netemu_list_add(p2p->_internal->users, p2p->user);
	p2p->_internal->login_connection = NULL;
	p2p->_internal->continueFn = NULL;

	_netemu_p2p_create_callbacks(p2p);

	p2p->_internal->values_to_send = NULL;
	p2p->_internal->values_buffered = 0;
	p2p->_internal->sent_values = 0;
	p2p->_internal->frame_index = 0;

	netemu_list_register_sort_fn(p2p->_internal->games, _netemu_p2p_game_compare);
	netemu_list_register_sort_fn(p2p->_internal->users, _netemu_p2p_user_compare);
	_netemu_p2p_register_responders(p2p);
	p2p->_internal->peers = type->collection;
	return p2p;
}

/**
 * Register lists containing callbacks.
 */
void _netemu_p2p_create_callbacks(struct netemu_p2p *p2p) {
	p2p->_internal->login_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->play_values_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->game_created_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->join_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->game_started_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->all_ready_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->player_joined_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->player_ready_callbacks = netemu_list_create(2,TRUE);
	p2p->_internal->player_join_success_callbacks = netemu_list_create(1,TRUE);

}

/**
 * Create the user for the netemu_p2p instance.
 */
void _netemu_p2p_create_user(struct netemu_p2p *p2p, char* username, char* emulatorname, char connection_quality) {
	p2p->user = malloc(sizeof(struct p2p_user));
	p2p->user->name = username;
	p2p->user->app_name = emulatorname;
	p2p->user->connection = connection_quality;
	p2p->user->ping = 0;
	p2p->user->_internal = netemu_application_p2p_create_user_internal();
}

/**
 * Establish a new connection to a host.
 * @param listener the tcp listener you want to use.
 * @param connection the connection to the player you want to
 * establish a connection to.
 */
void _netemu_p2p_new_connection(struct netemu_tcp_listener* listener, struct netemu_tcp_connection* connection, void* params) {
	struct netemu_p2p *p2p;
	p2p = (struct netemu_p2p*)params;
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers, connection);
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp ,p2p->_internal->receive_buffer);
	_netemu_p2p_send_ready(p2p->_internal->send_buffer,connection);
}

void _netemu_p2p_send_ready(struct netemu_sender_buffer *buffer, struct netemu_tcp_connection *connection) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_ready_add(instruction);
	type.connection = connection;
	netemu_sender_buffer_add(buffer, instruction,TCP_CONNECTION, type);
}

/**
 * Host a new P2P cloud.
 * This will let other users connect to you on the designated port. By calling this function,
 * you will initiate a new netemu cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param address the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 */
int netemu_p2p_host(struct netemu_p2p* p2p, NETEMU_DWORD address, unsigned short port, char* cloudname) {
	int size;
	netlib_sockaddr* addr;
	addr = (netlib_sockaddr*)netemu_util_create_addr(address, port, &size);
	p2p->_internal->host = netemu_tcp_listener_new(addr, size);
	if(p2p->_internal->host->error != 0) {
		return -1;
	}
	p2p->user->addr = address;
	p2p->user->port = port;
	p2p->cloud_name = cloudname;

	netemu_tcp_listener_register_new_connection_fn(p2p->_internal->host,_netemu_p2p_new_connection,p2p);
	netemu_tcp_listener_start_listening(p2p->_internal->host);
	return 0;
}

/**
 * Connect to a host. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte order.
 */
int netemu_p2p_connect(struct netemu_p2p* p2p, NETEMU_DWORD in_addr, unsigned short port,  NETEMU_DWORD connect_addr, unsigned short connect_port) {
	int error, size;
	struct netemu_tcp_connection *connection;
	netlib_sockaddr *saddr;

	error = 0;
	saddr = (netlib_sockaddr*)netemu_util_create_addr(connect_addr,connect_port,&size);
	netemu_p2p_host(p2p, in_addr,port,"thecloud");
	connection = _netemu_p2p_connect_to(p2p,saddr,size);
	netemu_p2p_login(p2p);
	return error;
}

/**
 * Connect asyncronously to a cloud. This function let's you connect to an existing cloud by specifying an address to which users can
 * connect to you and the address to one of the computers in the cloud.
 * @todo This function needs to be refactored to use addresses and ports as long and short instead of structs.
 * @param p2p the netemu_p2p module instance.
 * @param in_addr the address you want to bind to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param port the port you want to use. Please make sure that this port is in network byte order.
 * Please make sure to run a function like netemu_htons on the desired port number.
 * @param connect_address the address you want to connect to. This address should be accessible to the public.
 * This address must be in network byte order format. Please make sure to run a function like
 * netemu_htonl on the desired address.
 * @param connect_port the port you want to connect to. Please make sure that this port is in network byte orde
 */
int netemu_p2p_connect_async(struct netemu_p2p* p2p, netlib_sockaddr_in *in_addr, int in_addr_size,  netlib_sockaddr_in *connect_addr, int connect_addr_size, connectedFn callback) {
	int error;
	struct netemu_tcp_connection *connection;
	union p2p_callback_fn *fn;
	fn = malloc(sizeof(union p2p_callback_fn));
	fn->connectFn = callback;
	error = 0;
	netemu_list_add(p2p->_internal->login_callbacks,fn);
	netemu_p2p_host(p2p, in_addr, in_addr_size,"thecloud");
	connection = _netemu_p2p_connect_to_async(p2p, netemu_util_copy_addr((netlib_sockaddr*)connect_addr,connect_addr_size), connect_addr_size);
	netemu_p2p_login(p2p);
	return error;
}

/**
 * Login to the cloud. In order to participate, you need to be logged in. After logging in,
 * you receive a list of games and users, and you will be notified about new games and new users.
 * @ingroup netemu_p2p
 * @param p2p the netemu module instance.
 */
void netemu_p2p_login(struct netemu_p2p *p2p) {
	struct application_instruction *message;
	struct netemu_receiver_buffer_item *item;
	union netemu_connection_type type;
	time_t timestamp;
	type.collection = p2p->_internal->peers;
	message = netemu_application_instruction_create();
	netemu_application_p2p_login_request_add(message,p2p->user->addr,p2p->user->port,p2p->user->name,p2p->user->app_name, 1);
	message->important = 1;
	timestamp = time(NULL);

	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,CONNECTION_COLLECTION,type);
	item = netemu_receiver_buffer_wait_for_instruction(p2p->_internal->receive_buffer, P2P_LOGIN_SUCCESS, timestamp);
}

/**
 * Join a host. In order to be able to use a connection to a user, you must send this instruction.
 * This will make the host aware about your existence and it will send data to you. Note that this is
 * not the same as logging in, since this only make the host aware of your connection.
 * If you do not send a join_host instruction to every user you connect to, it will automaticly discard your
 * connection.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param user the user you want to identify yourself as.
 * @param connection the connection you want to send the instruction on.
 */
void netemu_p2p_join_host(struct netemu_p2p *p2p, struct p2p_user *user, struct netemu_tcp_connection *connection) {
	struct application_instruction *message;
	union netemu_connection_type type;
	time_t timestamp;
	message = netemu_application_instruction_create();
	netemu_application_p2p_join_host_add(message, user);
	type.connection = connection;
	message->important = 1;
	timestamp = time(NULL);
	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,TCP_CONNECTION,type);
}

/**
 * Connect to a specific player.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param addr the netemu_sockaddr to connect to.
 * @param the size of the struct.
 */
struct netemu_tcp_connection *_netemu_p2p_connect_to(struct netemu_p2p *p2p, netlib_sockaddr *connect_addr, int connect_addr_size) {
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	int error;

	connection = netemu_tcp_connection_new(connect_addr,connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	error = netemu_tcp_connection_connect(connection);
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp, p2p->_internal->receive_buffer);
	timestamp = time(NULL);
	netemu_receiver_buffer_wait_for_instruction(p2p->_internal->receive_buffer, P2P_READY, timestamp);
	return connection;
}

/**
 * Connect asyncronously.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 * @param addr the netemu_sockaddr to connect to.
 * @param the size of the struct.
 */
struct netemu_tcp_connection *_netemu_p2p_connect_to_async(struct netemu_p2p *p2p, netlib_sockaddr *connect_addr, int connect_addr_size) {
	time_t timestamp;
	struct netemu_tcp_connection *connection;
	int error;

	connection = netemu_tcp_connection_new(connect_addr,connect_addr_size);
	netemu_sender_collection_add_tcp_sender(p2p->_internal->peers,connection);
	p2p->_internal->continueFn = _netemu_p2p_send_login_request;
	netemu_tcp_connection_start_receiving(connection, netemu_application_parse_tcp, p2p->_internal->receive_buffer);
	error = netemu_tcp_connection_connect(connection);
	timestamp = time(NULL);
	return connection;
}

/**
 * Send a login request.
 * This function just sends a login request to the host, and will not block and
 * wait for a response.
 * @ingroup netemu_p2p
 * @param p2p the netemu_p2p module instance.
 */
void _netemu_p2p_send_login_request(struct netemu_p2p *p2p) {
	struct application_instruction *message;
	union netemu_connection_type type;
	time_t timestamp;
	type.collection = p2p->_internal->peers;
	message = netemu_application_instruction_create();
	netemu_application_p2p_login_request_add(message,p2p->user->addr,p2p->user->port,p2p->user->name,p2p->user->app_name, 1);
	message->important = 1;
	timestamp = time(NULL);
	netemu_sender_buffer_add(p2p->_internal->send_buffer,message,CONNECTION_COLLECTION,type);
}

/**
 * Send a login success instruction.
 * @ingroup netemu_p2p
 * @param info the netemu_p2p instance.
 * @param recipient the recipient.
 */
void _netemu_p2p_send_login_success(struct netemu_p2p *info, struct netemu_tcp_connection *recipient) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	type.connection = recipient;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_login_success_add(instruction,info->user, info->_internal->users,info->_internal->games);
	netemu_sender_buffer_add(info->_internal->send_buffer,instruction,TCP_CONNECTION,type);
}

/**
 * Create a new game. This function will create a new game which will be broadcasted to all
 * peers. This is a non-blocking call, since there's no requirement (or a way to check) that
 * all other users have received the notification that the game have been created.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param gamename the name of the game you want to create
 * @param connection_quality the quality of the connection for this game.
 * @param emulator_value_size This is the size of the play values for one player in the game.
 * No value sent over the network in this game can be larger than this.
 * @param result the created game.
 * @return The return value can be:
 * - 0 if all went well
 * - -1 if something went wrong. This generally indicates that you already have created a game.
 */
int netemu_p2p_create_game(struct netemu_p2p *connection, char *gamename, char connection_quality, int emulator_value_size, struct p2p_game** result) {
	struct p2p_game* game;
	struct application_instruction* instruction;
	union netemu_connection_type type;

	if(connection->current_game != NULL)
		return -1;

	type.collection = connection->_internal->peers;
	instruction = netemu_application_instruction_create();
	connection->user->_internal->player_no = 1;
	/* We add our new game to the list. */
	netemu_application_p2p_create_game_add(instruction,gamename,connection->user->app_name,connection->user, connection_quality, emulator_value_size);
	game = (struct p2p_game *)instruction->body;
	/* We have created a game, so we will set this to our current game. */
	connection->current_game = game;
	connection->_internal->play_values_event = netlib_thread_event_create();
	netemu_list_add(connection->_internal->games, game);
	game->_internal = netemu_application_p2p_create_game_internal();
	game->_internal->tcp_collection = netemu_sender_collection_create();
	game->_internal->udp_collection = netemu_sender_collection_create();
	game->creator->_internal->values_received = TRUE;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 0;
}

int netemu_p2p_leave_game(struct netemu_p2p *connection) {
	struct application_instruction *instruction;
	union netemu_connection_type type;
	type.collection = connection->current_game->_internal->tcp_collection;
	if(connection->current_game == NULL) {
		netlib_set_last_error(NETEMU_P2P_ENOTINGAME);
		return -1;
	}
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_player_leave_add(instruction, connection->user);
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	connection->current_game = NULL;
	return 0;
}

int _netemu_p2p_remove_player(struct p2p_game *game, struct p2p_user *player) {
	int i, removeIndex;
	struct p2p_user *players;
	removeIndex = -1;
	for(i = 0; i < game->user_count-1; i++) {
		if(_netemu_p2p_user_compare(&game->players[i],player) == 0) {
			removeIndex = i;
		}
	}
	if(removeIndex != -1) {
		players = malloc(sizeof(struct p2p_user)*game->user_count-2);
		for(i = 0; i< game->user_count-1; i++) {
			if(removeIndex != i) {
				players[i] = game->players[i];
			}
		}
		_netemu_p2p_remove_connection(game->_internal->tcp_collection,
									game->players[removeIndex].addr,
									game->players[removeIndex].port);
		_netemu_p2p_remove_connection(game->_internal->udp_collection,
											game->players[removeIndex].addr,
											game->players[removeIndex].port);
		free(game->players);
		game->user_count--;

		game->players = players;
		return 0;
	}
	return -1;
}

void _netemu_p2p_remove_connection(struct netemu_sender_collection *collection, NETEMU_DWORD address, unsigned short port) {
	struct netemu_sender_collection_item *item;
	struct netemu_list_iterator *iterator;
	int removeIndex;
	netlib_sockaddr_in *addr;
	removeIndex = -1;
	iterator = netemu_list_iterator_create(collection->senders);

	while((item = (struct netemu_sender_collection_item*)netemu_list_iterator_next(iterator)) != NULL) {
		if(item->type == SENDER_TCP) {
			addr = (netlib_sockaddr_in*)item->sender->tcp_sender->addr;
		}
		else if(item->type == SENDER_UDP){
			addr = (netlib_sockaddr_in*)item->sender->udp_sender->addr;
		}

		if(addr->sin_addr.s_addr == address && addr->sin_port == port) {
			removeIndex = iterator->index;
			break;
		}
	}
	netemu_list_iterator_destroy(iterator);
	if(removeIndex != -1) {
		netemu_sender_collection_remove_sender(collection, removeIndex);
	}
}

/**
 * Join an existing game. This is a blocking call that will connect to the creator of the
 * game if there isn't a connection already, and then send a "Join game" instruction.
 * @param connection an instance of the netemu_p2p module.
 * @param game the game you want to join. Use netemu_get_game_list to get a game to join.
 * @return the return value can be:
 * - 0 if everything went alright.
 * - -1 if you're already in a game.
 * - -2 if you try to join a game which you have created.
 * @todo we need more errors here for:
 * - if you can't connect to a host.
 * - if the host refuses you.
 */
int netemu_p2p_join_game(struct netemu_p2p *connection, struct p2p_game *game) {
	struct netemu_receiver_buffer_item *item;
	time_t timestamp;
	netlib_sockaddr* addr;
	int size, i;

	/* You can't join a game if you're already in one.*/
	if(connection->current_game != NULL) {
		return -1;
	}

	/* We can't join a game created by you, since you're always part of your own game.*/
	if(_netemu_p2p_user_compare(connection->user,game->creator) == 0) {
		return -2;
	}

	timestamp = time(NULL);
	connection->user->_internal->values_received = TRUE;
	/* Create a connection to the creator of this game if we don't have one already. */
	if(game->creator->_internal == NULL || (game->creator->_internal->connection == NULL)) {
		game->creator->_internal = netemu_application_p2p_create_user_internal();
		addr = (netlib_sockaddr*)netemu_util_create_addr(game->creator->addr,game->creator->port,&size);
		game->creator->_internal->connection = _netemu_p2p_connect_to(connection,addr,size);
		netemu_p2p_join_host(connection,game->creator, game->creator->_internal->connection);
		netemu_receiver_buffer_wait_for_instruction(connection->_internal->receive_buffer, P2P_READY, timestamp);
	}

	_netemu_p2p_send_join_game(connection, game);
	item = netemu_receiver_buffer_wait_for_instruction(connection->_internal->receive_buffer, P2P_PLAYER_JOIN_SUCCESS, timestamp);

	connection->current_game = item->instruction->body;
	/* The creator from the old game has an active connection */
	connection->current_game->creator = game->creator;

	/*Create internal stuff*/
	connection->current_game->_internal = netemu_application_p2p_create_game_internal();
	connection->current_game->_internal->tcp_collection = netemu_sender_collection_create();
	connection->current_game->_internal->udp_collection = netemu_sender_collection_create();

	/* We need to go through the array of players and establish a link to them. */
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(_netemu_p2p_user_compare(&connection->current_game->players[i], connection->user) == 0) {
			connection->current_game->players[i]._internal = connection->user->_internal;
			connection->user->_internal->player_no = i+2;
		}
		else {
			connection->current_game->players[i]._internal = netemu_application_p2p_create_user_internal();
			addr = (netlib_sockaddr*)netemu_util_create_addr(connection->current_game->players[i].addr,connection->current_game->players[i].port,&size);
			connection->current_game->players[i]._internal->connection = _netemu_p2p_connect_to(connection,addr,size);
			netemu_p2p_join_host(connection,game->creator, game->creator->_internal->connection);
		}
		connection->current_game->players[i]._internal->player_no = i+2;
	}

	netemu_sender_collection_add_tcp_sender(connection->current_game->_internal->tcp_collection, connection->current_game->creator->_internal->connection);
	connection->_internal->play_values_event = netlib_thread_event_create();
	return 0;
}

int netemu_p2p_join_game_async(struct netemu_p2p *connection, struct p2p_game *game, p2pPlayerJoinedFn fn, void* user_data) {
	time_t timestamp;
	int size;
	union p2p_callback_fn callback;
	netlib_sockaddr* addr;

	/* You can't join a game if you're already in one.*/
	if(connection->current_game != NULL) {
		return -1;
	}

	/* We can't join a game created by you, since you're always part of your own game.*/
	if(_netemu_p2p_user_compare(connection->user,game->creator) == 0) {
		return -2;
	}

	timestamp = time(NULL);
	connection->user->_internal->values_received = TRUE;
	callback.playerJoinedSuccessFn = _netemu_p2p_join_game_continue;
	_netemu_p2p_register_callback(connection->_internal->player_join_success_callbacks, callback, 1, NULL);

	callback.playerJoinedFn = fn;
	_netemu_p2p_register_callback(connection->_internal->player_joined_callbacks, callback, 1, user_data);
	connection->current_game = game;
	/* Create a connection to the creator of this game if we don't have one already. */
	if(game->creator->_internal == NULL || (game->creator->_internal->connection == NULL)) {
		game->creator->_internal = netemu_application_p2p_create_user_internal();
		addr = (netlib_sockaddr*)netemu_util_create_addr(game->creator->addr,game->creator->port,&size);
		game->creator->_internal->connection = _netemu_p2p_connect_to(connection,addr,size);
		connection->_internal->continueFn = _netemu_p2p_join_host_continue;
		netemu_p2p_join_host(connection,game->creator, game->creator->_internal->connection);
	}
	else {
		_netemu_p2p_send_join_game(connection, game);
	}
	return 0;
}

void _netemu_p2p_send_join_game(struct netemu_p2p *connection, struct p2p_game *game) {
	union netemu_connection_type type;
	struct application_instruction *instruction;
	type.connection = game->creator->_internal->connection;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_player_join_add(instruction, connection->user);
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, TCP_CONNECTION, type);
}

/**
 * Join a game after joining a host.
 */
void _netemu_p2p_join_host_continue(struct netemu_p2p *connection) {
	connection->_internal->continueFn = NULL;
	_netemu_p2p_send_join_game(connection, connection->current_game);
}

/**
 * This function is called after a p2p ready has been received.
 */
void _netemu_p2p_join_game_continue(struct netemu_p2p *connection, struct p2p_game *game) {
	int i;
	netlib_sockaddr *addr;
	int size;

	/* The creator from the old game has an active connection */
	game->creator = connection->current_game->creator;
	connection->current_game = game;

	/*Create internal stuff*/
	connection->current_game->_internal = netemu_application_p2p_create_game_internal();
	connection->current_game->_internal->tcp_collection = netemu_sender_collection_create();
	connection->current_game->_internal->udp_collection = netemu_sender_collection_create();

	/* We need to go through the array of players and establish a link to them. */
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(_netemu_p2p_user_compare(&connection->current_game->players[i], connection->user) == 0) {
			connection->current_game->players[i]._internal = connection->user->_internal;
			connection->user->_internal->player_no = i+2;
		}
		else {
			connection->current_game->players[i]._internal = netemu_application_p2p_create_user_internal();
			addr = (netlib_sockaddr*)netemu_util_create_addr(connection->current_game->players[i].addr,connection->current_game->players[i].port,&size);
			connection->current_game->players[i]._internal->connection = _netemu_p2p_connect_to(connection,addr,size);
			netemu_p2p_join_host(connection,game->creator, game->creator->_internal->connection);
		}
		connection->current_game->players[i]._internal->player_no = i+2;
	}

	netemu_sender_collection_add_tcp_sender(connection->current_game->_internal->tcp_collection, connection->current_game->creator->_internal->connection);
	connection->_internal->play_values_event = netlib_thread_event_create();
}

/**
 * Start a game. In order for the game to start, you must call this function.
 * This function will send a start game instruction to all players, and this will
 * cause them to make the necessary arrangement for playing the game such as binding to
 * UDP ports. If you are the creator of the game, the only thing you have to do is
 * call this function and it will be implicated that you are ready to play. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 0 if everything went OK
 * - -1 if you're not in a game or if you're not the owner.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_start_game(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port) {
	struct netemu_receiver_udp *receiver;
	struct application_instruction *instruction;
	union netemu_connection_type type;
	netlib_sockaddr_in addr;
	int cmp;
	/*We can't start a game if a game is not created first, and you are the owner of the game.*/
	if(connection->current_game == NULL || (cmp = _netemu_p2p_user_compare(connection->current_game->creator, connection->user)) != 0)
		return -1;

	/* Create an addr_in  */
	addr.sin_addr.s_addr = listen_addr;
	addr.sin_family = NETLIB_AF_INET;
	addr.sin_port = listen_port;

	receiver = netemu_receiver_udp_create((netlib_sockaddr*)&addr,sizeof(addr));
	netemu_receiver_udp_start_receiving(receiver, netemu_application_parse_udp,connection->_internal->receive_buffer);
	connection->user->_internal->receiver = receiver;
	connection->current_game->creator->_internal = connection->user->_internal;
	connection->_internal->values_received = malloc((connection->current_game->emulator_value_size*
			connection->current_game->user_count)*connection->current_game->connection_quality);
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_start_game_add(instruction, listen_addr, listen_port);
	/* This instruction will be broadcasted to all, in order for all clients to register that the game has started. */
	type.collection = connection->_internal->peers;
	connection->current_game->_internal->ready_count++;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 0;
}

/**
 * Indicate that you are ready to play. In order for the game to start, you must call this function.
 * This function will send a player ready instruction to all players, and this will
 * cause them to make register the address to which they can contact you wile playing.
 * If you are the creator of the game, you don't have to call this function, since netemu_p2p_start_game will
 * do everything this function does for you. Also note that you have to recieve the start game instruction before
 * calling this function. This function is <b>Not blocking</b>
 * and you have to register a callback function in order to know when the other players are ready.
 * @ingroup netemu_p2p
 * @param connection an instance of the netemu_p2p module.
 * @param listen_addr the address you want to listen to. This must be in network byte order.
 * @param listen_port the port you want to bind to. This must be in network byte order.
 * @return this function will return one of the following:
 * - 0 if everything went OK
 * - -1 if you're not in a game, if you're the creator of this game or if the game hasn't started.
 * @todo We need to add more errors, specifically:
 * - An error that indicates that we couldn't bind to the address.
 */
int netemu_p2p_player_ready(struct netemu_p2p *connection, NETEMU_DWORD listen_addr, unsigned short listen_port) {
	struct netemu_receiver_udp *receiver;
	struct application_instruction *instruction;
	struct netemu_list_iterator *iterator;
	union netemu_connection_type type;
	struct p2p_callback *item;
	netlib_sockaddr_in addr;
	/* We can't join a game if a game is not created and started first, and you are not the owner of the game. */
	if(connection->current_game == NULL || (_netemu_p2p_user_compare(connection->current_game->creator, connection->user)) == -1)
		return -1;

	addr.sin_addr.s_addr = listen_addr;
	addr.sin_family = NETLIB_AF_INET;
	addr.sin_port = listen_port;

	receiver = netemu_receiver_udp_create((netlib_sockaddr*)&addr,sizeof(addr));
	if(receiver == NULL) {
		return -1;
	}
	netemu_receiver_udp_start_receiving(receiver, netemu_application_parse_udp, connection->_internal->receive_buffer);
	connection->user->_internal->receiver = receiver;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_player_ready_add(instruction, listen_addr, listen_port);
	connection->current_game->_internal->ready_count++;
	if(connection->current_game->_internal->ready_count >= connection->current_game->user_count) {
		connection->current_game->started = 1;
		iterator = netemu_list_iterator_create(connection->_internal->all_ready_callbacks);
		while((item = netemu_list_iterator_next(iterator)) != NULL) {
			item->fn.allReadyFn(connection, connection->current_game);
		}
		netemu_list_iterator_destroy(iterator);
	}
	/* This instruction will be broadcasted to all game members */
	type.collection = connection->current_game->_internal->tcp_collection;
	netemu_sender_buffer_add(connection->_internal->send_buffer, instruction, CONNECTION_COLLECTION, type);
	return 0;
}

/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_game** netemu_p2p_get_game_list(struct netemu_p2p* info, int *count) {
	struct p2p_game** games;
	netemu_list_copy(info->_internal->games,(void***)&games);
	*count = info->_internal->games->count;
	return games;
}

/**
 * Get a list of games in the netemu p2p cloud.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param count the number of games there are in the list. This parameter will be set by the
 * function.
 * @return a list of p2p games that are available in the cloud. This list is a copy, and
 * you can free the memory of it, just don't free the games inside of it.
 */
struct p2p_user** netemu_p2p_get_user_list(struct netemu_p2p* info, int *count) {
	struct p2p_user** users;
	netemu_list_copy(info->_internal->users,(void***)&users);
	*count = info->_internal->users->count;
	return users;
}


/**
 * This function responds to all "User joined" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_send_user_joined(struct netemu_p2p *connection, struct p2p_user *user) {
	struct application_instruction *instruction;
	union netemu_connection_type type;

	type.collection = connection->_internal->peers;
	instruction = netemu_application_instruction_create();
	netemu_application_p2p_user_join_add(instruction, user);
	netemu_sender_buffer_add(connection->_internal->send_buffer,instruction,CONNECTION_COLLECTION,type);
}

/**
 * Check if a player exists.
 * @todo Use binary search instead (micro-optimization)
 * @ingroup p2p_player
 * @param game the game to check in
 * @param player the player to check for.
 */
NETEMU_BOOL _netemu_p2p_player_exists(struct p2p_game *game, struct p2p_user *player) {
	int i;
	if(_netemu_p2p_user_compare(player,game->creator) == 0) {
		return TRUE;
	}
	if(game->_internal != NULL) {
		netlib_thread_mutex_lock(game->_internal->game_lock, NETLIB_INFINITE);
	}
	for(i = 0; i < game->user_count-1; i++) {
		if(_netemu_p2p_user_compare(player,&game->players[i]) == 0) {
			netlib_thread_mutex_release(game->_internal->game_lock);
			return TRUE;
		}
	}
	if(game->_internal != NULL) {
		netlib_thread_mutex_release(game->_internal->game_lock);
	}
	return FALSE;
}


/**
 * Add a player to the list of players in a game.
 * @ingroup p2p_player
 * @param game the game to which the player should be added.
 * @param player the player that should be added to the game.
 * @todo Better memory handling for the array containing the players.
 */
void _netemu_p2p_add_player(struct p2p_game *game, struct p2p_user *player) {
	struct p2p_user* new_player_list;
	int i;
	/* If we have an internal struct, then we have a lock which we can use. */
	if(game->_internal != NULL) {
		netlib_thread_mutex_lock(game->_internal->game_lock, NETLIB_INFINITE);
	}

	game->user_count++;
	/* The player number is the same as the current count. */
	player->_internal->player_no = game->user_count;
	/* Allocate enough space for the new player list, and copy the existing users.
	   TODO: Try to use realloc here and if that fails, use this method. */
	new_player_list = malloc(game->user_count*sizeof(struct p2p_user));
	for(i = 0; i < game->user_count-2; i++) {
		netemu_application_p2p_copy_user(&new_player_list[i],&game->players[i]);
	}
	netemu_application_p2p_copy_user(&new_player_list[game->user_count-2],player);
	free(game->players);
	game->players = new_player_list;
	/* Release the lock. */
	if(game->_internal != NULL) {
		netlib_thread_mutex_release(game->_internal->game_lock);
	}
}

/**
 * Send play values to all clients.
 * This function will block if you haven't received play values from your co-players, and
 * will handle all the synchronization for you.
 * @ingroup netemu_p2p
 * @param info an instance of the netemu_p2p module.
 * @param data the data to send. The block must contain data with the size of emulator_value_size, and be big enough to contain
 * data from all players.
 * @return 0 if everything went alright.
 * @todo Error handling here.
 */
int netemu_p2p_send_play_values(struct netemu_p2p* info, void* data) {
	time_t timestamp;
	int size;
	struct application_instruction *message;
	union netemu_connection_type type;
	int cache_index;
	size = info->current_game->emulator_value_size;

	if(info->_internal->values_to_send == NULL) {
		info->_internal->values_to_send = malloc(info->current_game->connection_quality*size);
	}

	memcpy(info->_internal->values_to_send + (info->_internal->values_buffered * size), data, size);

	/* Copy data from the current frame index. */
	if(info->current_game->_internal->sent_first_values) {
		memcpy(data, info->_internal->values_received + (size * info->current_game->user_count*info->_internal->frame_index), size * info->current_game->user_count * info->current_game->connection_quality);
	}

	info->_internal->frame_index++;
	info->_internal->values_buffered++;

	/* We have used up all our frames, we need new ones. Let's send the data we collected and then receive data from other players. */
	if(info->_internal->values_buffered == info->current_game->connection_quality) {
		message = netemu_application_instruction_create();
		cache_index = _netemu_p2p_value_in_cache(info->user, info->_internal->values_to_send, info->current_game->emulator_value_size);
		if(cache_index == -1) {
			netemu_application_p2p_buffered_play_values_add(message,info->user->_internal->player_no, size,data);
			netemu_list_add(info->user->_internal->play_values, message);
		}
		else {
			netemu_application_p2p_cached_play_values_add(message,info->user->_internal->player_no, cache_index);
		}
		type.collection = info->current_game->_internal->udp_collection;
		timestamp = time(NULL);
		netemu_sender_buffer_add(info->_internal->send_buffer,message, CONNECTION_COLLECTION, type);
		info->_internal->values_buffered = 0;
		_netemu_p2p_receive_play_values(info);
		info->_internal->frame_index = 0;
		info->current_game->_internal->sent_first_values = TRUE;
	}
	return 0;
}

/**
 * Locate a value in the cache.
 * @param user in which cache the value should exist.
 * @param data the data which should be in the cache
 * @param size the size fo the data.
 * @return the index of the value if it was found, -1 otherwise.
 */
int _netemu_p2p_value_in_cache(struct p2p_user *user, char* data, int size) {
	int i;
	for(i = 0; i < user->_internal->cache_index; i++) {
		if(memcmp(user->_internal->cache[i].values, data, size) == 0) {
			return i;
		}
	}
	return -1;
}

int _netemu_p2p_receive_play_values(struct netemu_p2p *info) {
	int i;
	struct p2p_user *player;

	/* Wait for values if there we haven't received values from all players. */
	while(!info->current_game->_internal->all_values_received) {
		netlib_thread_event_wait(info->_internal->play_values_event, NETLIB_INFINITE);
	}
	netlib_thread_mutex_lock(info->current_game->_internal->game_lock, NETLIB_INFINITE);
	_netemu_process_user_value(info, info->current_game->creator);
	for(i = 0; i < info->current_game->user_count-1; i++) {
		player = &info->current_game->players[i];
		_netemu_process_user_value(info, player);
	}
	info->current_game->_internal->all_values_received = FALSE;
	netlib_thread_mutex_release(info->current_game->_internal->game_lock);
	return 0;
}

void _netemu_process_user_value(struct netemu_p2p *info, struct p2p_user *player) {
	struct p2p_buffered_play_values *values;
	struct application_instruction *instruction;
	int index, i;
	char *insert_ptr, *retrieve_ptr;
	instruction = player->_internal->play_values->elements[0];
	netemu_list_remove_at(player->_internal->play_values, 0);
	if(instruction->id == P2P_CACHED_BUFFERED_PLAY_VALUES) {
		index = ((struct p2p_cached_buffered_play_values*)instruction->body)->index;
	}
	else if(instruction->id == P2P_BUFFERED_PLAY_VALUES) {
		values = instruction->body;
		if(player->_internal->cache_index == 256) {
			for(i = 0; i < 255; i++) {
				player->_internal->cache[i] = player->_internal->cache[i+1];
			}
			netemu_application_p2p_buffered_play_values_copy(&player->_internal->cache[255], values);
			index = 255;
		}
		else {
			netemu_application_p2p_buffered_play_values_copy(&player->_internal->cache[player->_internal->cache_index], values);
			index = player->_internal->cache_index;
			player->_internal->cache_index++;

		}
	}
	for(i = 0; i < info->current_game->connection_quality; i++) {
		/*
		 * This is very tricky.
		 * First, some definitions:
		 * - A frame represents the input data one player sends for one frame in the game.
		 * - value_size is the size of one players input in a frame.
		 * - connection_quality is the number of frames in the packet.
		 *
		 * One block is always (value_size * player_count) big.
		 * If connection quality is bigger than 1, we have more than one frame in here. That means we have a block with the size
		 * (value_size * player_count * connection_quality) in total.
		 * We want to insert a specific player's value in the right place.
		 * We got a block from the other player containing (emulator_value_size * connection_quality).
		 * We need to:
		 *   - Get the data for each frame from the users data. We do that by adding (emulator_value_size * frame) in the block we received
		 *     from the other player.
		 *	 - We need to take the part described above, and insert it in the right place in the block we will return to the user. The right place
		 *	   means (player_no * emulator_value_size * frame).
		 * i is the frame below.
		 */
		insert_ptr = info->_internal->values_received + (i * (info->current_game->user_count * info->current_game->emulator_value_size)) /* Moves us to the right frame */ +
				(info->current_game->emulator_value_size*(player->_internal->player_no-1)); /* Moves us to the right place in the block. */

		retrieve_ptr = player->_internal->cache[index].values + (info->current_game->emulator_value_size * i); /* Moves us to the right place in the block */
		memcpy(insert_ptr, retrieve_ptr ,info->current_game->emulator_value_size);

	}
	player->_internal->values_received = FALSE;


}

/**
 * Compare two p2p users.
 * @ingroup netemu_p2p
 * @param arg1 the first user
 * @param arg2 the second user
 * @return value bigger than 0 if the user 1 is bigger than the other one, 0 if their equal and less than zero if user 1 is less than user 2
 */
int _netemu_p2p_user_compare(const void *arg1, const void *arg2) {
	const struct p2p_user* user1, *user2;
	user1 = (const struct p2p_user*)arg1;
	user2 = (const struct p2p_user*)arg2;

	return (user1->addr-user2->addr) + (user1->port - user2->port);
}

int _netemu_p2p_game_compare(const void *arg1, const void *arg2) {
	const struct p2p_game* game1, *game2;
	game1 = (const struct p2p_game*)arg1;
	game2 = (const struct p2p_game*)arg2;

	return (game1->creator->addr-game2->creator->addr) + (game1->creator->port - game2->creator->port);
}

int _netemu_p2p_addr_compare(const void *arg1, const void *arg2) {
	const netlib_sockaddr* addr1, *addr2;
	int cmp, i;
	addr1 = (const netlib_sockaddr*)arg1;
	addr2 = (const netlib_sockaddr*)arg2;
	cmp = 0;

	for(i = 0; i < 14; i++) {
		cmp = addr1->sa_data[i]-addr2->sa_data[i];
		if(cmp != 0)
			return cmp;
	}
	return cmp;
}

