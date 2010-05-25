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
#ifndef NETLIB_UTIL_H_
#define NETLIB_UTIL_H_

#include <stdlib.h>
#ifdef _NIX
#include "inttypes.h"
typedef unsigned int NETEMU_DWORD;
#else
typedef unsigned long NETEMU_DWORD;
#endif
typedef unsigned short NETEMU_WORD;

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

typedef char NETEMU_BOOL;

void netlib_snprintf(char* dest,size_t size, char* format, void* data);

long netlib_get_time_millis();
#endif /* NETLIB_UTIL_H_ */

