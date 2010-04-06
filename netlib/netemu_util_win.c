#include <stdio.h>
#include <stdlib.h>
#include "netemu_util.h"

void netemu_snprintf(char* dest,size_t size, char* format, void* data) {
	_snprintf(dest,size,format,data);
}
