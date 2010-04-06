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

char* netemu_transport_pack(struct protocol_message **messages, int count) {
	char *buffer;
	int i;
	int index;
	int total_size;
	int pos;
	total_size = sizeof(int);
	for(i = 0; i < count; i++) {
		total_size += messages[i]->total_size+sizeof(int)*2;
	}
	buffer = malloc(total_size);
	memcpy(buffer,(void *)&count,sizeof(int));
	pos = sizeof(int);
	for(i = 0; i < count; i++) {
		index = i+1;
		memcpy((void *)(buffer+pos),(void *)&index,sizeof(int));
		pos += sizeof(int);
		memcpy((void *)(buffer+pos),(void *)&messages[i]->total_size,sizeof(int));
		pos += sizeof(int);
		memcpy((void *)(buffer+pos),messages[i]->body,sizeof(messages[i]->total_size));
		pos += sizeof(messages[i]->total_size);
	}
	return (char*)buffer;
}

struct transport_packet* netemu_transport_unpack(char* data) {
	int count;
	int i;
	struct transport_packet* packet;
	struct transport_instruction* instruction;

	packet = malloc(sizeof(struct transport_packet));
	memcpy(&count,data,sizeof(int));
	packet->count = count;
	packet->instructions = malloc(sizeof(struct transport_instruction)*count);
	for(i = 0; i < count; i++) {
		instruction = malloc(sizeof(struct transport_instruction));
		memcpy(&instruction->serial,*(data+sizeof(int)),sizeof(int));
		memcpy(&instruction->length,*(data+sizeof(int)*2),sizeof(int));
		memcpy(instruction->instruction,*(data+sizeof(int)*3),instruction->length);
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
