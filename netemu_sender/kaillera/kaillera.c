/**
 * @file
 * This file contains the netemu_kaillera module.
 */
#include <stdio.h>
#include "kaillera_internal.h"
#include "../interface/netemu_kaillera.h"
#include "../protocol/application_kaillera.h"
#include "../resources.h"
#include "netlib_error.h"
#include "../network/receiver_buffer.h"
#include "../network/sender_buffer.h"
#include "../network/sender_udp.h"
#include "../network/receiver_udp.h"
#include "../structures/netemu_hashtbl.h"
#include "netemu_list.h"
#include "netemu_thread.h"
#include "kaillera_responses.h"
#include "../protocol/application_p2p.h"

void netemu_kaillera_add_user(struct netemu_kaillera* info, NETEMU_WORD user_id, char connection_type, char* username);
void netemu_kaillera_remove_user(struct netemu_kaillera *info, NETEMU_WORD user_id);
void netemu_kaillera_add_game(struct netemu_kaillera *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count, char *creator);
void netemu_kaillera_add_player(struct game *game, struct player *player);
int netemu_kaillera_receive_play_values(struct netemu_kaillera *info);
void _netemu_kaillera_add_game_struct(struct netemu_kaillera* info, struct game* game);
int _netemu_kaillera_user_comparator(const void* item1,const void* item2);
int _netemu_kaillera_game_comparator(const void* item1, const void* item2);
void _netemu_kaillera_add_user_struct(struct netemu_kaillera* info, struct user *user);
int netemu_kaillera_login(struct netemu_kaillera* connection);
int _netemu_kaillera_buffered_values_cmp(char *playerval, char *cachedval, int size, int player_no);
int _netemu_kaillera_buffered_values_cmp_all(char *playerval, char *cachedval, int size);


int netemu_send_chat_message(struct netemu_kaillera *info, char *message) {
	/*TODO: Implement chat message! */
	/*
	struct application_instruction *messages[1];
	messages[0] = netemu_application_create_message();
	*/
	return 0;
}

/**
 * Disconnect from a kaillera server.
 * @todo this probably doesn't work, but it's fun to try!
 * @ingroup netemu_kaillera
 * @param info the instance of the netemu_kaillera module.
 * @param message your going away message.
 */
int netemu_disconnect(struct netemu_kaillera *info, char *message) {
	struct netemu_client *client;
	struct application_instruction *instruction;

	client = netemu_resources_get_client();
	instruction = netemu_application_instruction_create();
	netemu_application_user_leave_add(instruction, message);
	/* TODO: Fix this function.*/
	return 0;
}

/**
 * Create a game on the server.
 * @param netemu_kaillera info an instance of the netemu_kaillera module.
 * @param gamename the name of the game
 * @param result the game that was created.
 */
int netemu_kaillera_create_game(struct netemu_kaillera *info, char *gamename, struct game** result) {
	time_t timestamp;
	struct application_instruction *message;
	struct netemu_receiver_buffer_item *item;
	union netemu_connection_type type;
	if(info->current_game != NULL) {
		netlib_set_last_error(NETEMU_EINKAILLERAGAME);
		return -1;
	}

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	if(message == NULL) {
		return -1;
	}
	netemu_application_create_game_add(message, gamename);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer, message, UDP_SENDER, type);
	item = netemu_receiver_buffer_wait_for_instruction(info->_internal->receive_buffer, CREATE_GAME, timestamp);
	*result = (struct game*)item->instruction->body;
	return 0;
}

void netemu_kaillera_create_game_async(struct netemu_kaillera *info, char *gamename, kailleraGameCreatedFn callback) {
	time_t timestamp;
	struct application_instruction *message;
	union callback_fn *fn;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	fn = malloc(sizeof(union callback_fn));
	fn->game_created_fn = callback;
	message = netemu_application_instruction_create();
	netemu_application_create_game_add(message, gamename);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message,UDP_SENDER, type);
	netemu_kaillera_register_callback(info->_internal->game_created_callback, fn, 1);
}

