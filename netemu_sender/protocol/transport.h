/*
 * transport.h
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "netemu_util.h"

struct transport_instruction {
	/* ? */
	NETEMU_WORD serial;
	NETEMU_WORD length;
	void* instruction;
};

struct transport_packet {
	char count;
	struct transport_instruction **instructions;
};



#endif /* TRANSPORT_H_ */
