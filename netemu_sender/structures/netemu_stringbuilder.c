#include "netemu_stringbuilder.h"

#include <string.h>
#include <stdio.h>

#define DEFAULT_CAPACITY	16

int _netemu_enlarge_stringbuilder(struct netemu_stringbuilder *builder);

struct netemu_stringbuilder *netemu_stringbuilder_new(NETEMU_DWORD capacity) {
	struct netemu_stringbuilder *builder;

	if(capacity < 0)
		return NULL;
	else if(capacity == 0)
		capacity = DEFAULT_CAPACITY;

	if((builder = (struct netemu_stringbuilder*)malloc(sizeof(struct netemu_stringbuilder))) == NULL)
		return NULL;

	builder->capacity = capacity;
	builder->count = 0;

	if((builder->string = (char*)malloc(sizeof(char) * capacity)) == NULL) {
		free(builder);
		return NULL;
	}

	memset(builder->string, 0, builder->capacity);

	return builder;
}

int netemu_stringbuilder_append_chars(struct netemu_stringbuilder *builder, char* string, NETEMU_DWORD len) {
	NETEMU_DWORD new_size;
	int error;

	error = 0;
	new_size = builder->count + len + 1;
	while(new_size > builder->capacity) {
		if((error = _netemu_enlarge_stringbuilder(builder)) == -1)
			break;
	}

	if(error == 0) {
		memcpy(&builder->string[builder->count], string, len);
		builder->count += len;
		builder->string[builder->count] = '\0';
	}
	
	return error;
}

int netemu_stringbuilder_append_char(struct netemu_stringbuilder *builder, char character) {
	int error;

	error = 0;
	if(builder->count + sizeof(char) + 1 > builder->capacity) {
		error = _netemu_enlarge_stringbuilder(builder);
	}

	if(error == 0) {
		builder->string[builder->count] = character;
		builder->count++;
		builder->string[builder->count] = '\0';
	}

	return error;
}

int _netemu_enlarge_stringbuilder(struct netemu_stringbuilder *builder) {
	builder->capacity *= 2;
	return (realloc(builder->string, builder->capacity) == NULL ? -1 : 0);
}