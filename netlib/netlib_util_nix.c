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

#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include "headers/netlib_util.h"

void netemu_snprintf(char* dest,size_t size, char* format, void* data) {
	snprintf(dest,size,format,data);
}

long netlib_get_time_millis() {
	struct timeval tim;
	gettimeofday(&tim, NULL);
	return tim.tv_usec;
}
