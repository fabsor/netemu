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
#pragma once
#ifndef _NETEMU_ERROR_H
#define	_NETEMU_ERROR_H

#ifdef	__cplusplus
extern "C" {
#endif

/* *** SOCKET ERROR CODES *** */

#ifdef _NIX

#include <errno.h>

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

#define NETLIB_ENOTINITIALIZED	WSANOTINITIALISED
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
#define NETEMU_ENOTENOUGHMEMORY		40000
#define NETEMU_EUNKNOWNERROR		40001
#define NETEMU_EPATHNOTFOUND		40002
#define NETEMU_EINVALIDHANDLE		40003
#define NETEMU_EINVALIDSERVERLIST 	40004

/**
 * @todo
 * We need to be able to register a callback function
 * that get's called every time an error occurs. This way,
 * we will be able to notify the developer about errors
 * even when they occur internally.
 */
int netlib_get_last_error();
int netlib_get_last_platform_error();
const char *netlib_get_last_error_string();
void netlib_set_last_mapped_error(int error);
void netlib_set_last_error(int error);

#ifdef	__cplusplus
}
#endif

#endif	/* _NETEMU_ERROR_H */

