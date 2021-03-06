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
 * This file contains instruction responder functions in the netemu_kaillera module.
 */

#include "kaillera_internal.h"
#include "kaillera_responses.h"
#include "../network/sender_buffer.h"
#include "../structures/netemu_list.h"
#include "../interface/netemu_kaillera.h"

void _netemu_respond_to_login_success(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct login_success *accepted;
	struct netemu_kaillera* connection;
	int i;
	connection = (struct netemu_kaillera*)arg;
	accepted = (struct login_success*)item->instruction->body;
	for(i = 0; i < accepted->users_count; i++) {
		_netemu_kaillera_add_user_struct(connection,accepted->users[i]);
	}
	for(i = 0; i < accepted->games_count; i++) {
		_netemu_kaillera_add_game_struct(connection,accepted->games[i]);
	}
}

void _netemu_respond_to_player_joined(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct player_joined *joined;
	struct player* player;
	struct netemu_kaillera *connection;
	struct callback *call;
	int i;
	if((player = malloc(sizeof(struct player))) == NULL) {
		return;
	}
	connection = (struct netemu_kaillera*)arg;
	joined = (struct player_joined*)item->instruction->body;

	player->connection = joined->connection;
	player->id = joined->user_id;
	player->username = malloc(sizeof(char)*(strlen(joined->username)+1));
	strcpy(player->username,joined->username);
	player->ping = joined->ping;

	if(connection->current_game != NULL) {
		if(connection->current_game->id == joined->game_id) {
			netemu_kaillera_add_player(connection->current_game, player);
			return;
		}
	}
	else {
		/* TODO: Better solution for this. */
		for(i = 0; i < connection->_internal->games->count; i++) {
			if(((struct game*)connection->_internal->games->elements[i])->id == joined->game_id) {
				netemu_kaillera_add_player((struct game*)(connection->_internal->games->elements[i]),player);
				return;
			}
		}
	}
	for(i = 0; i < connection->_internal->player_join_callback->count; i++) {
		call = connection->_internal->player_join_callback->elements[i];
		call->fn->player_join_fn(connection, joined);
		if(call->disposable)
			netemu_list_add(connection->_internal->callbacks_to_remove, call);
	}
	for(i = 0; i < connection->_internal->callbacks_to_remove->count; i++) {
		netemu_list_remove(connection->_internal->player_join_callback, call);
	}
	netemu_list_clear(connection->_internal->callbacks_to_remove);
	free(player->username);
	free(player);
}

void _netemu_respond_to_game_created(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct game_created *created;
	struct game *game;
	struct netemu_kaillera* connection;
	struct callback *call;
	int* itemsToRemove, i, j;

	j = 0;
	connection = (struct netemu_kaillera*)arg;
	itemsToRemove = malloc(sizeof(int)*connection->_internal->game_created_callback->count);
	created = (struct game_created*)item->instruction->body;
	netemu_kaillera_add_game(connection, created->appName, created->gameName, created->id, 0, 0, item->instruction->user);
	for(i = 0; i < connection->_internal->game_created_callback->count; i++) {
		call = (struct callback*)connection->_internal->game_created_callback->elements[i];
		game = connection->_internal->games->elements[connection->_internal->games->count-1];
		call->fn->game_created_fn(connection, game, call->user_data);
		if(((struct callback*)connection->_internal->game_created_callback->elements[i])->disposable) {
			itemsToRemove[j] = i;
			j++;
		}
	}
	for(i = 0; i < j; i++) {
		netemu_list_remove(connection->_internal->game_created_callback, itemsToRemove[i]);
	}
}

void _netemu_respond_to_ping(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct application_instruction* pong;
	struct netemu_kaillera* connection;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	connection = (struct netemu_kaillera*)arg;
	pong = netemu_application_instruction_create();
	netemu_application_pong_add(pong);
	pong->important = 1;
	netemu_sender_buffer_add(connection->_internal->send_buffer,pong,UDP_SENDER,type);
}

void _netemu_respond_to_game_started(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	int i;
	struct netemu_kaillera* connection;
	struct game_start *start;
	struct callback *call;
	connection = (struct netemu_kaillera*)arg;
	start = item->instruction->body;
	connection->_internal->time_band = start->time_band;
	connection->_internal->player_no = start->player_no;
	for(i = 0; i < connection->_internal->game_started_callbacks->count; i++)
	{
		call = connection->_internal->game_started_callbacks->elements[i];
		call->fn->game_started_fn(connection, connection->current_game, start, call->user_data);
		if(call->disposable)
			netemu_list_add(connection->_internal->callbacks_to_remove, call);
	}
	for (i = 0; i < connection->_internal->callbacks_to_remove->count; i++) {
		netemu_list_remove(connection->_internal->game_started_callbacks, call);
	}
	netemu_list_clear(connection->_internal->game_started_callbacks);
}

void _netemu_kaillera_remove_callback(struct netemu_receiver_buffer *buffer) {

}

void _netemu_respond_to_user_join(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct user_joined *joined;
	struct netemu_kaillera* connection;
	struct netemu_list *callbacks;
	struct callback *call;
	int i;

	connection = (struct netemu_kaillera*)arg;
	joined = (struct user_joined*)item->instruction->body;
	callbacks = connection->_internal->join_callback;

	/* If signed_in is 0, we have just logged on to the server and
	 * have to assume that this user_joined instruction refers to our local player */
	if(!connection->_internal->has_id) {
		connection->player_id = joined->id;
		connection->_internal->has_id = 1;
	}

	netemu_kaillera_add_user(connection, joined->id, joined->connection, item->instruction->user);
	for(i = 0; i < callbacks->count; i++)
	{
		call = netemu_list_get(callbacks, i);
		call->fn->join_fn(connection, item->instruction->user, joined->ping, joined->id, call->user_data);
	}

}

