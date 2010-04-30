#include "netemu_socket.h"

#include "netlib_error.h"

int netemu_init_network() {
    int errcode;
    WSADATA wsaData;
    int retval;

    retval = 0;
    errcode = WSAStartup(MAKEWORD(2, 2), &wsaData);

    /* WSAStartup returns the errorcode directly if something went wrong.
     * For consistency, we'll set that to be the last error code and return -1. */
    if(errcode != 0) {
		netlib_set_last_error(errcode);
        retval = -1;
    }

    return retval;
}

int netemu_cleanup() {
    int errcode;
	int retval;

	retval = 0;
	errcode = WSACleanup();
	if(errcode == SOCKET_ERROR) {
		netlib_set_last_error(WSAGetLastError());
		retval = -1;
	}

	return retval;
}

NETEMU_SOCKET netemu_socket(int address_family, int socket_type) {
	NETEMU_SOCKET return_socket;
    return_socket = socket(address_family, socket_type, 0);
	if(return_socket == INVALID_SOCKET)
		netlib_set_last_error(WSAGetLastError());
	
	return return_socket;
}

int netemu_bind(NETEMU_SOCKET socket, const struct sockaddr *address, socklen_t address_len) {
	int errcode;
    
	errcode = bind(socket, address, address_len);
	if(errcode != 0)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_connect(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len) {
	int errcode;

	errcode = connect(socket, address, address_len);
	if(errcode != 0)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_listen(NETEMU_SOCKET socket, int backlog) {
	int errcode;

    errcode = listen(socket, backlog);
	if(errcode != 0)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, struct sockaddr *address, socklen_t *address_len) {
	int errcode;

    errcode = accept(socket, address, address_len);
	if(errcode == INVALID_SOCKET)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags) {
	int errcode;
    
	errcode = send(socket, buffer, len, flags);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const struct sockaddr *dest_address, socklen_t address_len) {
    int errcode;
	
	errcode = sendto(socket, buffer, len, flags, dest_address, address_len);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags) {
	int errcode;

	errcode = recv(socket, buffer, len, flags);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, struct sockaddr *address, socklen_t *address_len) {
	int errcode;

    errcode = recvfrom(socket, buffer, len, flags, address, address_len);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_shutdown(NETEMU_SOCKET socket, int how) {
	int errcode;

    errcode = shutdown(socket, how);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_closesocket(NETEMU_SOCKET socket) {
	int errcode;

	errcode = closesocket(socket);
	if(errcode == SOCKET_ERROR)
		netlib_set_last_error(WSAGetLastError());

	return errcode;
}

int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result) {
	PADDRINFOA addrinfo = NULL;
	int errcode;
	struct netemu_addrinfo *result_addrinfo;
	struct addrinfo info_hints;

	if(hints == NULL) {
		errcode = getaddrinfo(nodename, servicetype, NULL, &addrinfo);
	}
	else {
		memset(&info_hints, 0, sizeof(struct addrinfo));
		info_hints.ai_family = hints->ai_family;
		info_hints.ai_socktype = hints->ai_socktype;
		info_hints.ai_protocol = hints->ai_protocol;
		errcode = getaddrinfo(nodename, servicetype, &info_hints, &addrinfo);
	}
	
	if(errcode != 0) {
		/* If a memory error occured, we map it to our general out-of-memory error code for consistency, 
		 * otherwise, we just use the WSA error code directly */
		if(errcode == WSA_NOT_ENOUGH_MEMORY) 
			netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		else
			netlib_set_last_error(errcode);
		
		return -1;
	}
	

	if((*result = malloc(sizeof(struct netemu_addrinfo))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return -1;
	}

	result_addrinfo = *result;
	while(addrinfo != NULL) {
		result_addrinfo->addr = addrinfo->ai_addr;
		result_addrinfo->hostname = addrinfo->ai_canonname;
		result_addrinfo->addrlen = addrinfo->ai_addrlen;
		result_addrinfo->ai_family = addrinfo->ai_family;
		result_addrinfo->ai_flags = addrinfo->ai_flags;
		result_addrinfo->ai_protocol = addrinfo->ai_protocol;
		result_addrinfo->ai_socktype = addrinfo->ai_socktype;
		addrinfo = addrinfo->ai_next;
		if(addrinfo != NULL) {
			if((result_addrinfo->next = malloc(sizeof(struct netemu_addrinfo))) == NULL) {
				netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
				errcode = -1;
				break;
			}
			result_addrinfo = result_addrinfo->next;
		}
	}

	if(errcode != 0) {
		/* TODO: Free the addrinfo that we've created here! */
	}

	return errcode;
}

//int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result) {
//	PADDRINFOA addrinfo = NULL;
//	struct netemu_addrinfo *result_addrinfo;
//	getaddrinfo(nodename, servicetype, hints, &addrinfo);
//
//	*result = malloc(sizeof(struct netemu_addrinfo));
//
//	result_addrinfo = *result;
//	while(addrinfo != NULL) {
//		result_addrinfo->addr = addrinfo->ai_addr;
//		result_addrinfo->hostname = addrinfo->ai_canonname;
//		result_addrinfo->addrlen = addrinfo->ai_addrlen;
//		addrinfo = addrinfo->ai_next;
//		if(addrinfo != NULL) {
//			result_addrinfo->next = malloc(sizeof(struct netemu_addrinfo));
//			result_addrinfo = result_addrinfo->next;
//		}
//	}
//	
//}

netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr) {
	struct sockaddr_in* in_addr;
	if((in_addr = malloc(sizeof(struct sockaddr_in))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}
    in_addr->sin_port = netaddr->port;
    in_addr->sin_family = netaddr->family;
    in_addr->sin_addr.s_addr = netaddr->addr;
    return (netemu_sockaddr*) in_addr;
}

unsigned long netemu_inet_addr(char* addr) {
	return inet_addr(addr);
}

unsigned long netemu_htonl(unsigned long value) {
	return htonl(value);
}

unsigned long netemu_ntohl(unsigned long value) {
	return ntohl(value);
}

/* Converts an unsigned long from network order to host order. */
unsigned short netemu_htons(unsigned short value) {
	return htons(value);
}
