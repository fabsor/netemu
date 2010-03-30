#include "netemu_socket.h"
#include <stdio.h>
//#include <arpa/inet.h>
void send_data(NETEMU_SOCKET);

int main()
{
	int error;
	NETEMU_SOCKET socket;
	struct netemu_sockaddr_in addr;
	addr.addr = htonl(INADDR_ANY);
	addr.family = NETEMU_AF_INET;
	addr.port = 0;


	printf("SENDER");

	error = netemu_init_network();
	if(error != 0) {
		printf("INIT: %i", netemu_get_last_error());
	}

	socket = netemu_socket(NETEMU_AF_INET, NETEMU_SOCK_DGRAM);
	if(socket == INVALID_SOCKET) {
		printf("OH NOES!\n %i", netemu_get_last_error());
	}

	error = netemu_bind(socket, netemu_prepare_net_addr(&addr), sizeof(addr));

	if(error != 0) {
		printf("Error: %d", netemu_get_last_error());
	}

	send_data(socket);
	return 0;
}

void send_data(NETEMU_SOCKET socket) {
	int error;
	unsigned long i = 0;
	struct netemu_sockaddr_in addr;
	int data_received;
	addr.addr = netemu_inet_addr("192.168.106.235");
	addr.family = NETEMU_AF_INET;
	addr.port = 27015;
	
	while(i <= 100000) {
		error = netemu_sendto(socket, &i, 4, 0, netemu_prepare_net_addr(&addr), sizeof(addr));
		i++;
		if(error < 0) {
			printf("Error: %s\n", netemu_get_last_error());
		}
	}
}
