#include "headers/netemu_socket.h"
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

int initialized;
void _fill_netemu_addrinfo(struct addrinfo* addrinfo, struct netemu_addrinfo *netemuinfo);

/* Initializes the socket usage for the application on the platform. */
int netemu_init_network() {
    initialized = 1;
    return 0;
    
}

/* Creates a new NETEMU_SOCKET. */
NETEMU_SOCKET netemu_socket(int address_family, int socket_type) {
    return socket(address_family,socket_type,0);

}

/* Binds a given NETEMU_SOCKET to an address. */
int netemu_bind(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
    return bind(socket,address,address_len);
}

/* Places a socket in a state where it listens for incoming connection attempts. */
int netemu_listen(NETEMU_SOCKET socket, int backlog) {
    return listen(socket,backlog);
}

/* Accepts a connection attempt made on the listening socket. */
NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, netemu_sockaddr *address, socklen_t *address_len) {
    return accept(socket,address,address_len);
}

/* Sends data through a connected socket. */
int netmu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags) {
    return send(socket,buffer,len,flags);
}

/* Sends data to a specific destination. */
int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const netemu_sockaddr *dest_address, socklen_t address_len){
    return sendto(socket,buffer,len,flags,dest_address,address_len);
}

/* Receives data on a connected socket. */
int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags) {
    return recv(socket,buffer,len,flags);
}

/* Received a datagram and stores the sender address */
int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, netemu_sockaddr *address, socklen_t *address_len) {
    return recvfrom(socket,buffer,len,flags,address,address_len);
}

/* Disables send or receive on a socket. */
int netemu_shutdown(NETEMU_SOCKET socket, int how) {
    return shutdown(socket,how);
}

int netemu_closesocket(NETEMU_SOCKET socket) {
	return close(socket);
}

/* Returns the error code for the last error that occured */
int netemu_get_last_error() {
  return errno;
}

int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result) {
	struct addrinfo* info;

	struct addrinfo info_hints;
	struct addrinfo* info_hints_ptr;
	struct netemu_addrinfo *addr_result, *iter;
	int error;
	if(hints == NULL) {
		info_hints_ptr = NULL;
	}
	else {
		info_hints.ai_family = hints->ai_family;
		info_hints.ai_socktype = hints->ai_socktype;
		info_hints.ai_protocol = hints->ai_protocol;
		info_hints_ptr = &info_hints;
	}
	error = getaddrinfo(nodename,servicetype,info_hints_ptr,&info);
	if(error == -1) {
		return error;
	}
	addr_result = malloc(sizeof(struct netemu_addrinfo));
	_fill_netemu_addrinfo(info,addr_result);
	info = info->ai_next;
	iter = addr_result->next;
	while(info != NULL) {
		iter = malloc(sizeof(struct netemu_addrinfo));
		_fill_netemu_addrinfo(info, iter);
		iter = iter->next;
		info = info->ai_next;
	}

	*result = addr_result;
	return error;
}

void _fill_netemu_addrinfo(struct addrinfo* addrinfo, struct netemu_addrinfo *netemuinfo) {
	netemuinfo->addrlen = addrinfo->ai_addrlen;
	if(addrinfo->ai_canonname != NULL)
		netemuinfo->hostname = addrinfo->ai_canonname;
	netemuinfo->addr = (netemu_sockaddr*)addrinfo->ai_addr;
}

unsigned long netemu_inet_addr(char* addr) {
	return inet_addr(addr);
}

netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr){
    struct sockaddr_in* addr;
    addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_port = netaddr->port;
    addr->sin_family = netaddr->family;
    addr->sin_addr.s_addr = netaddr->addr;
    
    return (netemu_sockaddr*) addr;
}

int netemu_connect(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
	return connect(socket,address,address_len);
}


/* Converts an unsigned long from host order to network order. */
unsigned long netemu_htonl(unsigned long value) {
	return htonl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned long netemu_ntohl(unsigned long value) {
	return ntohl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned short netemu_htons(unsigned short value) {
	return htons(value);
}
