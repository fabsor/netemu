#include "netemu_socket.h"
#include "protocol/communication.h"
#include "network/netemu_receiver.h"
#include "network/netemu_sender.h"
#include <stdio.h>

#define SERVER_PORT	27888
#define CLIENT_PORT 35888

NETEMU_SOCKET receive_data();
void send_data(NETEMU_SOCKET socket);

int main()
{
	NETEMU_SOCKET socket;
	netemu_init_network();
	socket = receive_data();
	send_data(socket);
	return 0;
}

void listener(char* data, size_t size, struct netemu_receiver* receiver) {
	printf("%s",data);
}

NETEMU_SOCKET receive_data() {
	struct netemu_sockaddr_in addr_in;
	netemu_sockaddr *addr;
	struct netemu_receiver *receiver;
	addr_in.addr = netemu_htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = htons(CLIENT_PORT);

	addr = netemu_prepare_net_addr(&addr_in);
	receiver = netemu_receiver_new(addr,sizeof(addr_in),64);
	netemu_receiver_register_recv_fn(receiver, listener);
	netemu_receiver_start_listening(receiver);
	return receiver->socket;
}

void send_data(NETEMU_SOCKET socket){
	struct netemu_sockaddr_in addr_in;
	struct netemu_sender* sender;
	netemu_sockaddr *addr;
	char* message;
	int error;
	addr_in.addr = netemu_htonl(INADDR_LOOPBACK);
	addr_in.family = NETEMU_AF_INET;
	addr_in.port = htons(SERVER_PORT);

	addr = netemu_prepare_net_addr(&addr_in);
	sender = netemu_sender_new_on_socket(addr,socket,sizeof(addr_in));
	message = netemu_communication_create_ping_message();

	while (1) {
		error = netemu_sender_send(sender,message,strlen(message)+1);
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
