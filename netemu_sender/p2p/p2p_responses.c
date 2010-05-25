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
 * This file contains all responses that netemu_p2p uses.
 */
#include "p2p_responses.h"
#include "netemu_p2p_internal.h"
#include "../protocol/application_p2p_internal.h"
#include "../network/sender_collection.h"

/**
 * Register responders that will be used by this module.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responces
 * @param p2p the netemu_p2p instance.
 */
void _netemu_p2p_register_responders(struct netemu_p2p_connection *p2p) {
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_LOGIN_REQUEST, netemu_p2p_respond_to_login_request, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_LOGIN_SUCCESS, _netemu_p2p_respond_to_login_success, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_USER_JOIN, netemu_p2p_respond_to_user_join, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, CREATE_P2P_GAME, _netemu_p2p_respond_to_game_created, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, JOIN_P2P_GAME, _netemu_p2p_respond_to_player_join, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_JOIN_HOST, _netemu_p2p_respond_to_join_host, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_GAME_START, netemu_p2p_respond_to_game_started, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_PLAYER_READY, netemu_p2p_respond_to_game_started, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_READY, netemu_p2p_respond_to_ready, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_BUFFERED_PLAY_VALUES, netemu_p2p_respond_to_play_values, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_CACHED_BUFFERED_PLAY_VALUES, netemu_p2p_respond_to_cached_play_values, p2p);
	netemu_receiver_buffer_add_instruction_received_fn(p2p->_internal->receive_buffer, P2P_PLAYER_JOIN_SUCCESS, _netemu_p2p_respond_to_player_join_success, p2p);
}

/**
 * This function responds to all "User Join" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_user_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int i;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if(netemu_list_contains(connection->_internal->users,user) == -1) {
		type.collection = connection->_internal->peers;
		netemu_list_add(connection->_internal->users,user);
		netemu_sender_buffer_add(connection->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);
		for (i = 0; i < connection->_internal->join_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->join_callbacks->elements[i])->fn.userJoinedFn(connection, user);
		}
	}
}

/**
 * This function responds to all "Ready" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_ready(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;

	connection = (struct netemu_p2p_connection*)arg;
	if(connection->_internal->continueFn != NULL) {
		connection->_internal->continueFn(connection);
	}

}

/**
 * This function responds to all "Game started" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_game_started(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_start_game *game;
	struct netemu_p2p_connection* connection;
	struct netemu_sender_udp *sender;
	netemu_sockaddr *addr;
	int i, size;
	NETEMU_BOOL user_joined;
	user_joined = 0;
	connection = (struct netemu_p2p_connection*)arg;
	if(connection->current_game == NULL || connection->current_game->started == 1 || connection->current_game->received_start_signal == TRUE) {
		return;
	}
	connection->current_game->received_start_signal = TRUE;
	connection->_internal->values_received = malloc((connection->current_game->emulator_value_size*
			connection->current_game->user_count)*connection->current_game->connection_quality);

	game = (struct p2p_start_game*)item->instruction->body;
	addr = (netemu_sockaddr*) netemu_util_create_addr(game->addr,game->port,&size);
	sender = netemu_sender_udp_new(addr, size);
	netemu_sender_collection_add_udp_sender(connection->current_game->_internal->udp_collection, sender);
	connection->current_game->_internal->ready_count++;

	if(connection->current_game->_internal->ready_count >= connection->current_game->user_count) {
		connection->current_game->started = 1;
		for(i = 0; i < connection->_internal->all_ready_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->all_ready_callbacks->elements[i])->fn.allReadyFn(connection, connection->current_game);
		}
	}
	if(item->instruction->id == P2P_PLAYER_READY) {
		for(i = 0; i < connection->_internal->player_ready_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->player_ready_callbacks->elements[i])->fn.gameStartedFn(connection, connection->current_game);
		}
	}
	else {
		for(i = 0; i < connection->_internal->game_started_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->game_started_callbacks->elements[i])->fn.gameStartedFn(connection, connection->current_game);
		}
	}


}

/**
 * This function responds to all "Join host" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_join_host(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct p2p_user *registered_user;
	int index;
	struct netemu_p2p_connection* connection;

	connection = (struct netemu_p2p_connection*)arg;
	user = (struct p2p_user*)item->instruction->body;
	if((index = netemu_list_contains(connection->_internal->users,user)) == -1) {
		return;
	}
	registered_user = connection->_internal->users->elements[index];
	if(registered_user->_internal == NULL) {
		registered_user->_internal = netemu_application_p2p_create_user_internal();
	}
	registered_user->_internal->connection = item->connection.connection;
}

/**
 * This function responds to all "player join" instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_player_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct application_instruction *join_success;
	struct netemu_p2p_connection* connection;
	struct netemu_tcp_connection *player_con;
	int index, i, size;
	union netemu_connection_type type;
	netemu_sockaddr* addr;
	connection = (struct netemu_p2p_connection*)arg;
	if(connection->current_game != NULL) {
		user = (struct p2p_user*)item->instruction->body;
		/* Do we have a connection to this player? */
		if((index = netemu_list_contains(connection->_internal->users,user)) != -1) {
			user->_internal = ((struct p2p_user*)connection->_internal->users->elements[index])->_internal;
		}
		else {
			/* If the user is not in the list, we shouldn't have received this instruction */
			return;
		}
		/* Does this player already exist in our list of players? In that case we don't need to do anything further. */
		if(_netemu_p2p_player_exists(connection->current_game,user)) {
			return;
		}
		/* If that's not the case, it's time to correct that. */
		if(user->_internal == NULL || (user->_internal->connection == NULL )) {
			addr = (netemu_sockaddr*)netemu_util_create_addr(user->addr, user->port, &size);
			player_con = _netemu_p2p_connect_to(connection,addr,  size);
			user->_internal = netemu_application_p2p_create_user_internal();
			user->_internal->connection = player_con;
			netemu_sender_collection_add_tcp_sender(connection->_internal->peers, player_con);
		}
		netemu_sender_collection_add_tcp_sender(connection->current_game->_internal->tcp_collection, user->_internal->connection);
		_netemu_p2p_add_player(connection->current_game,user);
		/* If this is our game, we send a join success back to the user TODO: Send denial if we're filled up. */
		if(_netemu_p2p_user_compare(connection->current_game->creator, connection->user) == 0) {
			type.connection = user->_internal->connection;
			join_success = netemu_application_instruction_create();
			netemu_application_p2p_player_join_success_add(join_success, connection->current_game);
			netemu_sender_buffer_add(connection->_internal->send_buffer, join_success, TCP_CONNECTION, type);
		}
		type.collection = connection->current_game->_internal->tcp_collection;
		netemu_sender_buffer_add(connection->_internal->send_buffer, item->instruction, CONNECTION_COLLECTION, type);
		for(i = 0; i < connection->_internal->player_joined_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->player_joined_callbacks->elements[i])->fn.playerJoinedFn(connection,
					connection->current_game, user);
		}
	}
}