void _netemu_kaillera_add_game_struct(struct netemu_kaillera* info, struct game* game) {
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
		info->game_count++;
		netemu_list_add(info->_internal->games,game);
	}
}

int netemu_kaillera_start_game(struct netemu_kaillera *info) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	netemu_application_start_game_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);
	message->important = 1;
	netemu_receiver_buffer_wait_for_instruction(info->_internal->receive_buffer, START_GAME,timestamp);
	return 0;
}

int netemu_kaillera_start_game_async(struct netemu_kaillera *info, gameStartedFn fn) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;
	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	netemu_application_start_game_add(message);
	timestamp = time(NULL);
	netemu_kaillera_register_callback(info->_internal->game_started_callbacks, fn, info);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);
	message->important = 1;
	return 0;
}

struct netemu_kaillera *netemu_kaillera_create(char* user, char* emulator_name, int conneciton_quality) {
	struct netemu_kaillera *info;

	info = (struct netemu_kaillera*)malloc(sizeof(struct netemu_kaillera));
	if(info == NULL) {
		return NULL;
	}
	info->emulator_name = emulator_name;
	info->current_game = NULL;
	info->username = user;
	info->user_count = 0;
	info->game_count = 0;
	info->_internal = malloc(sizeof(struct _netemu_info_internal));
	info->_internal->connected_event = netemu_thread_event_create();
	info->_internal->chat_callback = netemu_list_create(3, FALSE);
	info->_internal->game_created_callback = netemu_list_create(3, FALSE);
	info->_internal->join_callback = netemu_list_create(3, FALSE);
	info->_internal->player_join_callback = netemu_list_create(3, FALSE);
	info->_internal->leave_callback = netemu_list_create(3, FALSE);
	info->_internal->play_values_callback = netemu_list_create(3, FALSE);
	info->_internal->player_ready_callback = netemu_list_create(3, FALSE);
	info->_internal->receive_buffer = netemu_receiver_buffer_create(100);
	info->_internal->game_status_updated_callbacks = netemu_list_create(3, FALSE);
	info->_internal->game_started_callbacks = netemu_list_create(3, FALSE);
	info->_internal->send_buffer = netemu_sender_buffer_new(5, 10);
	info->_internal->cached_values_callback = netemu_list_create(3, TRUE);
	info->_internal->callbacks_to_remove = netemu_list_create(3, TRUE);
	info->_internal->game_create_requested = 1;
	info->_internal->users = netemu_list_create(10, FALSE);
	info->_internal->has_id = 0;
	info->_internal->connection_quality = conneciton_quality;
	info->_internal->cache_index = 0;
	info->_internal->frame_index = 0;
	info->_internal->cached_count = 0;
	info->_internal->values_buffered = 0;
	info->_internal->sent_values = 0;
	info->_internal->received_play_values = netemu_list_create(3, FALSE);
	info->_internal->values_to_send = NULL;
	info->_internal->play_values_event = netemu_thread_event_create();
	netemu_list_register_sort_fn(info->_internal->users,_netemu_kaillera_user_comparator);
	info->_internal->games = netemu_list_create(10, FALSE);
	netemu_list_register_sort_fn(info->_internal->games,_netemu_kaillera_game_comparator);

	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PARTYLINE_CHAT, _netemu_respond_to_chat, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,MOTD_CHAT, _netemu_respond_to_chat, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PING,_netemu_respond_to_ping, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,USER_JOINED,_netemu_respond_to_user_join, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer, USER_LEAVE, _netemu_respond_to_user_leave, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,LOGIN_SUCCESS,_netemu_respond_to_login_success, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer, PLAYER_JOINED, _netemu_respond_to_player_joined, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,CREATE_GAME,_netemu_respond_to_game_created, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,BUFFERED_PLAY_VALUES,_netemu_respond_to_buffered_values, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer, INTELLIGENTLY_CACHED_N_BUFFERED_PLAY_VALUES, _netemu_respond_to_cached_play_values, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST,_netemu_respond_to_player_list, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,GAME_STATUS_UPDATE,_netemu_respond_to_game_status_update, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,PLAYER_READY ,_netemu_respond_to_player_ready, info);
	netemu_receiver_buffer_add_instruction_received_fn(info->_internal->receive_buffer,START_GAME ,_netemu_respond_to_game_started, info);
	return info;
}