void _netemu_respond_to_user_leave(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct user_left *left;
	struct netemu_kaillera* connection;
	struct netemu_list *callbacks;
	struct callback *call;
	int i;

	connection = (struct netemu_kaillera*)arg;
	left = (struct user_left*)item->instruction->body;
	netemu_kaillera_remove_user(connection, left->id);

	for(i = 0; i < connection->_internal->leave_callback->count; i++)
	{
		call = netemu_list_get(connection->_internal->leave_callback, i);
		call->fn->leave_fn(left->id, item->instruction->user, left->exit_message, call->user_data);
	}
}

void _netemu_respond_to_player_ready(struct netemu_receiver_buffer *buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_kaillera* connection;
	struct game *game;
	struct callback *call;
	int i;
	game = NULL;
	connection = (struct netemu_kaillera*)arg;

	for(i = 0; i < connection->_internal->player_ready_callback->count; i++) {
		call = connection->_internal->player_ready_callback->elements[i];
		call->fn->player_ready_fn(connection);
		if(call->disposable) {
			netemu_list_add(connection->_internal->callbacks_to_remove,
					connection->_internal->player_ready_callback->elements[i]);
		}
	}
	for(i = 0; i < connection->_internal->callbacks_to_remove->count; i++) {
		netemu_list_remove(connection->_internal->player_ready_callback->elements,
							connection->_internal->callbacks_to_remove->elements[i]);
	}

}

void _netemu_respond_to_game_status_update(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_kaillera* connection;
	struct game *game;
	struct game_status_update *update;
	struct callback *call;
	int i;
	game = NULL;

	connection = (struct netemu_kaillera*)arg;
	update = (struct game_status_update*)item->instruction->body;
	if(connection->current_game != NULL && connection->current_game->id == update->id) {
		game = connection->current_game;
	}
	for(i = 0; i < connection->_internal->games->count; i++) {
		if(((struct game*)connection->_internal->games->elements[i])->id == update->id) {
			game = (struct game*)connection->_internal->games->elements[i];
			break;
		}
	}
	if(game != NULL) {
		game->users_count = update->num_players;
		game->player_count = update->num_players;
		game->status = update->status;
	}
	for(i = 0; i< connection->_internal->game_status_updated_callbacks->count; i++) {
		call = (struct callback*)netemu_list_get(connection->_internal->game_status_updated_callbacks, i);
		call->fn->status_update_fn(connection, game, update, call->user_data);
	}
}

void _netemu_respond_to_player_list(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_kaillera* connection;
	struct existing_player_list *list, *body;
	int i;
	connection = (struct netemu_kaillera*)arg;
	body = (struct existing_player_list*)item->instruction->body;
	list = malloc(sizeof(struct existing_player_list));
	list->players = malloc(sizeof(struct player)*body->players_count);
	list->players_count = body->players_count;
	for (i = 0; i < list->players_count; i++) {
		list->players[i].username = malloc(strlen(body->players[i].username)+1);
		strcpy(list->players[i].username,body->players[i].username);
		list->players[i].connection = body->players[i].connection;
		list->players[i].id = body->players[i].id;
		list->players[i].ping = body->players[i].ping;
	}

	if (connection->current_game != NULL) {
		connection->current_game->players = list;
	}
}

void _netemu_respond_to_buffered_values(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_kaillera* connection;
	struct buffered_play_values* values;
	struct netemu_list *callbacks;
	int i;
	struct callback *call;

	connection = (struct netemu_kaillera*)arg;
	callbacks = connection->_internal->play_values_callback;
	values = (struct buffered_play_values*)item->instruction->body;
	netemu_list_add(connection->_internal->received_play_values, item->instruction);
	netlib_thread_event_signal(connection->_internal->play_values_event);
	for(i = 0; i < callbacks->count; i++) {
		call = (struct callback*)callbacks->elements[i];
		call->fn->values_received_fn(connection, values, call->user_data);
	}
}

void _netemu_respond_to_chat(struct netemu_receiver_buffer* buffer, struct netemu_receiver_buffer_item *item, void* arg) {
	struct netemu_kaillera* connection;
	char *chat;
	struct callback *call;
	int i;
	connection = (struct netemu_kaillera*)arg;
	/* TODO: Apparently, the body here does not point to a struct chat as it should, but rather directly to the chat message. */
	chat = (char*)item->instruction->body;
	for(i = 0; i< connection->_internal->chat_callback->count; i++) {
		call = (struct callback*)netemu_list_get(connection->_internal->chat_callback, i);
		call->fn->chat_fn(item->instruction->user, chat, call->user_data);
	}
}

void _netemu_respond_to_cached_play_values(struct netemu_receiver_buffer *buffer, struct netemu_receiver_buffer_item *item, void *arg) {
	struct netemu_kaillera* connection;
	struct callback *call;
	struct intelligently_cached_buffered_play_values *cache;
	int i;
	connection = (struct netemu_info*)arg;

	netemu_list_add(connection->_internal->received_play_values, item->instruction);

	cache = (struct intelligently_cached_buffered_play_values*)item->instruction->body;
	netlib_thread_event_signal(connection->_internal->play_values_event);
	for(i = 0; i < connection->_internal->cached_values_callback->count; i++) {
		call = (struct callback*)netemu_list_get(connection->_internal->cached_values_callback, i);
		call->fn->cached_values_received_fn(cache, call->user_data);
	}
}
