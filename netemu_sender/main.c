#include "netemu_socket.h"
#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include <stdio.h>
void send_data();

void receive_data();
int main()
{
	netemu_init_network();
	receive_data();
	send_data();
	return 0;
}

void listener(char* data, size_t size, struct netemu_receiver* receiver) {
	printf("%s",data);
}

void receive_data() {
	struct netemu_sockaddr_in addr_in;
	netemu_sockaddr *addr;
	struct netemu_receiver *receiver;
	addr_in.addr = htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = 27015;

	addr = netemu_prepare_net_addr(&addr_in);
	receiver = netemu_receiver_new(addr,sizeof(addr_in),64);
	netemu_receiver_register_recv_fn(receiver, listener);
	netemu_receiver_start_listening(receiver);
}

void send_data(){
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	int error;
	addr_in.addr = INADDR_LOOPBACK;
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = 27015;

	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new(addr,sizeof(addr_in));
	while (1) {
		error = netemu_sender_send(sender,"Hello to you old friend.",64);
		if(error < 0) {
			printf("Error: %d\n", netemu_get_last_error());
		}
	}
}

/*
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
			printf("Error: %d\n", netemu_get_last_error());
		}
	}
}
*/
