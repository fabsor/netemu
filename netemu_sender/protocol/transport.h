/*
 * transport.h
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

struct transport_instruction {
	/* ? */
	int serial;
	int length;
	void* instruction;
};

struct transport_packet {
	int count;
	struct transport_instruction **instructions;
};



#endif /* TRANSPORT_H_ */
