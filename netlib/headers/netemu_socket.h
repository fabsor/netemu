/**
 * @file
 * Header for the netlib socket component.
 */

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

	typedef int NETEMU_SOCKET;
#define INVALID_SOCKET  (NETEMU_SOCKET)(-1)

	/* Error code constants */
#define NETEMU_EINTR			EINTR
#define NETEMU_EWOULDBLOCK		EWOULDBLOCK
#define NETEMU_EACCES			EACCES
#define NETEMU_EFAULT			EFAULT
#define NETEMU_EINVAL			EINVAL
#define NETEMU_EMFILE			EMFILE
#define NETEMU_ENOTSOCK			ENOTSOCK
#define NETEMU_EDESTADDRREQ		EDESTADDRREQ
#define NETEMU_EMSGSIZE			EMSGSIZE
#define NETEMU_EPROTOTYPE		EPROTOTYPE
#define NETEMU_EPROTONOSUPPORT	EPROTONOSUPPORT
#define NETEMU_ESOCKNOSUPPORT	ESOCKNOSUPPORT
#define NETEMU_EOPNOTSUPP		EOPNOTSUPP
#define NETEMU_EAFNOSUPP		EAFNOSUPP
#define NETEMU_EADDRINUSE		EADDRINUSE
#define NETEMU_EADDRNOTAVAIL	EADDRNOTAVAIL
#define NETEMU_ENETDOWN			ENETDOWN
#define NETEMU_ENETUNREACH		ENETUNREACH
#define NETEMU_ENETRESET		ENETRESET
#define NETEMU_ECONNABORTED		ECONNABORTED
#define NETEMU_ECONNRESET		ECONNRESET
#define NETEMU_ENOBUFS			ENOBUFS
#define NETEMU_EISCONN			EISCONN
#define NETEMU_ENOTCONN			ENOTCONN
#define NETEMU_ESHUTDOWN		ESHUTDOWN
#define NETEMU_ETIMEOUT			ETIMEOUT
#define NETEMU_ECONNREFUSED		ECONNREFUSED
#define NETEMU_EHOSTUNREACH		EHOSTUNREACH
#define NETEMU_EALREADY			EALREADY
#define NETEMU_EINPROGRESS		EINPROGRESS
#define NETEMU_ENOTINITIALIZED	ENOTINITIALIZED

#else

#include <winsock2.h>
#include <ws2tcpip.h>

typedef unsigned int NETEMU_SOCKET;
#define NETEMU_INVALID_SOCKET  (NETEMU_SOCKET)(~0)

#define NETEMU_ENOTINITIALIZED	WSANOTINITIALIZED
#define NETEMU_EINTR			WSAEINTR
#define NETEMU_EWOULDBLOCK		WSAEWOULDBLOCK
#define NETEMU_EACCES			WSAEACCES
#define NETEMU_EFAULT			WSAEFAULT
#define NETEMU_EINVAL			WSAEINVAL
#define NETEMU_EMFILE			WSAEMFILE
#define NETEMU_ENOTSOCK			WSAENOTSOCK
#define NETEMU_EDESTADDRREQ		WSAESTADDRREQ
#define NETEMU_EMSGSIZE			WSAEMSGSIZE
#define NETEMU_EPROTOTYPE		WSAEPROTOTYPE
#define NETEMU_EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define NETEMU_ESOCKNOSUPPORT	WSAESOCKNOSUPPORT
#define NETEMU_EOPNOTSUPP		WSAEOPNOTSUPP
#define NETEMU_EAFNOSUPP		WSAEAFNOSUPP
#define NETEMU_EADDRINUSE		WSAEADDRINUSE
#define NETEMU_EADDRNOTAVAIL	WSAEADDRNOTAVAIL
#define NETEMU_ENETDOWN			WSAENETDOWN
#define NETEMU_ENETUNREACH		WSAENETUNREACH
#define NETEMU_ENETRESET		WSAENETRESET
#define NETEMU_ECONNABORTED		WSAECONNABORTED
#define NETEMU_ECONNRESET		WSAECONNRESET
#define NETEMU_ENOBUFS			WSAENOBUFS
#define NETEMU_EISCONN			WSAEISCONN
#define NETEMU_ENOTCONN			WSAENOTCONN
#define NETEMU_ESHUTDOWN		WSAESHUTDOWN
#define NETEMU_ETIMEOUT			WSAETIMEOUT
#define NETEMU_ECONNREFUSED		WSAECONNREFUSED
#define NETEMU_EHOSTUNREACH		WSAEHOSTUNREACH
#define NETEMU_EALREADY			WSAEALREADY
#define NETEMU_EINPROGRESS		WSAEINPROGRESS
#define NETEMU_ESYSNOTREADY		WSASYSNOTREADY
#define NETEMU_EVERNOTSUPPORTED	WSAVERNOTSUPPORTED
#define NETEMU_EPROCLIM			WSAEPROCLIM
#endif

	/* Constants for the different address families that
	* can be specified when creating a new socket. */

