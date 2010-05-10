
#include <stdio.h>
#include "netemu_info.h"
#include "interface/netemu_kaillera.h"
#include "protocol/application_kaillera.h"
#include "netemu_resources.h"
#include "network/netemu_packet_buffer.h"
#include "network/netemu_sender_buffer.h"
#include "network/netemu_sender_udp.h"
#include "network/netemu_receiver.h"
#include "structures/netemu_hashtbl.h"
#include "netemu_list.h"
#include "netemu_thread.h"
#include "responses.h"
#include "protocol/application_p2p.h"

void netemu_kaillera_add_user(struct netemu_info* info, NETEMU_WORD user_id, char connection_type, char* username);
void netemu_kaillera_remove_user(struct netemu_info *info, NETEMU_WORD user_id);
void netemu_kaillera_add_game(struct netemu_info *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count, char *creator);
void netemu_kaillera_add_player(struct game *game, struct player *player);

void _netemu_kaillera_add_game_struct(struct netemu_info* info, struct game* game);
int _netemu_kaillera_user_comparator(const void* item1,const void* item2);
int _netemu_kaillera_game_comparator(const void* item1, const void* item2);
void _netemu_kaillera_add_user_struct(struct netemu_info* info, struct user *user);


int netemu_send_chat_message(struct netemu_info *info, char *message) {
	/*TODO: Implement chat message! */
	/*
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	*/
	return 1;
}

int netemu_disconnect(struct netemu_info *info, char *message) {
	struct netemu_client *client;
	struct transport_packet_buffer buffer;
	struct application_instruction *instruction;

	client = netemu_resources_get_client();
	instruction = netemu_application_create_message();
	netemu_application_user_leave_add(instruction, message);
	buffer = netemu_transport_pack(&instruction,1);
	return netemu_sender_udp_send(client->sender ,buffer.data, buffer.size);
}


int netemu_kaillera_create_game(struct netemu_info *info, char *gamename, struct game** result) {
	time_t timestamp;
	struct application_instruction *message;
	struct netemu_packet_buffer_item *item;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);

	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer, message, UDP_CONNECTION, type);
	item = netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, CREATE_GAME, timestamp);
	*result = (struct game*)item->instruction->body;
	return 1;
}

void netemu_kaillera_create_game_async(struct netemu_info *info, char *gamename, kailleraGameCreatedFn callback) {
	time_t timestamp;
	struct application_instruction *message;
	union callback_fn *fn;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	fn = malloc(sizeof(union callback_fn));
	fn->game_created_fn = callback;
	message = netemu_application_create_message();
	netemu_application_create_game_add(message, gamename);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message,UDP_CONNECTION, type);
	netemu_kaillera_register_callback(info->_internal->game_created_callback, fn, 1);
}


void _netemu_kaillera_add_game_struct(struct netemu_info* info, struct game* game) {
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

int netemu_kaillera_start_game(struct netemu_info *info) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_start_game_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type);
	message->important = 1;
	netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, START_GAME,timestamp);
	return 1;
}


struct netemu_info *netemu_info_new(char* user, char* emulator_name, struct netemu_sender_buffer* buffer) {
	struct netemu_info *info;

	info = (struct netemu_info*)malloc(sizeof(struct netemu_info));
	if(info == NULL) {
		return NULL;
	}
	info->emulator_name = emulator_name;
	info->current_game = NULL;
	info->username = user;
	info->user_count = 0;
	info->game_count = 0;
	info->_internal = malloc(sizeof(struct _netemu_info_internal));
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
	info->_internal->has_id = 0;
	netemu_list_register_sort_fn(info->_internal->users,_netemu_kaillera_user_comparator);
	info->_internal->games = netemu_list_new(10, FALSE);
	netemu_list_register_sort_fn(info->_internal->games,_netemu_kaillera_game_comparator);

	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PARTYLINE_CHAT, _netemu_respond_to_chat, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,MOTD_CHAT, _netemu_respond_to_chat, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PING,_netemu_respond_to_ping, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,USER_JOINED,_netemu_respond_to_user_join, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer, USER_LEAVE, _netemu_respond_to_user_leave, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,LOGIN_SUCCESS,_netemu_respond_to_login_success, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer, PLAYER_JOINED, _netemu_respond_to_player_joined, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,CREATE_GAME,_netemu_respond_to_game_created, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,BUFFERED_PLAY_VALUES,_netemu_respond_to_buffered_values, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST,_netemu_respond_to_player_list, info);
	netemu_packet_buffer_add_instruction_received_fn(info->_internal->receive_buffer,GAME_STATUS_UPDATE,_netemu_respond_to_game_status_update, info);
	return info;
}

