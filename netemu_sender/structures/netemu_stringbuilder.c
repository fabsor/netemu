#include "netemu_stringbuilder.h"
#include "netlib_error.h"

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

	if((builder = (struct netemu_stringbuilder*)malloc(sizeof(struct netemu_stringbuilder))) == NULL) {
		netlib_set_last_error(NETEMU_ENOTENOUGHMEMORY);
		return NULL;
	}

	builder->capacity = capacity;
	builder->count = 0;

	if((builder->string = (char*)malloc(sizeof(char) * capacity)) == NULL) {
		free(builder);
		return NULL;
	}

	memset(builder->string, 0, builder->capacity);

	return builder;
}

void netemu_stringbuilder_free(struct netemu_stringbuilder *builder) {
	free(builder->string);
	free(builder);
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
	char *buffer;
	
	/* realloc would in theory be a better option here, but for some reason
	 * it kept corrupting our data when the capacity reached a high number ( >4096).*/
	builder->capacity *= 2;
	if((buffer = (char*)malloc(sizeof(char) * builder->capacity)) == NULL)
		return -1;
	memcpy(buffer, builder->string, builder->count + 1);
	free(builder->string);
	builder->string = buffer;
	return 0;
}
