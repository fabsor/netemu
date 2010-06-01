/*
 * application.c
 *
 *  Created on: May 10, 2010
 *      Author: emil
 */
#include "transport.h"
#include "application.h"
#include "application_kaillera.h"
#include "application_p2p_internal.h"
#include <stdlib.h>
#include "netlib_error.h"
#include "../util.h"
#include "../network/receiver_buffer.h"

int netemu_application_parse_tcp(NETLIB_SOCKET socket, netemu_connection_types type,  union netemu_connection_type connection, void* param) {
	int i, j;
	struct transport_packet* packet;
	struct transport_instruction* instruction;
	struct application_instruction *app_instruction;
	struct netemu_receiver_buffer *buffer;
	int error;
	buffer = (struct netemu_receiver_buffer *)param;
	if((packet = malloc(sizeof(struct transport_packet))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}
	error = netlib_recvfrom(socket, &packet->count, sizeof(char), 0, NULL, 0);
	if(error == -1) {
		return -1;
	}
	else if(error == 0) {
		return 1;
	}
	if((packet->instructions = malloc(sizeof(struct transport_instruction*)*packet->count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(packet);
		return -1;
	}
	for (i = 0; i < packet->count; i++) {
		if((instruction = malloc(sizeof(struct transport_instruction))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);

			for(j = 0; j <= i; j++)
				free(instruction);

			free(packet->instructions);
			free(packet);
			return -1;
		}

		error = netlib_recvfrom(socket, (char*)&instruction->serial, sizeof(NETEMU_WORD), 0, NULL, 0);
		if(error == -1) {
			return -1;
		}

		error = netlib_recvfrom(socket, (char*)&instruction->length, sizeof(NETEMU_WORD),0,  NULL, 0);
		if(error == -1) {
			return -1;
		}
		instruction->instruction = malloc(instruction->length);
		netlib_recvfrom(socket, instruction->instruction, instruction->length,0, NULL, 0);
		if(error == -1) {
			return -1;
		}
		packet->instructions[i] = instruction;
	}
	for(i = 0; i < packet->count; i++) {
		app_instruction = netemu_application_instruction_parse(packet->instructions[i]);
		netemu_receiver_buffer_add(buffer, app_instruction,type,connection, NULL, 0);
	}
	return 1;
}

int netemu_application_parse_udp(NETLIB_SOCKET socket, netemu_connection_types type,  union netemu_connection_type connection, void* param) {
	int i, j;
	socklen_t addr_size;
	unsigned int pos;
	netlib_sockaddr_storage addr;
	struct transport_packet* packet;
	struct application_instruction *app_instruction;
	struct transport_instruction* instruction;
	struct netemu_receiver_buffer *buffer;
	int error;
	char* bytebuffer;

	bytebuffer = malloc(512);
	addr_size = sizeof(addr);
	//addr = malloc(sizeof(netemu_sockaddr_in));
	buffer = (struct netemu_receiver_buffer *)param;
	error = netlib_recvfrom(socket, bytebuffer, 512, 0, (netlib_sockaddr*)&addr, &addr_size);

	if(error == -1) {
		return -1;
	}
	if(error == 0) {
		return 1;
	}

	if((packet = malloc(sizeof(struct transport_packet))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	memcpy(&packet->count,bytebuffer, sizeof(char));
	pos = sizeof(char);
	if((packet->instructions = malloc(sizeof(struct transport_instruction*)*packet->count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(packet);
		return -1;
	}
	for (i = 0; i < packet->count; i++) {
		if((instruction = malloc(sizeof(struct transport_instruction))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);

			for(j = 0; j <= i; j++)
				free(instruction);

			free(packet->instructions);
			free(packet);
			return -1;
		}

		memcpy(&instruction->serial,bytebuffer+pos,sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		memcpy(&instruction->length, bytebuffer+pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		instruction->instruction = malloc(instruction->length);
		memcpy(instruction->instruction, bytebuffer+pos, instruction->length);
		pos += instruction->length;
		packet->instructions[i] = instruction;
	}

	for(i = 0; i < packet->count; i++) {
		app_instruction = netemu_application_instruction_parse(packet->instructions[i]);
		netemu_receiver_buffer_add(buffer, app_instruction,type,connection,netemu_util_copy_addr((netlib_sockaddr*)&addr, addr_size), addr_size);
	}
	return 1;
}

struct application_instruction* netemu_application_instruction_parse(struct transport_instruction *instruction) {
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
	app_instruction->user = netemu_util_parse_string(data);
	if(app_instruction->user == NULL) {
		/* Error code already set in parse_string */
		free(app_instruction);
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	data += strlen(app_instruction->user) + 1;

	app_instruction->body_size = instruction->length - (sizeof(char) + strlen(app_instruction->user)+1);
	app_instruction->body = NULL;
	/* TODO: We need a smarter way to deal with this. */
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
		case GAME_CLOSED:
			netemu_application_game_closed_parse(app_instruction, data);
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
		case P2P_JOIN_HOST:
			netemu_application_p2p_join_host_parse(app_instruction, data);
			break;
		case P2P_GAME_START: case P2P_PLAYER_READY:
			netemu_application_p2p_start_game_parse(app_instruction, data);
			break;
		case P2P_BUFFERED_PLAY_VALUES:
			netemu_application_p2p_buffered_play_values_parse(app_instruction, data);
			break;
		case P2P_CACHED_BUFFERED_PLAY_VALUES:
			netemu_application_p2p_cached_play_values_parse(app_instruction, data);
			break;
		case P2P_PLAYER_JOIN_SUCCESS:
			netemu_application_p2p_player_join_success_parse(app_instruction, data);
			break;
		case P2P_LEAVE_GAME:
			netemu_application_p2p_player_leave_parse(app_instruction, data);
			break;
		case P2P_LEAVE_CLOUD:
			netemu_application_p2p_user_leave_parse(app_instruction, data);
			break;

	}
	return app_instruction;
}

void netemu_application_instruction_destroy(struct application_instruction* message) {
	if(message->freeBodyFn != NULL) {
		message->freeBodyFn(message);
	}
	free(message->user);
	free(message);
}

