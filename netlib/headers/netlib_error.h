/* 
 * File:   netlib_error.h
 * Author: emil
 *
 * Created on den 26 april 2010, 15:55
 */

#ifndef _NETEMU_ERROR_H
#define	_NETEMU_ERROR_H

#ifdef	__cplusplus
extern "C" {
#endif

/* *** SOCKET ERROR CODES *** */

#ifdef _NIX
//#include <netinet/in.h>
//#include <sys/socket.h>
//#include <sys/un.h>
//#include <netdb.h>

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
#define NETEMU_ETIMEDOUT		ETIMEDOUT



#define NETEMU_EAGAIN			EAI_AGAIN
#define NETEMU_BADFLAGS			EAI_BADFLAGS
#define NETEMU_EFAIL			EAI_FAIL
#define NETEMU_EAFNOSUPPORT		EAFNOSUPPORT
#define NETEMU_ENONAME			EAI_NONAME
#define NETEMU_ESERVICE			EAI_SERVICE
#define NETEMU_ESOCKTYPE		EAI_SOCKTYPE
#define NETEMU_ENODATA			EAI_NODATA

#else

//#include <winsock2.h>
//#include <ws2tcpip.h>

#define NETEMU_ENOTINITIALIZED	WSANOTINITIALISED
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
#define NETEMU_ETIMEDOUT		WSAETIMEDOUT

#define NETEMU_EAGAIN			WSATRY_AGAIN
#define NETEMU_BADFLAGS			WSAEINVAL
#define NETEMU_EFAIL			WSANO_RECOVERY
#define NETEMU_EAFNOSUPPORT		WSAEAFNOSUPPORT
#define NETEMU_ENONAME			WSAHOST_NOT_FOUND
#define NETEMU_ESERVICE			WSATYPE_NOT_FOUND
#define NETEMU_ESOCKTYPE		WSAESOCKTNOSUPPORT
#define NETEMU_ENODATA			WSANO_DATA

#endif

/* *** GENERAL ERROR CODES *** */
#define NETEMU_ENOTENOUGHMEMORY	40000
#define NETEMU_EUNKOWNERROR		40001
#define NETEMU_EPATHNOTFOUND	40002
#define NETEMU_EINVALIDHANDLE	40003


int netlib_get_last_error();
void netlib_set_last_mapped_error(int error);
void netlib_set_last_error(int error);

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_ERROR_H */

