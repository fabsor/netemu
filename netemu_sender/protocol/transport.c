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
	wrapper_size = sizeof(char);
	for(i = 0; i < count; i++) {
		total_size +=
			(sizeof(NETEMU_WORD) * 2) +
			wrapper_size +
			messages[i]->body_size;
	}
	buffer = malloc(total_size);
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
		messages[i]->packBodyFn(messages[i],(buffer+pos));
		pos += messages[i]->body_size;
	}
	current_index += count;
	packet_buffer.data = buffer;
	packet_buffer.size = total_size;
	return packet_buffer;
}

struct transport_packet* netemu_transport_unpack(char* data) {
	char count;
	int i;
	unsigned int pos;
	struct transport_packet* packet;
	struct transport_instruction* instruction;

	packet = malloc(sizeof(struct transport_packet));
	memcpy(&count,data,sizeof(char));
	packet->count = count;
	packet->instructions = malloc(sizeof(struct transport_instruction)*count);
	pos = sizeof(char);
	for (i = 0; i < count; i++) {
		instruction = malloc(sizeof(struct transport_instruction));
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
