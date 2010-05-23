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

#include "headers/netlib_error.h"
#include <errno.h>
#include <netdb.h>

int last_error;

int netlib_get_last_error() {
	return last_error;
}

void netlib_set_last_error(int error) {
	last_error = error;
}

const char *netlib_get_last_error_string() {
	const char *message;
	if(last_error < 0)
		message = gai_strerror(last_error);
	else
		message = strerror(last_error);

	return message;
}

void netlib_set_last_mapped_error(int error) {

}

int netlib_get_last_platform_error() {
	return errno;
}