#define NETEMU_AF_APPLETALK AF_APPLETALK
#define NETEMU_AF_BLUETOOTH AF_BLUETOOTH
#define NETEMU_AF_INET  AF_INET
#define NETEMU_AF_INET6 AF_INET6
#define NETEMU_AF_IPX   AF_IPX
#define NETEMU_AF_IRDA  AF_IRDA
#define NETEMU_AF_UNSPEC    AF_UNSPEC


	/* Constans for the different socket types that
	* can be specified when creating a new socket. */
#define NETEMU_SOCK_DGRAM   SOCK_DGRAM
#define NETEMU_SOCK_RAW     SOCK_RAW
#define NETEMU_SOCK_RDM     SOCK_RDM
#define NETEMU_SOCK_STREAM  SOCK_STREAM


	/* Constants defining flags for the send/recv calls. */

#define NETEMU_MSG_OOB          MSG_OOB        /* Process Out-of-band data (send/recv). */
#define NETEMU_MSG_DONTROUTE    MSG_DONTROUTE  /* Don't use local routing (send). */
#define NETEMU_MSG_PEEK         MSG_PEEK       /* Peeks at incoming data (recv). */
#define  NETEMU_MSG_WAITALL     MSG_WAITALL     /* Incoming data will be received only when the buffer is full, or when the connection is closed or an error occured (recv). */


	/* Constants for the shutdown method. */

#define NETEMU_SHUT_RD      SHUT_RD     /* Disables further data receival on the socket. */
#define NETEMU_SHUT_WR      SHUT_WR     /* Disables further data sending from the socket. */
#define NETEMU_SHUT_RDWR    SHUT_RDWR   /* Disables sending and receiving data on the socket. */


#define NETEMU_INADDR_ANY		INADDR_ANY
#define NETEMU_INADDR_LOOPBACK	INADDR_LOOPBACK

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
	typedef struct sockaddr netemu_sockaddr;

	struct netemu_sockaddr_in{
		unsigned short family;
		char sa_data[14];
		unsigned int addr;
		short port;
	};

	typedef struct sockaddr_in6 netemu_sockaddr_in6;


	/*! Winsock has a handy struct which contains address information of a specific host. This struct
	* aims to replicate this.*/
	struct netemu_addrinfo {
		char* hostname;
		size_t addrlen;
		struct netemu_sockaddr* addr;
		struct netemu_addrinfo** next;
	};



	/* Method definitions */

	/*! Initializes the socket usage for the application on the platform. */
	int netemu_init_network();

	/*! Creates a new NETEMU_SOCKET. */
	NETEMU_SOCKET netemu_socket(int address_family, int socket_type);

	/*! Binds a given NETEMU_SOCKET to an address. */
	int netemu_bind(NETEMU_SOCKET socket, const netemu_sockaddr *address, socklen_t address_len);

	/*! Places a socket in a state where it listens for incoming connection attempts. */
	int netemu_listen(NETEMU_SOCKET socket, int backlog);

	/* Accepts a connection attempt made on the listening socket. */
	NETEMU_SOCKET netemu_accept(NETEMU_SOCKET socket, netemu_sockaddr *address, socklen_t *address_len);

	/* Sends data through a connected socket. */
	int netemu_send(NETEMU_SOCKET socket, const char *buffer, int len, int flags);

	/* Sends data to a specific destination. */
	int netemu_sendto(NETEMU_SOCKET socket, const char *buffer, int len, int flags, const netemu_sockaddr *dest_address, socklen_t address_len);

	/* Receives data on a connected socket. */
	int netemu_recv(NETEMU_SOCKET socket, char *buffer, int len, int flags);

	/* Receives a data and stores the sender address */
	int netemu_recvfrom(NETEMU_SOCKET socket, char *buffer, int len, int flags, netemu_sockaddr *address, socklen_t *address_len);

	/* Disables send or receive on a socket. */
	int netemu_shutdown(NETEMU_SOCKET socket, int how);

	/* Frees the memory allocated for the socket */
	int netemu_free(NETEMU_SOCKET socket);

	/* Close socket*/
	int netemu_closesocket(NETEMU_SOCKET socket);
	/* Returns the error code for the last error that occured */
	int netemu_get_last_error();

	int netemu_get_addr_info(char* nodename, char* servicetype, const struct netemu_addrinfo* hints, struct netemu_addrinfo** result);

	netemu_sockaddr* netemu_prepare_net_addr(struct netemu_sockaddr_in *netaddr);

	/* Converts a string containing a dotted IPv4 address to a network order unsigned long. */
	unsigned long netemu_inet_addr(char* addr);

	/* Converts an unsigned long from host order to network order. */
	unsigned long netemu_htonl(unsigned long value);

	/* Converts an unsigned long from network order to host order. */
	unsigned long netemu_ntohl(unsigned long value);

	/* Converts an unsigned long from network order to host order. */
	unsigned long netemu_htons(unsigned long value);

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_SOCKET_H */