void _netemu_p2p_respond_to_player_join_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_p2p_connection *connection;
	connection = arg;
	/* TODO: Fix some stuff here. */
}

/**
 * This function handles all incoming "Game Created"-instructions.
 * @ingroup p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_game_created(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_game *game, *copy;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int index, i;

	connection = (struct netemu_p2p_connection*)arg;
	game = (struct p2p_game*)item->instruction->body;
	/* We don't need to do anything if this game already exists in our list. */
	if(netemu_list_contains(connection->_internal->games,game) == -1) {
		/* If we have the creator in our list (which we should have), then we might be interested
		   in any internal data we have on that player. */
		if((index = netemu_list_contains(connection->_internal->users,game->creator)) != -1) {
			game->creator->_internal = ((struct p2p_user*)connection->_internal->users->elements[index])->_internal;
		}
		if(game->creator->_internal != NULL) {
			game->creator->_internal->player_no = 1;
		}
		/* Add the game to our game list. */
		netemu_list_add(connection->_internal->games,game);
		/* Create a copy of the game, to preserve the instruction integrity. */
		copy = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(copy, game);
		/* Replace the body in the instruction with the copy. */
		item->instruction->body = copy;
		/* Now let's send this to all other players we are connected to. */
		type.collection = connection->_internal->peers;
		netemu_sender_buffer_add(connection->_internal->send_buffer,item->instruction,CONNECTION_COLLECTION,type);

		/* Notify any listeners about the new game. */
		for(i = 0; i < connection->_internal->game_created_callbacks->count; i++) {
			((struct p2p_callback*)connection->_internal->game_created_callbacks->elements[i])->fn.gameCreatedFn(connection, game);
		}
	}
}

void netemu_p2p_respond_to_play_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;
	struct netemu_list *callbacks;
	struct p2p_buffered_play_values *values;
	int i;
	NETEMU_BOOL all_values_received;
	all_values_received = TRUE;
	connection = (struct netemu_p2p_connection*)arg;
	callbacks = connection->_internal->play_values_callbacks;
	values = item->instruction->body;
	netemu_thread_mutex_lock(connection->current_game->_internal->game_lock, NETEMU_INFINITE);
	if(values->player_no == 1) {
		netemu_list_add(connection->current_game->creator->_internal->play_values, item->instruction);
		connection->current_game->creator->_internal->values_received = TRUE;
	}
	else if(connection->current_game->creator->addr != connection->user->connection && connection->current_game->creator->port != connection->user->port && connection->current_game->creator->_internal->values_received == FALSE) {
		all_values_received = FALSE;
	}
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(connection->current_game->players[i]._internal->player_no == values->player_no && connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no) {
			netemu_list_add(connection->current_game->players[i]._internal->play_values, item->instruction);
			connection->current_game->players[i]._internal->values_received = TRUE;
		}
		else if(connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no && connection->current_game->players[i]._internal->values_received == FALSE) {
			all_values_received = FALSE;
		}
	}
	netemu_thread_mutex_release(connection->current_game->_internal->game_lock);
	if(all_values_received) {
		/* Signal anyone that waits for an event. */
		connection->current_game->_internal->all_values_received = TRUE;
		netemu_thread_event_signal(connection->_internal->play_values_event);
	}

	for(i = 0; i < callbacks->count; i++) {
		((struct p2p_callback*)callbacks->elements[i])->fn.valuesFn(connection, values->values, values->size);
	}
}

