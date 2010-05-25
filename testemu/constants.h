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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define EMUNAME		"testemu"
#define PLAYERNAME	"fabsor"


#define BIND_ADDR	netlib_inet_addr("127.0.0.1")

#define ADDR	netlib_inet_addr("192.168.0.179")
#define PORT	netlib_htons(27888)

#define VALUE	"0x0x0xff"
#define JOIN_VALUE "1x1x1xff"

#define P2P_HOST_TEST_BIND_ADDR	netlib_inet_addr("127.0.0.1")
#define P2P_HOST_TEST_PORT	netlib_htons(3535)

#define P2P_JOIN_TEST_BIND_ADDR	netlib_inet_addr("127.0.0.1")
#define P2P_JOIN_TEST_CONNECT_ADDR	netlib_inet_addr("127.0.0.1")
#define P2P_JOIN_TEST_PORT	netlib_htons(3536)
#define P2P_JOIN_TEST_CONNECT_PORT	netlib_htons(3535)

#define CLOUD_NAME	"the_cloud"

#define NO_GAMES	2


#endif /* CONSTANTS_H_ */
