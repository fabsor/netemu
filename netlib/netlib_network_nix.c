/*
 *   This file is part of netlib.
 *
 *   Netlib is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netlib is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netlib.  If not, see <http://www.gnu.org/licenses/>
 */

#include "headers/netlib_network.h"
#include "headers/netlib_error.h"
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

int initialized;
void _fill_netlib_addrinfo(struct addrinfo* addrinfo, struct netlib_addrinfo *netemuinfo);

/* Initializes the socket usage for the application on the platform. */
int netlib_init_network() {
    initialized = 1;
    return 0;
    
}

/* Creates a new NETEMU_SOCKET. */
NETLIB_SOCKET netlib_socket(int address_family, int socket_type) {
	int error;

    error = socket(address_family,socket_type,0);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* Binds a given NETEMU_SOCKET to an address. */
int netlib_bind(NETLIB_SOCKET socket, const netlib_sockaddr *address, socklen_t address_len) {
	int error;

    error = bind(socket,address,address_len);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* Places a socket in a state where it listens for incoming connection attempts. */
int netlib_listen(NETLIB_SOCKET socket, int backlog) {
	int error;

    error = listen(socket,backlog);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* Accepts a connection attempt made on the listening socket. */
NETLIB_SOCKET netlib_accept(NETLIB_SOCKET socket, netlib_sockaddr *address, socklen_t *address_len) {
	int error;

    error = accept(socket,address,address_len);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* We have to use this ugly function until we can decide to remove netemu_prepare_addr.*/
NETLIB_SOCKET netlib_accept_inet(NETLIB_SOCKET socket,netlib_sockaddr** address, socklen_t *address_len) {
    struct sockaddr_in addr;
    socklen_t size;
    int error;
    size = sizeof(addr);
    error = accept(socket,(struct sockaddr*)&addr,&size);

    if(error == -1) {
    	netlib_set_last_error(errno);
    	return error;
    }
    *address = malloc(size);
    memcpy(*address, &addr, size);
    *address_len = size;

    return error;
}

/* Sends data through a connected socket. */
int netlib_send(NETLIB_SOCKET socket, const char *buffer, int len, int flags) {
    int error;
	error = send(socket,buffer,len,flags);
	if(error == -1) {
		netlib_set_last_error(errno);
	}
	return error;
}

/* Sends data to a specific destination. */
int netlib_sendto(NETLIB_SOCKET socket, const char *buffer, int len, int flags, const netlib_sockaddr *dest_address, socklen_t address_len){
    int error;
	error = sendto(socket,buffer,len,flags,dest_address,address_len);
	if(error == -1) {
		netlib_set_last_error(errno);
	}
	return error;
}

/*! Sets the blocking status of the socket */
int netlib_set_blocking(NETLIB_SOCKET socket, int blocking) {
	return ioctl(socket, FIONBIO, &blocking);
}

/* Receives data on a connected socket. */
int netlib_recv(NETLIB_SOCKET socket, char *buffer, int len, int flags) {
	int error;

    error = recv(socket,buffer,len,flags);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* Received a datagram and stores the sender address */
int netlib_recvfrom(NETLIB_SOCKET socket, char *buffer, int len, int flags, netlib_sockaddr *address, socklen_t *address_len) {
	int error;

    error = recvfrom(socket,buffer,len,flags,address,address_len);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

/* Disables send or receive on a socket. */
int netlib_shutdown(NETLIB_SOCKET socket, int how) {
	int error;

    error = shutdown(socket,how);
    if(error == -1) {
    	netlib_set_last_error(errno);
    }
    return error;
}

int netlib_closesocket(NETLIB_SOCKET socket) {
	int error;
	error = shutdown(socket, SHUT_RDWR);
	if(error == -1) {
		netlib_set_last_error(errno);
	}
	error = close(socket);
	if(error == -1) {
		netlib_set_last_error(errno);
	}
	return error;
}

int netlib_get_addr_info(char* nodename, char* servicetype, const struct netlib_addrinfo* hints, struct netlib_addrinfo** result) {
	struct addrinfo* info;
	struct addrinfo info_hints;
	struct addrinfo* info_hints_ptr;
	struct netlib_addrinfo *addr_result, *iter;
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
	if(error < 0) {
		netlib_set_last_error(error);
		return -1;
	}

	if((addr_result = malloc(sizeof(struct netlib_addrinfo))) == NULL) {
		netlib_set_last_error(0);
		return -1;
	}
	_fill_netlib_addrinfo(info,addr_result);
	info = info->ai_next;
	iter = addr_result->next;
	while(info != NULL) {
		/* TODO: Check for out-of-memory errors here */
		iter = malloc(sizeof(struct netlib_addrinfo));
		if(iter == NULL) {
			netlib_set_last_error(errno);
			return -1;
		}
		_fill_netlib_addrinfo(info, iter);
		iter = iter->next;
		info = info->ai_next;
	}

	*result = addr_result;
	return 0;
}

void _fill_netlib_addrinfo(struct addrinfo* addrinfo, struct netlib_addrinfo *netemuinfo) {
	netemuinfo->addrlen = addrinfo->ai_addrlen;
	if(addrinfo->ai_canonname != NULL)
		netemuinfo->hostname = addrinfo->ai_canonname;
	netemuinfo->addr = (netlib_sockaddr*)addrinfo->ai_addr;
}

/**
 * Converts a string containing a dotted IPv4 address to a network order unsigned long.
 * @param addr the address to convert.
 * @return the address in network byte order.
 */
unsigned long netlib_inet_addr(char* addr) {
	return inet_addr(addr);
}


int netlib_connect(NETLIB_SOCKET socket, const netlib_sockaddr *address, socklen_t address_len) {
	int error;

	error = connect(socket,address,address_len);
	if(error == -1) {
		netlib_set_last_error(errno);
	}
	return error;
}


/**
 * Converts an unsigned long from host order to network order.
 * @param value the value to convert
 * @return the value in network byte order.
 */
unsigned long netlib_htonl(unsigned long value) {
	return htonl(value);
}

/**
 * Converts an unsigned long from network order to host order.
 * @param value the value to convert.
 * @return the value in host order.
 */
unsigned long netlib_ntohl(unsigned long value) {
	return ntohl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned short netlib_htons(unsigned short value) {
	return htons(value);
}
