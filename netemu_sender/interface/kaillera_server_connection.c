﻿
#include <stdio.h>
#include "kaillera_server_connection.h"
#include "../protocol/application.h"
#include "../netemu_resources.h"
#include "../network/netemu_packet_buffer.h"
#include "../network/netemu_sender_buffer.h"
#include "../network/netemu_sender.h"
#include "../network/netemu_receiver.h"
#include "../structures/netemu_hashtbl.h"
#include "netemu_list.h"
#include "netemu_thread.h"

void _server_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params);
int server_connection_login(struct server_connection* connection);
int server_connection_wait_for_instruction(struct server_connection* connection, int instruction_id, time_t timestamp);
void respondToPing(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_respond_to_user_join(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_add_user(struct server_connection* connection, NETEMU_WORD user_id, char connection_type, char* username);
void server_connection_respond_to_login_success(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_add_game(struct server_connection *connection, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count);
void server_connection_respond_to_game_created(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_respond_to_player_joined(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg);
void server_connection_respond_to_buffered_values(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_respond_to_player_list(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_respond_to_game_status_update(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg);
void server_connection_add_player(struct game *game, struct player *player);

int _server_connection_user_comparator(const void* item1,const void* item2);
int _server_connection_game_comparator(const void* item1, const void* item2);
void _server_connection_add_user_struct(struct server_connection* connection, struct user *user);
void _server_connection_add_game_struct(struct server_connection* connection, struct game* game);
int _server_connection_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable);

struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	struct netemu_list *play_values_callback;
	struct netemu_list *users;
	struct netemu_list *games;
	short game_create_requested;
	struct buffered_play_values *buffered_values;
	struct netemu_packet_buffer *receive_buffer;
	struct netemu_sender_buffer *send_buffer;

};

int server_connection_send_chat_message(struct server_connection *connection, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();

	return 1;
}

int server_connection_register_chat_callback(struct server_connection *connection, chatFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	_server_connection_register_callback(connection->_internal->chat_callback, fn, 0);
	return 0;
}

int server_connection_unregister_chat_callback(struct server_connection *connection, chatFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->chat_fn = callback;
	return netemu_list_remove(connection->_internal->chat_callback, fn);
}

int server_connection_register_user_join_callback(struct server_connection *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;
	_server_connection_register_callback(connection->_internal->join_callback, fn, 0);
	return 0;
}

int _server_connection_register_callback(struct netemu_list *list, union callback_fn *fn, int disposable) {
	struct callback *callback;

	if((callback = malloc(sizeof(struct callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->disposable = disposable;
	netemu_list_add(list,callback);
	return 0;
}

int _server_connection_unregister_callback(struct netemu_list *list, union callback_fn *fn) {
	struct callback *callback;

	if((callback = malloc(sizeof(struct callback))) == NULL ) {
		return -1;
	}
	callback->fn = fn;
	callback->disposable = 0;
	return netemu_list_remove(list,callback);

}

int server_connection_register_play_values_received_callback(struct server_connection *connection, valuesReceivedFn callback) {
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}
	fn->valuesReceivedFn = callback;
	_server_connection_register_callback(connection->_internal->play_values_callback, fn, 0);
	return 0;
}

int server_connection_unregister_play_values_callback(struct server_connection *connection, joinFn callback) {
	int error;
	union callback_fn *fn;
	if((fn = malloc(sizeof(union callback_fn))) == NULL) {
		return -1;
	}
	fn->join_fn = callback;
	error = netemu_list_remove(connection->_internal->join_callback, fn);
	free(fn);

	return error;
}


int server_connection_unregister_user_join_callback(struct server_connection *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;
	return _server_connection_unregister_callback(connection->_internal->join_callback, fn);
}

int server_connection_register_user_leave_callback(struct server_connection *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	_server_connection_register_callback(connection->_internal->leave_callback, fn, 0);
	return 0;
}


int server_connection_unregister_user_leave_callback(struct server_connection *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	return netemu_list_remove(connection->_internal->leave_callback, fn);
}

int server_connection_disconnect(struct server_connection *connection, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];

	client = netemu_resources_get_client();
	client->sender->addr = connection->addr;
	messages[0] = netemu_application_create_message();
	netemu_application_user_leave_add(messages[0], message);
	buffer = netemu_transport_pack(messages,1);
	return netemu_sender_udp_send(client->sender ,buffer.data, buffer.size);
}


int server_connection_create_game(struct server_connection *connection, char *gamename, struct game** result) {
	time_t timestamp;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);

	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	reply = netemu_packet_buffer_wait_for_instruction(connection->_internal->receive_buffer, CREATE_GAME, timestamp);
	*result = (struct game*)reply->body;
	return 1;
}

void server_connection_create_game_async(struct server_connection *connection, char *gamename, gameCreatedFn callback) {
	time_t timestamp;
	struct application_instruction *message, *reply;
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->game_created_fn = callback;
	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);
	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	_server_connection_register_callback(connection->_internal->game_created_callback, fn, 1);
}


void _server_connection_add_game_struct(struct server_connection* connection, struct game* game) {
	int index;

	if(connection->_internal->game_create_requested) {
		connection->current_game = game;
		connection->_internal->game_create_requested = 0;
	}

	index = netemu_list_contains(connection->_internal->games,game);
	if(index != -1) {
		/* TODO: Free memory here... */
		connection->_internal->games->elements[index] = game;
	}
	else {
		netemu_list_add(connection->_internal->games,game);
	}
}

int server_connection_start_game(struct server_connection *connection) {
	time_t timestamp;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_start_game_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	message->important = 1;
	reply = netemu_packet_buffer_wait_for_instruction(connection->_internal->receive_buffer, START_GAME, timestamp);
	return 1;
}


struct server_connection *server_connection_new(char* user, char* emulator_name) {
	struct server_connection *connection;

	connection = (struct server_connection*)malloc(sizeof(struct server_connection));
	if(connection == NULL) {
		return NULL;
	}
	connection->user = user;
	connection->emulator_name = emulator_name;
	connection->current_game = NULL;
	connection->has_id = 0;
	connection->_internal = malloc(sizeof(struct _server_connection_internal));
	connection->_internal->chat_callback = netemu_list_new(3);
	connection->_internal->game_created_callback = netemu_list_new(3);
	connection->_internal->join_callback = netemu_list_new(3);
	connection->_internal->leave_callback = netemu_list_new(3);
	connection->_internal->play_values_callback = netemu_list_new(3);
	connection->_internal->receive_buffer = netemu_packet_buffer_new(100);
	connection->_internal->send_buffer = netemu_sender_buffer_new(5,10);
	connection->_internal->buffered_values = malloc(sizeof(struct buffered_play_values));
	connection->_internal->buffered_values->values = NULL;
	connection->_internal->game_create_requested = 1;
	connection->_internal->users = netemu_list_new(10);
	netemu_list_register_sort_fn(connection->_internal->users,_server_connection_user_comparator);
	connection->_internal->games = netemu_list_new(10);
	netemu_list_register_sort_fn(connection->_internal->games,_server_connection_game_comparator);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,PING,respondToPing, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,USER_JOINED,server_connection_respond_to_user_join, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,LOGIN_SUCCESS,server_connection_respond_to_login_success, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer, PLAYER_JOINED, server_connection_respond_to_player_joined, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,CREATE_GAME,server_connection_respond_to_game_created, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,BUFFERED_PLAY_VALUES,server_connection_respond_to_buffered_values, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,EXISTING_PLAYERS_LIST,server_connection_respond_to_player_list, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,GAME_STATUS_UPDATE,server_connection_respond_to_game_status_update, connection);
	netemu_receiver_udp_register_recv_fn(netemu_resources_get_receiver(), _server_connection_receive, connection);
	server_connection_login(connection);
	return connection;
}

void server_connection_respond_to_game_status_update(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg) {
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

void server_connection_respond_to_player_list(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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

void server_connection_respond_to_buffered_values(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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
		((union callback_fn*)callbacks->elements[i])->valuesReceivedFn(values);
	}
}

int server_connection_login(struct server_connection* connection) {
	struct application_instruction *message;
	struct application_instruction *success;
	struct netemu_sender_udp *sender;
	time_t timestamp;

	sender = netemu_resources_get_sender();
	message = netemu_application_create_message();

	netemu_application_login_request_add(message,connection->emulator_name,connection->user,1);
	message->important = 1;

	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	success = netemu_packet_buffer_wait_for_instruction(connection->_internal->receive_buffer, LOGIN_SUCCESS, timestamp);
	return 1;
}

void server_connection_respond_to_chat(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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



void respondToPing(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct application_instruction* pong;
	struct server_connection* connection;
	connection = (struct server_connection*)arg;
	pong = netemu_application_create_message();
	netemu_application_pong_add(pong);
	pong->important = 1;
	netemu_sender_buffer_add(connection->_internal->send_buffer,pong);
}

void server_connection_respond_to_user_join(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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

int server_connection_join_game(struct server_connection *connection, NETEMU_DWORD gameid) {
	struct application_instruction* message;
	time_t timestamp;
	message = netemu_application_create_message();
	timestamp = time(NULL);
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	netemu_packet_buffer_wait_for_instruction(connection->_internal->receive_buffer,EXISTING_PLAYERS_LIST, timestamp);
	return 1;
}

int server_connection_join_game_async(struct server_connection *connection, NETEMU_DWORD gameid) {
	struct application_instruction* message;

	message = netemu_application_create_message();
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);

	return 1;
}


void server_connection_respond_to_login_success(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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

void server_connection_respond_to_player_joined(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg) {
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

void server_connection_add_player(struct game *game, struct player *player) {
	struct player* players;
	int i;

	game->players->players_count++;
	if((game->players->players = realloc(game->players->players,game->players->players_count)) == NULL) {
		players = malloc(sizeof(struct player)*game->players->players_count);
		for(i = 0; i < game->players->players_count-1; i++) {
			players[i] = game->players->players[i];
		}
		free(game->players->players);
		game->players->players = players;
	}
	game->players->players[game->players->players_count] = *player;

}

void server_connection_respond_to_game_created(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
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

void server_connection_add_game(struct server_connection *connection, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count) {
	struct game* game;
	game = malloc(sizeof(struct game));
	game->app_name = app_name;
	game->name = game_name;
	game->id = id;
	game->status = status;
	game->users_count = users_count;
	_server_connection_add_game_struct(connection, game);
}

void server_connection_add_user(struct server_connection* connection, NETEMU_WORD user_id, char connection_type, char* username) {
	struct user* user;
	user = malloc(sizeof(struct user*));
	user->id = user_id;
	user->connection = connection_type;
	user->username = username;
	_server_connection_add_user_struct(connection,user);
}

void _server_connection_add_user_struct(struct server_connection* connection, struct user *user) {
	int index;

	index = netemu_list_contains(connection->_internal->users,user);
	if(index != -1) {
		/* TODO: Free memory here... */
		connection->_internal->users->elements[index] = user;
	}
	else {
		netemu_list_add(connection->_internal->users,user);
	}
}

struct game** server_connection_get_game_list(struct server_connection* connection, int *count) {
	struct game** games;
	netemu_list_copy(connection->_internal->games,&games);
	*count = connection->_internal->games->count;
	return games;
}

struct user** server_connection_get_user_list(struct server_connection* connection, int *count) {
	struct user** users;
	netemu_list_copy(connection->_internal->users,&users);
	*count = connection->_internal->users->count;
	return users;
}

void _server_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params) {
	struct server_connection* connection;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	int i;
	connection = (struct server_connection*) params;
	packet = netemu_transport_unpack(data);
	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		if(instruction->id == CREATE_GAME) {
			printf("GAME CREATED");
		}
		netemu_packet_buffer_add(connection->_internal->receive_buffer,instruction);
	}
}

/**
 * Compare users.
 */
int _server_connection_user_comparator(const void* item1,const void* item2) {
	return ((struct user*)item1)->id - ((struct user*)item2)->id;
}
/**
 * Compare games.
 */
int _server_connection_game_comparator(const void* item1, const void* item2) {
	return ((struct game*)item1)->id - ((struct game*)item2)->id;
}

struct buffered_play_values* server_connection_get_play_values(struct server_connection *connection) {
	return connection->_internal->buffered_values;
}

void server_connection_send_play_values(struct server_connection* connection, int size, void* data) {
	int error;
	time_t timestamp;
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_buffered_play_values_add(message,size,data);
	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	return 1;
}

int server_connection_send_player_ready(struct server_connection *connection) {
	int error;
	time_t timestamp;
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_player_ready_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	return 1;
}
