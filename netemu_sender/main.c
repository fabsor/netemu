#include "netemu_socket.h"
#include "application_test.h"
#include <stdio.h>
#include <stdlib.h>

void test_server_communication();
void test_sender_receiver();

int main()
{
	netemu_init_network();
	run_application_tests();
	while(1);
  	return 0;
}
