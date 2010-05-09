/*
 * constants.h
 *
 *  Created on: 4 maj 2010
 *      Author: fabbe
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define EMUNAME		"testemu"
#define PLAYERNAME	"fabsor"


#define BIND_ADDR	netemu_inet_addr("127.0.0.1")

#define ADDR	netemu_inet_addr("127.0.0.1")
#define PORT	netemu_htons(27888)
#define VALUE	"0x0x0xff"

#define P2P_HOST_TEST_BIND_ADDR	netemu_inet_addr("127.0.0.1")
#define P2P_HOST_TEST_PORT	netemu_htons(3535)

#define P2P_JOIN_TEST_BIND_ADDR	netemu_inet_addr("127.0.0.1")
#define P2P_JOIN_TEST_CONNECT_ADDR	netemu_inet_addr("127.0.0.1")
#define P2P_JOIN_TEST_PORT	netemu_htons(3536)
#define P2P_JOIN_TEST_CONNECT_PORT	netemu_htons(3535)

#define CLOUD_NAME	"the_cloud"

#define NO_GAMES	2


#endif /* CONSTANTS_H_ */
