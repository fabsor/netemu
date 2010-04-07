#include "netemu_socket.h"
#include "protocol/communication.h"
#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include "test_util.h"
#include "network_test.h"
#include "application_test.h"
#include "communication_test.h"
#include <stdio.h>
#include <stdlib.h>

void test_server_communication();
void test_sender_receiver();

int main()
{
	netemu_init_network();
	run_application_tests();
	return 0;
}
