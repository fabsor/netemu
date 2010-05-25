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

/**
 * @file
 * Header for the netlib socket component.
 */

/**
 * @defgroup netlib_network network tools.
 * Various functions for handling networks.
 */
#pragma once
#ifndef _NETEMU_SOCKET_H
#define	_NETEMU_SOCKET_H

#ifdef	__cplusplus
extern "C" {
#endif

	/*  Platform-specific socket typedefs.
	* Unix uses signed integers for the socket file descriptor,
	* while windows uses an unsigned integer.
	* TODO: Since there are no namespaces, we need to prefix all these with NETEMU_
	* also, wouldn't it be better to define them to their constants instead?
	*/
#ifdef _NIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>

typedef int NETLIB_SOCKET;
#define NETLIB_INVALID_SOCKET  (NETLIB_SOCKET)(-1)

	/* Error code constants */
#define NETLIB_EINTR			EINTR
#define NETLIB_EWOULDBLOCK		EWOULDBLOCK
#define NETLIB_EACCES			EACCES
#define NETLIB_EFAULT			EFAULT
#define NETLIB_EINVAL			EINVAL
#define NETLIB_EMFILE			EMFILE
#define NETLIB_ENOTSOCK			ENOTSOCK
#define NETLIB_EDESTADDRREQ		EDESTADDRREQ
#define NETLIB_EMSGSIZE			EMSGSIZE
#define NETLIB_EPROTOTYPE		EPROTOTYPE
#define NETLIB_EPROTONOSUPPORT	EPROTONOSUPPORT
#define NETLIB_ESOCKNOSUPPORT	ESOCKNOSUPPORT
#define NETLIB_EOPNOTSUPP		EOPNOTSUPP
#define NETLIB_EAFNOSUPP		EAFNOSUPP
#define NETLIB_EADDRINUSE		EADDRINUSE
#define NETLIB_EADDRNOTAVAIL	EADDRNOTAVAIL
#define NETLIB_ENETDOWN			ENETDOWN
#define NETLIB_ENETUNREACH		ENETUNREACH
#define NETLIB_ENETRESET		ENETRESET
#define NETLIB_ECONNABORTED		ECONNABORTED
#define NETLIB_ECONNRESET		ECONNRESET
#define NETLIB_ENOBUFS			ENOBUFS
#define NETLIB_EISCONN			EISCONN
#define NETLIB_ENOTCONN			ENOTCONN
#define NETLIB_ESHUTDOWN		ESHUTDOWN
#define NETLIB_ETIMEOUT			ETIMEOUT
#define NETLIB_ECONNREFUSED		ECONNREFUSED
#define NETLIB_EHOSTUNREACH		EHOSTUNREACH
#define NETLIB_EALREADY			EALREADY
#define NETLIB_EINPROGRESS		EINPROGRESS
#define NETLIB_ENOTINITIALIZED	ENOTINITIALIZED

#else

#include <ws2tcpip.h>

typedef unsigned int NETLIB_SOCKET;
#define NETLIB_INVALID_SOCKET  (NETEMU_SOCKET)(~0)

#define NETLIB_ENOTINITIALIZED	WSANOTINITIALIZED
#define NETLIB_EINTR			WSAEINTR
#define NETLIB_EWOULDBLOCK		WSAEWOULDBLOCK
#define NETLIB_EACCES			WSAEACCES
#define NETLIB_EFAULT			WSAEFAULT
#define NETLIB_EINVAL			WSAEINVAL
#define NETLIB_EMFILE			WSAEMFILE
#define NETLIB_ENOTSOCK			WSAENOTSOCK
#define NETLIB_EDESTADDRREQ		WSAESTADDRREQ
#define NETLIB_EMSGSIZE			WSAEMSGSIZE
#define NETLIB_EPROTOTYPE		WSAEPROTOTYPE
#define NETLIB_EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define NETLIB_ESOCKNOSUPPORT	WSAESOCKNOSUPPORT
#define NETLIB_EOPNOTSUPP		WSAEOPNOTSUPP
#define NETLIB_EAFNOSUPP		WSAEAFNOSUPP
#define NETLIB_EADDRINUSE		WSAEADDRINUSE
#define NETLIB_EADDRNOTAVAIL	WSAEADDRNOTAVAIL
#define NETLIB_ENETDOWN			WSAENETDOWN
#define NETLIB_ENETUNREACH		WSAENETUNREACH
#define NETLIB_ENETRESET		WSAENETRESET
#define NETLIB_ECONNABORTED		WSAECONNABORTED
#define NETLIB_ECONNRESET		WSAECONNRESET
#define NETLIB_ENOBUFS			WSAENOBUFS
#define NETLIB_EISCONN			WSAEISCONN
#define NETLIB_ENOTCONN			WSAENOTCONN
#define NETLIB_ESHUTDOWN		WSAESHUTDOWN
#define NETLIB_ETIMEOUT			WSAETIMEOUT
#define NETLIB_ECONNREFUSED		WSAECONNREFUSED
#define NETLIB_EHOSTUNREACH		WSAEHOSTUNREACH
#define NETLIB_EALREADY			WSAEALREADY
#define NETLIB_EINPROGRESS		WSAEINPROGRESS
#define NETEMU_ESYSNOTREADY		WSASYSNOTREADY
#define NETEMU_EVERNOTSUPPORTED	WSAVERNOTSUPPORTED
#define NETEMU_EPROCLIM			WSAEPROCLIM
#endif

	/* Constants for the different address families that
	* can be specified when creating a new socket. */

#define NETLIB_AF_APPLETALK AF_APPLETALK
#define NETLIB_AF_BLUETOOTH AF_BLUETOOTH
#define NETLIB_AF_INET  AF_INET
#define NETLIB_AF_INET6 AF_INET6
#define NETLIB_AF_IPX   AF_IPX
#define NETLIB_AF_IRDA  AF_IRDA
#define NETLIB_AF_UNSPEC    AF_UNSPEC


	/* Constans for the different socket types that
	* can be specified when creating a new socket. */
#define NETLIB_SOCK_DGRAM   SOCK_DGRAM
#define NETLIB_SOCK_RAW     SOCK_RAW
#define NETLIB_SOCK_RDM     SOCK_RDM
#define NETLIB_SOCK_STREAM  SOCK_STREAM

	/* Constans for the different  */


	/* Constants defining flags for the send/recv calls. */

#define NETLIB_MSG_OOB          MSG_OOB        /* Process Out-of-band data (send/recv). */
#define NETLIB_MSG_DONTROUTE    MSG_DONTROUTE  /* Don't use local routing (send). */
#define NETLIB_MSG_PEEK         MSG_PEEK       /* Peeks at incoming data (recv). */
#define  NETLIB_MSG_WAITALL     MSG_WAITALL     /* Incoming data will be received only when the buffer is full, or when the connection is closed or an error occured (recv). */


	/* Constants for the shutdown method. */

#define NETLIB_SHUT_RD      SHUT_RD     /* Disables further data receival on the socket. */
#define NETLIB_SHUT_WR      SHUT_WR     /* Disables further data sending from the socket. */
#define NETLIB_SHUT_RDWR    SHUT_RDWR   /* Disables sending and receiving data on the socket. */


#define NETLIB_INADDR_ANY		INADDR_ANY
#define NETLIB_INADDR_LOOPBACK	INADDR_LOOPBACK

	/**
	* Structure describing a generic socket address.
	* Hopefully this is identical on both the unix and win32 platform, not yet verified though.
	*/
	/*
	struct netemu_sockaddr {
	unsigned short sa_family;   // THe address family of the socket.
	char sa_data[14];           // Address info.
	};
	*/
	typedef struct sockaddr netlib_sockaddr;

	typedef struct sockaddr_storage netlib_sockaddr_storage;

	typedef struct sockaddr_in netlib_sockaddr_in;

	typedef struct sockaddr_in6 netlib_sockaddr_in6;


	/*! Winsock has a handy struct which contains address information of a specific host. This struct
	* aims to replicate this.*/
	struct netlib_addrinfo {
		int ai_flags;       /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
		int ai_family;      /* PF_xxx */
		int ai_socktype;    /* SOCK_xxx */
		int ai_protocol;    /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
		size_t addrlen;
		char* hostname;
		netlib_sockaddr* addr;
		struct netlib_addrinfo* next;
	};



	/* Method definitions */

	/*! Initializes the socket usage for the application on the platform. */
	int netlib_init_network();

	/*! Creates a new NETEMU_SOCKET. */
	NETLIB_SOCKET netlib_socket(int address_family, int socket_type);

	int netlib_set_blocking(NETLIB_SOCKET socket, int blocking);

	/*! Binds a given NETEMU_SOCKET to an address. */
	int netlib_bind(NETLIB_SOCKET socket, const netlib_sockaddr *address, socklen_t address_len);

	/*! Connects a given NETEMU_SOCKET to an address. */
	int netlib_connect(NETLIB_SOCKET socket, const netlib_sockaddr *address, socklen_t address_len);

	/*! Places a socket in a state where it listens for incoming connection attempts. */
	int netlib_listen(NETLIB_SOCKET socket, int backlog);

	/* Accepts a connection attempt made on the listening socket. */
	NETLIB_SOCKET netlib_accept(NETLIB_SOCKET socket, netlib_sockaddr *address, socklen_t *address_len);

	/* Sends data through a connected socket. */
	int netlib_send(NETLIB_SOCKET socket, const char *buffer, int len, int flags);

	/* Sends data to a specific destination. */
	int netlib_sendto(NETLIB_SOCKET socket, const char *buffer, int len, int flags, const netlib_sockaddr *dest_address, socklen_t address_len);

	/* Receives data on a connected socket. */
	int netlib_recv(NETLIB_SOCKET socket, char *buffer, int len, int flags);

	/* Receives a data and stores the sender address */
	int netlib_recvfrom(NETLIB_SOCKET socket, char *buffer, int len, int flags, netlib_sockaddr *address, socklen_t *address_len);

	/* Disables send or receive on a socket. */
	int netlib_shutdown(NETLIB_SOCKET socket, int how);

	NETLIB_SOCKET netlib_accept_inet(NETLIB_SOCKET socket,netlib_sockaddr** address, socklen_t *address_len);

	/* Close socket*/
	int netlib_closesocket(NETLIB_SOCKET socket);

	int netlib_get_addr_info(char* nodename, char* servicetype, const struct netlib_addrinfo* hints, struct netlib_addrinfo** result);

	/**
	 * Free an netemu_addr_info struct.
	 */
	void netemu_free_addr_info(struct netlib_addrinfo *info);

	/**
	 * Converts a string containing a dotted IPv4 address to a network order unsigned long.
	 * @param addr the address to convert.
	 * @return the address in network byte order.
	 */
	unsigned long netlib_inet_addr(char* addr);

	/**
	 * Converts an unsigned long from host order to network order.
	 * @param value the value to convert
	 * @return the value in network byte order.
	 */
	unsigned long netlib_htonl(unsigned long value);

	/**
	 * Converts an unsigned long from network order to host order.
	 * @param value the value to convert.
	 * @return the value in host order.
	 */
	unsigned long netlib_ntohl(unsigned long value);

	/**
	 * Converts an unsigned long from network order to host order.
	 */
	unsigned short netlib_htons(unsigned short value);

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

