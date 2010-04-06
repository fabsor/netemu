/*
 * netemu_util_nix.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include <stdio.h>
#include <stdlib.h>
#include "headers/netemu_util.h"

void netemu_snprintf(char* dest,size_t size, char* format, void* data) {
	snprintf(dest,size,format,data);
}