int netemu_kaillera_login(struct netemu_info* info) {
	struct application_instruction *message;
	union netemu_connection_type type;
	int error;
	time_t timestamp;

	message = netemu_application_create_message();
	if(message == NULL) {
		return -1;
	}
	type.udp_sender = netemu_resources_get_sender();
	error = netemu_application_login_request_add(message,info->emulator_name,info->username,1);
	if(error == -1) {
		netemu_application_free_message(message);
		return -1;
	}
	message->important = 1;
	timestamp = time(NULL);

	if(netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type) == -1) {
		netemu_application_free_login_request((struct login_request*)message->body);
	}
	netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer, LOGIN_SUCCESS, timestamp);
	return 0;
}


int netemu_kaillera_join_game(struct netemu_info *info, NETEMU_DWORD gameid) {
	struct application_instruction* message;
	time_t timestamp;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	timestamp = time(NULL);
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type);
	netemu_packet_buffer_wait_for_instruction(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST, timestamp);
	return 1;
}

int netemu_kaillera_join_game_async(struct netemu_info *info, NETEMU_DWORD gameid) {
	struct application_instruction* message;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type);

	return 1;
}


void netemu_kaillera_add_player(struct game *game, struct player *player) {
	struct player* players;
	int i;

	game->players->players_count++;
	if((game->players->players = realloc(game->players->players,game->players->players_count*sizeof(struct player))) == NULL) {
		players = malloc(sizeof(struct player)*game->players->players_count);
		for(i = 0; i < game->players->players_count-1; i++) {
			players[i] = game->players->players[i];
		}
		free(game->players->players);
		game->players->players = players;
	}
	game->players->players[game->players->players_count-1] = *player;

}

void netemu_kaillera_add_game(struct netemu_info *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count, char *creator) {
	struct game* game;
	game = malloc(sizeof(struct game));
	game->app_name = app_name;
	game->name = game_name;
	game->id = id;
	game->status = status;
	game->users_count = users_count;
	game->creator = creator;
	_netemu_kaillera_add_game_struct(info, game);
}

void netemu_kaillera_add_user(struct netemu_info* info, NETEMU_WORD user_id, char connection_type, char* username) {
	struct user* user;
	user = malloc(sizeof(struct user));
	user->id = user_id;
	user->connection = connection_type;
	user->username = username;
	_netemu_kaillera_add_user_struct(info,user);
}

void netemu_kaillera_remove_user(struct netemu_info *info, NETEMU_WORD user_id)
{
	int i;
	int index_to_remove = -1;
	struct user *user;
	for(i = 0; i < info->_internal->users->count; i++) {
		user = (struct user*)netemu_list_get(info->_internal->users, i);
		if(user->id == user_id) {
			index_to_remove = i;
			break;
		}
	}

	if(index_to_remove != -1) {
		netemu_list_remove_at(info->_internal->users, index_to_remove);
		free(user);
	}
}

void _netemu_kaillera_add_user_struct(struct netemu_info* info, struct user *user) {
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

struct game** netemu_kaillera_get_game_list(struct netemu_info* info, int *count) {
	struct game** games;
	netemu_list_copy(info->_internal->games,&games);
	*count = info->_internal->games->count;
	return games;
}

struct user** netemu_kaillera_get_user_list(struct netemu_info* info, int *count) {
	struct user** users;
	netemu_list_copy(info->_internal->users,&users);
	*count = info->_internal->users->count;
	return users;
}

void netemu_udp_connection_receive(char* data, size_t size, struct netemu_receiver_udp* receiver, void* params) {
	struct netemu_info* info;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	union netemu_connection_type type;
	int i;

	type.udp_sender = netemu_resources_get_sender();
	info = (struct netemu_info*) params;
	packet = netemu_transport_unpack(data);

	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		if(instruction != NULL) {
			netemu_packet_buffer_add(info->_internal->receive_buffer,instruction, UDP_CONNECTION, type);
		}
	}
}

void netemu_tcp_connection_receive(char* data, size_t size, struct netemu_tcp_connection *receiver, void* params) {
	struct netemu_info* info;
	struct transport_packet* packet;
	struct application_instruction* instruction;
	union netemu_connection_type type;
	int i;

	type.connection = receiver;
	info = (struct netemu_info*) params;
	packet = netemu_transport_unpack(data);

	for (i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		if(instruction != NULL) {
			netemu_packet_buffer_add(info->_internal->receive_buffer,instruction, TCP_CONNECTION, type);
		}
	}
}
/**
 * Compare users.
 */
int _netemu_kaillera_user_comparator(const void* item1,const void* item2) {
	return ((struct user*)item1)->id - ((struct user*)item2)->id;
}
/**
 * Compare games.
 */
int _netemu_kaillera_game_comparator(const void* item1, const void* item2) {
	return ((struct game*)item1)->id - ((struct game*)item2)->id;
}

struct buffered_play_values* netemu_kaillera_get_play_values(struct netemu_info *info) {
	return info->_internal->buffered_values;
}

void netemu_kaillera_send_play_values(struct netemu_info* info, int size, void* data) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_buffered_play_values_add(message,size,data);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type);
}

int netemu_kaillera_send_player_ready(struct netemu_info *info) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_create_message();
	netemu_application_player_ready_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_CONNECTION, type);
	return 1;
}
