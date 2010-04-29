
#include <stdio.h>
#include "netemu_server_connection.h"
#include "protocol/application.h"
#include "netemu_resources.h"
#include "network/netemu_packet_buffer.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_sender_udp.h"
#include "network/netemu_receiver.h"
#include "structures/netemu_hashtbl.h"
#include "netemu_list.h"
#include "netemu_thread.h"
#include "responses.h"


int server_connection_wait_for_instruction(struct netemu_info* info, int instruction_id, time_t timestamp);
void server_connection_add_user(struct netemu_info* info, NETEMU_WORD user_id, char connection_type, char* username);
void server_connection_add_game(struct netemu_info *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count);
void server_connection_add_player(struct game *game, struct player *player);

void _server_connection_add_game_struct(struct netemu_info* info, struct game* game);
int _server_connection_user_comparator(const void* item1,const void* item2);
int _server_connection_game_comparator(const void* item1, const void* item2);
void _server_connection_add_user_struct(struct netemu_info* info, struct user *user);


int netemu_send_chat_message(struct netemu_info *info, char *message) {
	/*TODO: Implemen chat message! */
	/*
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	*/
	return 1;
}

int netemu_disconnect(struct netemu_info *info, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *messages[1];

	client = netemu_resources_get_client();
	client->sender->addr = info->addr;
	messages[0] = netemu_application_create_message();
	netemu_application_user_leave_add(messages[0], message);
	buffer = netemu_transport_pack(messages,1);
	return netemu_sender_udp_send(client->sender ,buffer.data, buffer.size);
}


int netemu_create_game(struct netemu_info *info, char *gamename, struct game** result) {
	time_t timestamp;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);

	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	reply = netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, CREATE_GAME, timestamp);
	*result = (struct game*)reply->body;
	return 1;
}

void netemu_create_game_async(struct netemu_info *info, char *gamename, gameCreatedFn callback) {
	time_t timestamp;
	struct application_instruction *message;
	union callback_fn *fn;
	fn = malloc(sizeof(union callback_fn));
	fn->game_created_fn = callback;
	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	netemu_register_callback(info->_internal->game_created_callback, fn, 1);
}


void _server_connection_add_game_struct(struct netemu_info* info, struct game* game) {
	int index;

	if(info->_internal->game_create_requested) {
		info->current_game = game;
		info->_internal->game_create_requested = 0;
	}

	index = netemu_list_contains(info->_internal->games,game);
	if(index != -1) {
		/* TODO: Free memory here... */
		info->_internal->games->elements[index] = game;
	}
	else {
		netemu_list_add(info->_internal->games,game);
	}
}

int server_connection_start_game(struct netemu_info *info) {
	time_t timestamp;
	struct application_instruction *message, *reply;

	message = netemu_application_create_message();
	netemu_application_start_game_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	message->important = 1;
	reply = netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, START_GAME, timestamp);
	return 1;
}


struct netemu_info *netemu_server_connection_new(char* user, char* emulator_name, struct netemu_sender_buffer* buffer) {
	struct netemu_info *info;

	info = (struct netemu_info*)malloc(sizeof(struct netemu_info));
	if(info == NULL) {
		return NULL;
	}
	info->user = user;
	info->emulator_name = emulator_name;
	info->current_game = NULL;
	info->has_id = 0;
	info->_internal = malloc(sizeof(struct _server_connection_internal));
	info->_internal->chat_callback = netemu_list_new(3, FALSE);
	info->_internal->game_created_callback = netemu_list_new(3, FALSE);
	info->_internal->join_callback = netemu_list_new(3, FALSE);
	info->_internal->leave_callback = netemu_list_new(3, FALSE);
	info->_internal->play_values_callback = netemu_list_new(3, FALSE);
	info->_internal->receive_buffer = netemu_packet_buffer_new(100);
	info->_internal->send_buffer = buffer;
	info->_internal->buffered_values = malloc(sizeof(struct buffered_play_values));
	info->_internal->buffered_values->values = NULL;
	info->_internal->game_create_requested = 1;
	info->_internal->users = netemu_list_new(10, FALSE);
	netemu_list_register_sort_fn(info->_internal->users,_server_connection_user_comparator);
	info->_internal->games = netemu_list_new(10, FALSE);
	netemu_list_register_sort_fn(info->_internal->games,_server_connection_game_comparator);

	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PING,_netemu_respond_to_ping, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,USER_JOINED,_netemu_respond_to_user_join, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,LOGIN_SUCCESS,_netemu_respond_to_login_success, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer, PLAYER_JOINED, _netemu_respond_to_player_joined, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,CREATE_GAME,_netemu_respond_to_game_created, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,BUFFERED_PLAY_VALUES,_netemu_respond_to_buffered_values, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST,_netemu_respond_to_player_list, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,GAME_STATUS_UPDATE,_netemu_respond_to_game_status_update, info);
	return info;
}

