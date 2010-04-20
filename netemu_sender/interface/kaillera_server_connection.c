
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
void server_connection_add_game(struct server_connection *connection, char* app_name, NETEMU_WORD id, char status, int users_count);
void server_connection_respond_to_game_created(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg);
void server_connection_respond_to_player_joined(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg);
struct _server_connection_internal {
	struct netemu_list *chat_callback;
	struct netemu_list *join_callback;
	struct netemu_list *leave_callback;
	struct netemu_list *game_created_callback;
	NETEMU_HASHTBL *users;
	NETEMU_HASHTBL *games;
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
	netemu_list_add(connection->_internal->chat_callback, fn);
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
	netemu_list_add(connection->_internal->join_callback, fn);
	return 0;
}

int server_connection_unregister_user_join_callback(struct server_connection *connection, joinFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->join_fn = callback;
	return netemu_list_remove(connection->_internal->join_callback, fn);
}

int server_connection_register_user_leave_callback(struct server_connection *connection, leaveFn callback) {
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->leave_fn = callback;
	netemu_list_add(connection->_internal->leave_callback, fn);
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
	int error;
	time_t timestamp;
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);

	timestamp = time(NULL);
	netemu_sender_buffer_add(connection->_internal->send_buffer,message);
	reply = netemu_packet_buffer_wait_for_instruction(connection->_internal->receive_buffer, CREATE_GAME, timestamp);
	*result = (struct game*)reply->body;
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
	connection->_internal->receive_buffer = netemu_packet_buffer_new(100);
	connection->_internal->send_buffer = netemu_sender_buffer_new(5,10);
	connection->_internal->users = netemu_hashtbl_create(10,def_hashfunc_int, comparator_int);
	connection->_internal->games = netemu_hashtbl_create(10,def_hashfunc_int, comparator_int);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,PING,respondToPing, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,USER_JOINED,server_connection_respond_to_user_join, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,LOGIN_SUCCESS,server_connection_respond_to_login_success, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer, PLAYER_JOINED, server_connection_respond_to_player_joined, connection);
	netemu_packet_buffer_add_instruction_received_fn(connection->_internal->receive_buffer,CREATE_GAME,server_connection_respond_to_game_created, connection);
	netemu_receiver_udp_register_recv_fn(netemu_resources_get_receiver(), _server_connection_receive, connection);
	server_connection_login(connection);
	return connection;
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
	netemu_application_free_message(success);
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
	//netemu_application_free_message(instruction);
}

void server_connection_respond_to_login_success(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct login_success *accepted;
	struct server_connection* connection;
	int i;
	connection = (struct server_connection*)arg;
	accepted = (struct login_success*)instruction->body;
	for(i = 0; i < accepted->users_count; i++) {
		netemu_hashtbl_insert(connection->_internal->users,&accepted->users[i]->id,sizeof(NETEMU_WORD),accepted->users[i]);
	}
	for(i = 0; i < accepted->games_count; i++) {
		netemu_hashtbl_insert(connection->_internal->games,&accepted->games[i]->id,sizeof(NETEMU_WORD),accepted->games[i]);
	}
	//netemu_application_free_message(instruction);
}

void server_connection_respond_to_player_joined(struct netemu_packet_buffer *buffer, struct application_instruction *instruction, void *arg) {
	struct player_joined *joined;
	struct server_connection *connection;

	connection = (struct server_connection*)arg;
	joined = (struct player_joined*)instruction->body;

	if(connection->current_game != NULL) {
		if(connection->current_game->id == joined->game_id) {
			//joined->
		}
	}
}

void server_connection_respond_to_game_created(struct netemu_packet_buffer* buffer, struct application_instruction *instruction, void* arg) {
	struct game_created *created;
	struct server_connection* connection;
	connection = (struct server_connection*)arg;
	created = (struct game_created*)instruction->body;
	server_connection_add_game(connection,created->appName, created->id,0, 0);
}

void server_connection_add_game(struct server_connection *connection, char* app_name, NETEMU_WORD id, char status, int users_count) {
	struct game* game;
	game = malloc(sizeof(struct game));
	game->app_name = app_name;
	game->id = id;
	game->status = status;
	game->users_count = users_count;
	netemu_hashtbl_insert(connection->_internal->games,&game->id,sizeof(NETEMU_WORD),game);
}

void server_connection_add_user(struct server_connection* connection, NETEMU_WORD user_id, char connection_type, char* username) {
	struct user* user;
	user = malloc(sizeof(struct user*));
	user->id = user_id;
	user->connection = connection_type;
	user->username = username;

	netemu_hashtbl_insert(connection->_internal->users,&user->id,sizeof(NETEMU_WORD),user);
}

struct game* server_connection_get_game_list(struct server_connection* connection, int *count) {
	struct game** games;
	struct game* game;
	struct netemu_hashtable_iter *iter;
	int i = 0;
	games = malloc(sizeof(struct game*)*connection->_internal->games->count);
	iter = netemu_hashtbl_iterator_new(connection->_internal->games);
	while(game = (struct game*)netemu_hashtbl_iterator_next(iter) != NULL) {
		games[i] = game;
		i++;
	}
	*count = connection->_internal->games->count;
	//netemu_hashtbl_iterator_free(iter);
	return game;
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
