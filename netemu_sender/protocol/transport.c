/*
 * transport.c
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */
#include <stdlib.h>
#include <stdio.h>
#include "transport.h"
#include "application.h"

char* netemu_transport_pack(struct protocol_message **messages, int count) {
	struct transport_packet packet;
	struct transport_instruction* instruction;
	char *buffer;
	int i;
	int total_size;

	total_size = sizeof(int);

	for(i = 0; i < count; i++) {
		total_size += messages[i]->total_size+sizeof(int)*2;
	}
	buffer = malloc(total_size);
	snprintf(buffer,sizeof(int),"%d",count);

	for(i = 0; i < count; i++) {
		snprintf(buffer,sizeof(int),"%d",i+1);
		snprintf(buffer,sizeof(int),"%d",messages[i]->total_size);
		snprintf(buffer,sizeof(int),"%s",messages[i]->body);
	}

	return buffer;
}