int server_connection_login(struct netemu_info* info) {
	struct application_instruction *message;
	struct application_instruction *success;
	time_t timestamp;
	message = netemu_application_create_message();

	netemu_application_login_request_add(message,info->emulator_name,info->user,1);
	message->important = 1;

	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	success = netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, LOGIN_SUCCESS, timestamp);
	return 1;
}


int netemu_join_game(struct netemu_info *info, NETEMU_DWORD gameid) {
	struct application_instruction* message;
	time_t timestamp;
	message = netemu_application_create_message();
	timestamp = time(NULL);
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST, timestamp);
	return 1;
}

int server_connection_join_game_async(struct netemu_info *info, NETEMU_DWORD gameid) {
	struct application_instruction* message;

	message = netemu_application_create_message();
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);

	return 1;
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

void server_connection_add_game(struct netemu_info *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count) {
	struct game* game;
	game = malloc(sizeof(struct game));
	game->app_name = app_name;
	game->name = game_name;
	game->id = id;
	game->status = status;
	game->users_count = users_count;
	_server_connection_add_game_struct(info, game);
}

void server_connection_add_user(struct netemu_info* info, NETEMU_WORD user_id, char connection_type, char* username) {
	struct user* user;
	user = malloc(sizeof(struct user*));
	user->id = user_id;
	user->connection = connection_type;
	user->username = username;
	_server_connection_add_user_struct(info,user);
}

void _server_connection_add_user_struct(struct netemu_info* info, struct user *user) {
	int index;

	index = netemu_list_contains(info->_internal->users,user);
	if(index != -1) {
		/* TODO: Free memory here... */
		info->_internal->users->elements[index] = user;
	}
	else {
		netemu_list_add(info->_internal->users,user);
	}
}

struct game** netemu_get_game_list(struct netemu_info* info, int *count) {
	struct game** games;
	netemu_list_copy(info->_internal->games,&games);
	*count = info->_internal->games->count;
	return games;
}

struct user** server_connection_get_user_list(struct netemu_info* info, int *count) {
	struct user** users;
	netemu_list_copy(info->_internal->users,&users);
	*count = info->_internal->users->count;
	return users;
}

void netemu_udp_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params) {
	struct netemu_info* info;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	int i;
	info = (struct netemu_info*) params;
	packet = netemu_transport_unpack(data);
	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		if(instruction->id == CREATE_GAME) {
			printf("GAME CREATED");
		}
		netemu_packet_buffer_add(info->_internal->receive_buffer,instruction);
	}
}

void netemu_tcp_connection_receive(char* data, size_t size, struct netemu_tcp_connection *receiver, void* params) {
	struct netemu_info* info;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	int i;
	info = (struct netemu_info*) params;
	packet = netemu_transport_unpack(data);
	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		if(instruction->id == CREATE_GAME) {
			printf("GAME CREATED");
		}
		netemu_packet_buffer_add(info->_internal->receive_buffer,instruction);
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

struct buffered_play_values* server_connection_get_play_values(struct netemu_info *info) {
	return info->_internal->buffered_values;
}

void netemu_send_play_values(struct netemu_info* info, int size, void* data) {
	time_t timestamp;
	struct application_instruction *message;
	message = netemu_application_create_message();
	netemu_application_buffered_play_values_add(message,size,data);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
}

int netemu_send_player_ready(struct netemu_info *info) {
	time_t timestamp;
	struct application_instruction *message;

	message = netemu_application_create_message();
	netemu_application_player_ready_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message);
	return 1;
}
