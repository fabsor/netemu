/*
 * p2ptest.h
 *
 *  Created on: 4 maj 2010
 *      Author: fabbe
 */

#ifndef P2PTEST_H_
#define P2PTEST_H_
#include "netemu_socket.h"
void host_p2p(netemu_sockaddr_in addr);

void connect_p2p();
void connect_p2p_async();

void run_p2p_host_test(int no_instructions);
void run_p2p_join_test(int no_instructions);

#endif /* P2PTEST_H_ */
