/*
 *   This file is part of netemu.
 *
 *   Netemu is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Netemu is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with netemu.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef NETEMU_STRINGBUILDER_H_
#define NETEMU_STRINGBUILDER_H_

#include "../util.h"

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
