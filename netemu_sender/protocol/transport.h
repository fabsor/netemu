/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "netlib_util.h"
#include "netemu_socket.h"
#include "protocol.h"

struct transport_packet_buffer netemu_transport_pack(struct application_instruction **messages, char count);
void netemu_transport_free_packet_buffer(struct transport_packet_buffer* buffer);

#endif /* TRANSPORT_H_ */