int netemu_kaillera_login(struct netemu_kaillera* info) {
	struct application_instruction *message;
	union netemu_connection_type type;
	int error;
	time_t timestamp;

	message = netemu_application_instruction_create();
	if(message == NULL) {
		return -1;
	}
	type.udp_sender = netemu_resources_get_sender();
	error = netemu_application_login_request_add(message,info->emulator_name,info->username, info->_internal->connection_quality);
	if(error == -1) {
		netemu_application_instruction_destroy(message);
		return -1;
	}
	message->important = 1;
	timestamp = time(NULL);

	if(netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type) == -1) {
		netemu_application_free_login_request((struct login_request*)message->body);
	}
	netemu_receiver_buffer_wait_for_instruction(info->_internal->receive_buffer, LOGIN_SUCCESS, timestamp);
	return 0;
}


int netemu_kaillera_join_game(struct netemu_kaillera *info, NETEMU_DWORD gameid) {
	struct application_instruction* message;
	time_t timestamp;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	timestamp = time(NULL);
	netemu_application_join_game_add(message,gameid,1);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);
	netemu_receiver_buffer_wait_for_instruction(info->_internal->receive_buffer,EXISTING_PLAYERS_LIST, timestamp);
	return 0;
}

int netemu_kaillera_join_game_async(struct netemu_kaillera *info, NETEMU_DWORD gameid, joinFn fn) {
	struct application_instruction* message;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	netemu_application_join_game_add(message,gameid,1);
	netemu_kaillera_register_callback(info->_internal->join_callback, fn, 1, info);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);

	return 0;
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

void netemu_kaillera_add_game(struct netemu_kaillera *info, char* app_name, char *game_name, NETEMU_WORD id, char status, int users_count, char *creator) {
	struct game* game;
	game = malloc(sizeof(struct game));
	game->app_name = app_name;
	game->name = game_name;
	game->id = id;
	game->status = status;
	/*TODO: ??? */
	game->users_count = users_count;
	game->creator = creator;
	_netemu_kaillera_add_game_struct(info, game);
}

void netemu_kaillera_add_user(struct netemu_kaillera* info, NETEMU_WORD user_id, char connection_type, char* username) {
	struct user* user;
	user = malloc(sizeof(struct user));
	user->id = user_id;
	user->connection = connection_type;
	user->username = username;
	_netemu_kaillera_add_user_struct(info,user);
}

void netemu_kaillera_remove_user(struct netemu_kaillera *info, NETEMU_WORD user_id)
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

void _netemu_kaillera_add_user_struct(struct netemu_kaillera* info, struct user *user) {
	int index;

	index = netemu_list_contains(info->_internal->users,user);
	if(index != -1) {
		/* TODO: Free memory here... */
		info->_internal->users->elements[index] = user;
	}
	else {
		info->user_count++;
		netemu_list_add(info->_internal->users,user);
	}
}

struct game** netemu_kaillera_get_game_list(struct netemu_kaillera* info, int *count) {
	struct game** games;
	netemu_list_copy(info->_internal->games,(void***)&games);
	*count = info->_internal->games->count;
	return games;
}

