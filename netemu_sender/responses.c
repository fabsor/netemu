/*
 * responses.c
 *
 *  Created on: 27 apr 2010
 *      Author: fabian
 */

#include "netemu_server_connection.h"
#include "responses.h"
#include "netemu_list.h"

void _netemu_respond_to_login_success(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct login_success *accepted;
	struct server_connection* connection;
	int i;
	connection = (struct server_connection*)arg;
	accepted = (struct login_success*)instruction->body;
	for(i = 0; i < accepted->users_count; i++) {
		_server_connection_add_user_struct(connection,accepted->users[i]);
	}
	for(i = 0; i < accepted->games_count; i++) {
		_server_connection_add_game_struct(connection,accepted->games[i]);
	}
}

void _netemu_respond_to_player_joined(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg) {
	struct player_joined *joined;
	struct player* player;
	struct server_connection *connection;
	int i;
	if((player = malloc(sizeof(struct player))) == NULL) {
		return;
	}
	connection = (struct server_connection*)arg;
	joined = (struct player_joined*)instruction->body;

	player->connection = joined->connection;
	player->id = joined->user_id;
	player->username = malloc(sizeof(char)*(strlen(joined->username)+1));
	strcpy(player->username,joined->username);
	player->ping = joined->ping;

	if(connection->current_game != NULL) {
		if(connection->current_game->id == joined->game_id) {
			server_connection_add_player(connection->current_game, player);
			return;
		}
	}
	else {
		/* TODO: Better solution for this. */
		for(i = 0; i < connection->_internal->games->count; i++) {
			if(((struct game*)connection->_internal->games->elements[i])->id == joined->game_id) {
				server_connection_add_player((struct game*)(connection->_internal->games->elements[i]),player);
				return;
			}
		}
	}
	free(player->username);
	free(player);
}

void _netemu_respond_to_game_created(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct game_created *created;
	struct server_connection* connection;
	int* itemsToRemove, i, j;

	j = 0;
	connection = (struct server_connection*)arg;
	itemsToRemove = malloc(sizeof(int)*connection->_internal->game_created_callback->count);
	created = (struct game_created*)instruction->body;
	server_connection_add_game(connection, created->appName, created->gameName, created->id, 0, 0);
	for(i = 0; i < connection->_internal->game_created_callback->count; i++) {
		((struct callback*)connection->_internal->game_created_callback->elements[i])->fn->game_created_fn(connection->_internal->games->elements[connection->_internal->games->count-1]);
		if(((struct callback*)connection->_internal->game_created_callback->elements[i])->disposable) {
			itemsToRemove[j] = i;
			j++;
		}
	}
	for(i = 0; i < j+1; i++) {
		netemu_list_remove(connection->_internal->game_created_callback, itemsToRemove[j]);
	}
}

void _netemu_respond_to_ping(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct application_instruction* pong;
	struct server_connection* connection;
	connection = (struct server_connection*)arg;
	pong = netemu_application_create_message();
	netemu_application_pong_add(pong);
	pong->important = 1;
	netemu_sender_buffer_add(connection->_internal->send_buffer,pong);
}

void _netemu_respond_to_user_join(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct user_joined *joined;
	struct server_connection* connection;

	connection = (struct server_connection*)arg;
	joined = (struct user_joined*)instruction->body;

	/* If signed_in is 0, we have just logged on to the server and
	 * have to assume that this user_joined instruction refers to our local player */
	if(!connection->has_id) {
		connection->player_id = joined->id;
		connection->has_id = 1;
	}

	server_connection_add_user(connection, joined->id, joined->connection, instruction->user);

}

void _netemu_respond_to_game_status_update(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg) {
	struct server_connection* connection;
	struct game *game;
	struct game_status_update *update;
	int i;
	game = NULL;
	connection = (struct server_connection*)arg;
	update = (struct game_status_update*)instruction->body;
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
		game->status = update->status;
	}
}

void _netemu_respond_to_player_list(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct server_connection* connection;
	struct existing_player_list *list, *body;
	int i;
	connection = (struct server_connection*)arg;
	body = (struct existing_player_list*)instruction->body;
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

void _netemu_respond_to_buffered_values(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct server_connection* connection;
	struct buffered_play_values* values;
	struct netemu_list *callbacks;
	int i;

	connection = (struct server_connection*)arg;
	callbacks = connection->_internal->play_values_callback;
	values = (struct buffered_play_values*)instruction->body;
	connection->_internal->buffered_values->size = values->size;
	if(connection->_internal->buffered_values->values !=NULL)
		free(connection->_internal->buffered_values->values);
	connection->_internal->buffered_values->values = malloc(values->size);
	memcpy(connection->_internal->buffered_values->values, values->values, values->size);

	for(i = 0; i < callbacks->count; i++) {
		((struct callback*)callbacks->elements[i])->fn->valuesReceivedFn(values);
	}
}

void _netemu_respond_to_chat(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct server_connection* connection;
	struct chat *chat;
	union callback_fn* fn;
	int i;
	connection = (struct server_connection*)arg;
	chat = (struct chat*)instruction->body;
	for(i = 0; i< connection->_internal->chat_callback->count; i++) {
		fn = (union callback_fn*)connection->_internal->chat_callback->elements[i];
		fn->chat_fn(instruction->user,chat->message);
	}
}

