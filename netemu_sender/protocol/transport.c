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
		memcpy((void *)(buffer+pos),messages[i]->body,sizeof(int));
		pos += sizeof(messages[i]->total_size);
	}
	return (char*)buffer;
}

struct transport_packet* netemu_transport_unpack(void* data) {
	struct tranport_packet* transport_packet;
	struct transport_instruction instruction;


}
