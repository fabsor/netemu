/*
 * netemu_stringbuilder.h
 *
 *  Created on: 15 apr 2010
 *      Author: emil
 */

#ifndef NETEMU_STRINGBUILDER_H_
#define NETEMU_STRINGBUILDER_H_

#include "../netemu_util.h"

struct netemu_stringbuilder {
	NETEMU_DWORD count;
	NETEMU_DWORD capacity;
	char *string;
};

struct netemu_stringbuilder *netemu_stringbuilder_new(NETEMU_DWORD capacity);
void netemu_stringbuilder_free(struct netemu_stringbuilder *builder);

int netemu_stringbuilder_append_chars(struct netemu_stringbuilder *builder, char* string, NETEMU_DWORD len);
int netemu_stringbuilder_append_char(struct netemu_stringbuilder *builder, char character);

#endif /* NETEMU_STRINGBUILDER_H_ */
