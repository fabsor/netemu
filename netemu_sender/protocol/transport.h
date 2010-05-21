/*
 * transport.h
 *
 *  Created on: 5 apr 2010
 *      Author: fabbe
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "netlib_util.h"
#include "netemu_socket.h"
#include "protocol.h"

struct transport_packet_buffer netemu_transport_pack(struct application_instruction **messages, char count);
void netemu_transport_free_packet_buffer(struct transport_packet_buffer* buffer);

#endif /* TRANSPORT_H_ */
