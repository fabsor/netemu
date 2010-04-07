/*
 * netemu_util.h
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */

#ifndef NETEMU_UTIL_H_
#define NETEMU_UTIL_H_
#include <stdlib.h>
#ifdef _NIX
typedef unsigned int NETEMU_DWORD;
#else
typedef unsigned long NETEMU_DWORD;
#endif
typedef unsigned short NETEMU_WORD;

void netemu_snprintf(char* dest,size_t size, char* format, void* data);


#endif /* NETEMU_UTIL_H_ */

