/*
 * transport.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include "transport.h"
#include "application.h"
#include "netlib_error.h"

struct transport_packet_buffer netemu_transport_pack(struct application_instruction **messages, char count) {
	static int current_index;
	struct transport_packet_buffer packet_buffer;
	char *buffer;
	int i;
	int index;
	int pos;
	int total_size;
	int instruction_size;
	int wrapper_size;

	total_size = sizeof(char);

	for(i = 0; i < count; i++) {
		wrapper_size = sizeof(char)*(strlen(messages[i]->user)+2);
		total_size +=
			(sizeof(NETEMU_WORD) * 2) +
			wrapper_size +
			messages[i]->body_size;
	}
	if((buffer = malloc(total_size)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		/* **** NEED TO RETURN FROM THE FUNCTION HERE **** */
	}
	memcpy(buffer,(void *)&count,sizeof(char));
	pos = sizeof(char);
	for (i = 0; i < count; i++) {
		index = current_index+i;
		memcpy((void *)(buffer+pos),(void *)&index, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		instruction_size = messages[i]->body_size + wrapper_size;
		memcpy((void *)(buffer+pos), (void *)&instruction_size, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		memcpy((void *)(buffer+pos), (void *)&messages[i]->id, sizeof(char));
		pos += sizeof(char);
		if (messages[i]->id > 24) {
			memcpy((void *)(buffer+pos),(void *)&messages[i]->p2p_id, sizeof(NETEMU_WORD));
			pos += sizeof(long);
		}
		memcpy((void *)(buffer+pos), (void *)messages[i]->user, sizeof(char)*(strlen(messages[i]->user)+1));
		pos += sizeof(char)*(strlen(messages[i]->user)+1);
		if(messages[i]->body != NULL){
			messages[i]->packBodyFn(messages[i],(buffer+pos));
			pos += messages[i]->body_size;
		}
		
	}
	current_index += count;
	packet_buffer.data = buffer;
	packet_buffer.size = total_size;
	return packet_buffer;
}

struct transport_packet* netemu_transport_unpack(char* data) {
	char count;
	int i, j;
	unsigned int pos;
	static int foo = 0;
	struct transport_packet* packet;
	struct transport_instruction* instruction;
	foo++;
	if((packet = malloc(sizeof(struct transport_packet))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
	memcpy(&count,data,sizeof(char));
	packet->count = count;
	if((packet->instructions = malloc(sizeof(struct transport_instruction*)*count)) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		free(packet);
		return NULL;
	}
	pos = sizeof(char);
	for (i = 0; i < count; i++) {
		if((instruction = malloc(sizeof(struct transport_instruction))) == NULL) {
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);

			for(j = 0; j <= i; j++)
				free(instruction);

			free(packet->instructions);
			free(packet);
			return NULL;
		}

		memcpy(&instruction->serial, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);

		memcpy(&instruction->length, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		instruction->instruction = malloc(instruction->length);
		memcpy(instruction->instruction, data + pos,instruction->length);
		pos += instruction->length;
		packet->instructions[i] = instruction;

	}
	return packet;
}

void netemu_transport_free_packet(struct transport_packet* packet) {
	int i;
	for(i = 0; i < packet->count; i++) {
		free(packet->instructions[i]->instruction);
		free(packet->instructions[i]);
	}
	free(packet->instructions);
	free(packet);
}

void netemu_transport_free_packet_buffer(struct transport_packet_buffer* buffer) {
	free(buffer->data);
}
