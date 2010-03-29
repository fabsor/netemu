#include "../netlib/netemu_socket.h"
#include <stdio.h>

void receive_data(NETEMU_SOCKET);

int main()
{
	int error;
	unsigned int h;
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
	char* buffer;
	int data_received;
	int size = 128;
	buffer = malloc(size);
	
	while(1) {
		data_received = netemu_recv(socket, buffer, size, 0); 
		if(data_received < 0){
			printf("RECV: %i", netemu_get_last_error());
		}
		else if(data_received > 0)
		{
			printf("%c", buffer[0]);
		}
	}
}