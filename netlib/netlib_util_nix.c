/*
 * netemu_util_nix.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
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