struct user** netemu_kaillera_get_user_list(struct netemu_kaillera* info, int *count) {
	struct user** users;
	netemu_list_copy(info->_internal->users,(void***)&users);
	*count = info->_internal->users->count;
	return users;
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

int netemu_kaillera_send_play_values(struct netemu_kaillera* info, int size, void* data) {
	time_t timestamp;
	int i;
	struct application_instruction *message, *reply;
	struct buffered_play_values *values;
	union netemu_connection_type type;
	NETEMU_BOOL is_cached;

	is_cached = FALSE;
	if(info->_internal->values_to_send == NULL) {
		info->_internal->values_to_send = malloc(info->_internal->connection_quality*size);
	}
	memcpy(info->_internal->values_to_send + (info->_internal->values_buffered * size), data, size);

	if(info->_internal->cached_count != 0) {
		values = &info->_internal->cached_values[info->_internal->cache_index];
		memcpy(data, values->values + (values->size*info->_internal->frame_index), size * info->current_game->player_count);
	}
	info->_internal->frame_index++;
	info->_internal->values_buffered++;

	if(info->_internal->values_buffered == info->_internal->connection_quality) {

		if(info->_internal->sent_values == info->_internal->time_band) {
				info->_internal->cache_index = netemu_kaillera_receive_play_values(info);
				info->_internal->frame_index = 0;
				info->_internal->sent_values = 0;
		}

		type.udp_sender = netemu_resources_get_sender();
		message = netemu_application_instruction_create();

		for(i = 0; i < info->_internal->cached_count; i++) {
			if(_netemu_kaillera_buffered_values_cmp(info->_internal->values_to_send,info->_internal->cached_values[i].values,
					size, info->current_game->player_count) == 0) {
				netemu_application_intelligently_cached_play_values_add(message, i);
				is_cached = TRUE;
				break;
			}
		}
		if(!is_cached) {
			netemu_application_buffered_play_values_add(message,size,data);
		}
		timestamp = time(NULL);
		netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);
		info->_internal->values_buffered = 0;
		info->_internal->sent_values++;
	}
	return 0;
}

int netemu_kaillera_receive_play_values(struct netemu_kaillera *info) {
	struct application_instruction *instruction;
	struct buffered_play_values *values;
	int i;
	while(info->_internal->received_play_values->count == 0) {
		netemu_thread_event_wait(info->_internal->play_values_event, NETEMU_INFINITE);
	}

	instruction = info->_internal->received_play_values->elements[0];

	if(instruction->id == INTELLIGENTLY_CACHED_N_BUFFERED_PLAY_VALUES) {
		return ((struct intelligently_cached_buffered_play_values*)instruction->body)->index;
	}

	values = instruction->body;
	for(i = 0; i < info->_internal->cached_count; i++) {
		if(_netemu_kaillera_buffered_values_cmp_all(values->values, info->_internal->cached_values->values, values->size)) {
			return i;
		}
	}

	if(info->_internal->cached_count == 256) {
		for(i = 0; i < 255; i++) {
			info->_internal->cached_values[i] = info->_internal->cached_values[i+1];
		}
		netemu_application_buffered_play_values_copy(&info->_internal->cached_values[255], values);
	}
	else {
		netemu_application_buffered_play_values_copy(&info->_internal->cached_values[info->_internal->cached_count], values);
		info->_internal->cached_count++;
	}
	return info->_internal->cached_count-1;
}

int _netemu_kaillera_buffered_values_cmp(char *playerval, char *cachedval, int size, int player_no) {
	if(cachedval == NULL) {
		return 1;
	}
	return memcmp(cachedval+(size*player_no-1), playerval, size);
}

int _netemu_kaillera_buffered_values_cmp_all(char *playerval, char *cachedval, int size) {
	if(cachedval == NULL) {
		return 1;
	}
	return memcmp(cachedval, playerval, size);
}

int netemu_kaillera_send_player_ready(struct netemu_kaillera *info) {
	time_t timestamp;
	struct application_instruction *message;
	union netemu_connection_type type;

	type.udp_sender = netemu_resources_get_sender();
	message = netemu_application_instruction_create();
	netemu_application_player_ready_add(message);
	timestamp = time(NULL);
	netemu_sender_buffer_add(info->_internal->send_buffer,message, UDP_SENDER, type);
	netemu_receiver_buffer_wait_for_instruction(info->_internal->receive_buffer, PLAYER_READY, timestamp);
	return 0;
}
