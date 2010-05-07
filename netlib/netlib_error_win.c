#include "netlib_error.h"
#include "windows.h"

int last_error;

int netlib_get_last_error() {
	return last_error;
}

void netlib_set_last_error(int error) {
	last_error = error;
}

void netlib_set_last_mapped_error(int error) {
	switch(error) {
		case ERROR_PATH_NOT_FOUND:
			last_error = NETEMU_EPATHNOTFOUND;
			break;
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_OUTOFMEMORY:
			last_error = NETEMU_ENOTENOUGHMEMORY;
			break;
		case ERROR_INVALID_HANDLE:
			last_error = NETEMU_EINVALIDHANDLE;
			break;
	}
}

int netlib_get_last_platform_error() {
	return GetLastError();
}
