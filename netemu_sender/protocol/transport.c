/*
 * transport.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <string.h>
#include "transport.h"
#include "application_kaillera.h"
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
		index = current_index-i;
		memcpy((void *)(buffer+pos),(void *)&index, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		instruction_size = messages[i]->body_size + wrapper_size;
		memcpy((void *)(buffer+pos), (void *)&instruction_size, sizeof(NETEMU_WORD));
		pos += sizeof(NETEMU_WORD);
		memcpy((void *)(buffer+pos), (void *)&messages[i]->id, sizeof(char));
		pos += sizeof(char);
		memcpy((void *)(buffer+pos), (void *)messages[i]->user, sizeof(char)*(strlen(messages[i]->user)+1));
		pos += sizeof(char)*(strlen(messages[i]->user)+1);
		if(messages[i]->body != NULL){
			messages[i]->packBodyFn(messages[i],(buffer+pos));
			pos += messages[i]->body_size;
		}
		
	}
	current_index++;;
	packet_buffer.data = buffer;
	packet_buffer.size = total_size;
	return packet_buffer;
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
