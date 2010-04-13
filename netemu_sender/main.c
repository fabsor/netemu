#include "netemu_socket.h"
#include "application_test.h"
#include <stdio.h>
#include <stdlib.h>

void test_server_communication();
void test_sender_receiver();

int main()
{
	struct netemu_addrinfo* result = NULL;
	netemu_init_network();

	netemu_get_addr_info("www.kaillera.com", NULL, NULL, &result);

	run_application_tests();
	while(1);
  	return 0;
}
