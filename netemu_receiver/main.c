#include "netemu_socket.h"
#include <stdio.h>
#include <stdlib.h>
void receive_data(NETEMU_SOCKET);

int main()
{
	int error;
	NETEMU_SOCKET socket;
	struct netemu_sockaddr_in addr;
	addr.addr = htonl(INADDR_ANY);
	addr.family = NETEMU_AF_INET;
	addr.port = 27015;
	printf("RECEIVER\n");

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

	receive_data(socket);

	return 0;
}

void receive_data(NETEMU_SOCKET socket) {
	unsigned char buffer[4];
	int data_received;
	int size = 4;
	unsigned long i = 0;
	int received[100000];
	
	while(i < 99999) {
		data_received = netemu_recv(socket, buffer, size, 0); 
		i = *((unsigned long*)buffer);
		if(data_received < 0){
			printf("RECV: %i", netemu_get_last_error());
		}
		else if(data_received > 0)
		{
			received[i] = 1;
		}
	}

	for(i = 0; i < 100000; i++)
		if(!received[i])
		{
			printf("OH NOES!");
		}
}
