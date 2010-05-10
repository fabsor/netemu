/*
 * application.c
 *
 *  Created on: May 10, 2010
 *      Author: emil
 */
#include "transport.h"

int netemu_application_parse(NETEMU_SOCKET socket, void* params) {
	int i, j;
	unsigned int pos;
	struct transport_packet* packet;
	struct transport_instruction* instruction;
	struct netemu_packet_buffer *buffer;
	int error;
	buffer = (struct netemu_packet_buffer *)params;
	if((packet = malloc(sizeof(struct transport_packet))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	error = netemu_recvfrom(socket, &packet->count, sizeof(char), 0, NULL, 0);
	if(error == -1) {
		return NULL;
	}
	if((packet->instructions = malloc(sizeof(struct transport_instruction*)*packet->count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(packet);
		return NULL;
	}
	for (i = 0; i < packet->count; i++) {
		if((instruction = malloc(sizeof(struct transport_instruction))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);

			for(j = 0; j <= i; j++)
				free(instruction);

			free(packet->instructions);
			free(packet);
			return NULL;
		}

		error = netemu_recvfrom(socket, (char*)&instruction->serial, sizeof(NETEMU_WORD), 0, NULL, 0);
		if(error == -1) {
			return NULL;
		}

		error = netemu_recvfrom(socket, (char*)&instruction->length, sizeof(NETEMU_WORD),0,  NULL, 0);
		if(error == -1) {
			return NULL;
		}
		instruction->instruction = malloc(instruction->length);
		netemu_recvfrom(socket, instruction->instruction, instruction->length,0, NULL, 0);
		if(error == -1) {
			return NULL;
		}
		packet->instructions[i] = instruction;
	}
	for(i = 0; i < packet->count; i++) {
		instruction = netemu_application_parse_message(packet->instructions[i]);
		netemu_packet_buffer_add(buffer, instruction,TCP_CONNECTION,type);
	}

}

struct application_instruction* netemu_application_parse_message(struct transport_instruction *instruction) {
	struct application_instruction *app_instruction;
	char *data;

	if((app_instruction = malloc(sizeof(struct application_instruction))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	app_instruction->timestamp = time(NULL);
	app_instruction->important = 0;
	data = (char*)instruction->instruction;
	memcpy(&app_instruction->id,data,sizeof(char));
	data += sizeof(char);

	if((app_instruction->user = netemu_util_parse_string(data)) == NULL) {
		/* Error code already set in parse_string */
		free(app_instruction);
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	data += strlen(app_instruction->user) + 1;

	app_instruction->body_size = instruction->length - (sizeof(char) + strlen(app_instruction->user)+1);
	app_instruction->body = NULL;

	switch(app_instruction->id) {
		case PING:
			app_instruction->body_size = 0;
			break;
		case LOGIN_SUCCESS:
			netemu_application_login_success_parse(app_instruction, data);
			break;
		case USER_JOINED:
			netemu_application_user_joined_parse(app_instruction,data);
			break;
		case USER_LEAVE:
			netemu_application_user_leave_parse(app_instruction, data);
			break;
		case PARTYLINE_CHAT:
		case GAME_CHAT:
		case MOTD_CHAT:
			netemu_application_chat_parse(app_instruction, data);
			break;
		case CREATE_GAME:
			netemu_application_create_game_parse(app_instruction,data);
			break;
		case EXISTING_PLAYERS_LIST:
			netemu_application_existing_players_list_parse(app_instruction, data);
			break;
		case GAME_STATUS_UPDATE:
			netemu_application_game_status_parse(app_instruction,data);
			break;
		case BUFFERED_PLAY_VALUES:
			netemu_application_buffered_play_values_parse(app_instruction, data);
			break;
		case INTELLIGENTLY_CACHED_N_BUFFERED_PLAY_VALUES:
			netemu_application_intelligently_cached_play_values_parse(app_instruction, data);
			break;
		case PLAYER_DROPPED:
			netemu_application_player_dropped_parse(app_instruction, data);
			break;
		case PLAYER_JOINED:
			netemu_application_player_joined_parse(app_instruction, data);
			break;
		case START_GAME:
			netemu_application_start_game_parse(app_instruction, data);
			break;
		case P2P_LOGIN_REQUEST:
			netemu_application_p2p_login_request_parse(app_instruction, data);
			break;
		case P2P_LOGIN_SUCCESS:
			netemu_application_p2p_login_success_parse(app_instruction, data);
			break;
		case P2P_USER_JOIN:
			netemu_application_p2p_user_join_parse(app_instruction, data);
			break;
		case CREATE_P2P_GAME:
			netemu_application_p2p_create_game_parse(app_instruction,data);
			break;
		case JOIN_P2P_GAME:
			netemu_application_p2p_player_join_parse(app_instruction,data);
			break;
		case P2P_GAME_START: case P2P_PLAYER_READY:
			netemu_application_p2p_start_game_parse(app_instruction, data);
			break;

	}
	return app_instruction;
}