void netemu_p2p_respond_to_cached_play_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_p2p_connection* connection;
	int i;
	NETEMU_BOOL all_values_received;
	struct p2p_cached_buffered_play_values *values;
	all_values_received = TRUE;
	connection = (struct netemu_p2p_connection*)arg;
	values = item->instruction->body;
	netemu_thread_mutex_lock(connection->current_game->_internal->game_lock, NETEMU_INFINITE);
	if(values->player_no == 1) {
		netemu_list_add(connection->current_game->creator->_internal->play_values, item->instruction);
		connection->current_game->creator->_internal->values_received = TRUE;
	}
	else if(connection->current_game->creator->addr != connection->user->connection && connection->current_game->creator->port != connection->user->port && connection->current_game->creator->_internal->values_received == FALSE) {
		all_values_received = FALSE;
	}
	for(i = 0; i < connection->current_game->user_count-1; i++) {
		if(connection->current_game->players[i]._internal->player_no == values->player_no && connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no) {
			netemu_list_add(connection->current_game->players[i]._internal->play_values, item->instruction);
			connection->current_game->players[i]._internal->values_received = TRUE;
		}
		else if(connection->current_game->players[i]._internal->player_no != connection->user->_internal->player_no && connection->current_game->players[i]._internal->values_received == FALSE) {
			all_values_received = FALSE;
		}
	}
	netemu_thread_mutex_release(connection->current_game->_internal->game_lock);
	if(all_values_received) {
		/* Signal anyone that waits for an event. */
		connection->current_game->_internal->all_values_received = TRUE;
		netemu_thread_event_signal(connection->_internal->play_values_event);
	}
}

/**
 * This function responds to all "Login success" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void _netemu_p2p_respond_to_login_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
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
		netemu_list_add(info->_internal->users,user);

		/* The first user in the list is always the host, so lets add the contact information we know is here. */
		if(i == 0 && item->type == TCP_CONNECTION) {
			user->_internal = netemu_application_p2p_create_user_internal();
			user->_internal->connection = item->connection.connection;
		}
	}
	for(i = 0; i < success->games_count; i++) {
		game = malloc(sizeof(struct p2p_game));
		netemu_application_p2p_copy_game(game, &success->games[i]);
		netemu_list_add(info->_internal->users,user);
	}
	/* Notify any listeners about successful login. */
	for(i = 0; i < info->_internal->login_callbacks->count; i++) {
		((union p2p_callback_fn*)info->_internal->login_callbacks->elements[i])->connectFn(info, 1);
	}
}

/**
 * This function responds to all "Login request" instructions.
 * @ingroup netemu_p2p
 * @ingroup netemu_p2p_responses
 * @param buffer the packet buffer which contained the instruction
 * @param item the item in the instruction that triggered the event.
 * @param arg if not null, this is the argument that was passed by the player when
 * adding this function as a listener.
 */
void netemu_p2p_respond_to_login_request(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct p2p_user *user;
	struct netemu_p2p_connection* connection;
	union netemu_connection_type type;
	int i;
	connection = (struct netemu_p2p_connection *)arg;
	type.collection = connection->_internal->peers;
	user = malloc(sizeof(struct p2p_user));
	netemu_application_p2p_copy_user(user,(struct p2p_user*)item->instruction->body);
	netemu_util_copy_string(&user->name, item->instruction->user);
	user->_internal = netemu_application_p2p_create_user_internal();
	switch(item->type) {
		case TCP_CONNECTION:
			user->_internal->connection = item->connection.connection;
			break;
		case UDP_SENDER:
			user->_internal->sender = item->connection.udp_sender;
			break;
		default:
			break;
	}

	_netemu_p2p_send_login_success(connection,item->connection.connection);
	netemu_p2p_send_user_joined(connection, user);
	netemu_list_add(connection->_internal->users,user);

	/* Notify listeners about successful join */
	for(i = 0; i < connection->_internal->join_callbacks->count; i++) {
		((struct p2p_callback*)connection->_internal->join_callbacks->elements[i])->fn.userJoinedFn(connection, user);
	}
}

