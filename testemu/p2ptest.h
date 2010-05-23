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

#ifndef P2PTEST_H_
#define P2PTEST_H_
#include "netemu_socket.h"
void host_p2p(netemu_sockaddr_in addr);

void connect_p2p();
void connect_p2p_async();

void run_p2p_host_test(int no_instructions, char connection);
void run_p2p_join_test(int no_instructions, char connection);

#endif /* P2PTEST_H_ */
