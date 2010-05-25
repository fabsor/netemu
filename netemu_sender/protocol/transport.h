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

/**
 * @file
 * This file contains functions in the interface for the kaillera and netemu p2p
 * transport protocol.
 */

/**
 * @defgroup netemu_transport Transport layer
 * Functions related to the kaillera and netemu p2p transport layer.
 */
#pragma once
#ifndef NETEMU_TRANSPORT_H_
#define NETEMU_TRANSPORT_H_

#include "netlib_util.h"
#include "netlib_network.h"
#include "protocol.h"

/**
 * Pack application instruction into something that's ready to be sent over
 * the network.
 * @ingroup netemu_transport
 * @param messages an array of instructions that should be included in the packet.
 * @param count the number of instructions in the array.
 * @return a buffer containing the size and the packets.
 */
struct transport_packet_buffer netemu_transport_pack(struct application_instruction **messages, char count);

/**
 * Free a packet buffer
 * @ingroup netemu_transport
 * @param buffer the buffer to free.
 */
void netemu_transport_free_packet_buffer(struct transport_packet_buffer* buffer);

#endif /* NETEMU_TRANSPORT_H_ */
