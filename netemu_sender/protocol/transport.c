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

char* netemu_transport_pack(struct application_instruction **messages, char count) {
	char *buffer;
	int i;
	int index;
	int pos;
	int total_size;
	int instruction_size;
	total_size = sizeof(int);
	for(i = 0; i < count; i++) {
		total_size +=
			(sizeof(NETEMU_WORD) * 2) +
			APPLICATION_INSTRUCTION_SIZE +
			sizes[messages[i]->id - 1];
	}
	buffer = malloc(total_size);
	memcpy(buffer,(void *)&count,sizeof(char));
	pos = sizeof(char);
	for(i = 0; i < count; i++) {
		index = i+1;
		memcpy((void *)(buffer+pos),(void *)&index, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);

		instruction_size = APPLICATION_INSTRUCTION_SIZE + sizes[messages[i]->id - 1];
		memcpy((void *)(buffer+pos), (void *)&instruction_size, sizeof(NETEMU_WORD));

		memcpy((void *)(buffer+pos),messages[i]->body, sizes[messages[i]->id - 1]);
		pos += sizes[messages[i]->id - 1];
	}
	return (char*)buffer;
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
	pos = sizeof(int);
	for(i = 0; i < count; i++) {
		instruction = malloc(sizeof(struct transport_instruction));
		memcpy(&instruction->serial, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);

		memcpy(&instruction->length, data + pos, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);

		memcpy(instruction->instruction, data + pos,instruction->length);
		pos += instruction->length;
		packet->instructions[i] = instruction;
	}
}

netemu_transport_free_packet(struct transport_packet* packet) {
	int i;
	for(i = 0; i < packet->count; i++) {
		free(packet->instructions[i]->instruction);
		free(packet->instructions[i]);
	}
	free(packet->instructions);
	free(packet);
}
