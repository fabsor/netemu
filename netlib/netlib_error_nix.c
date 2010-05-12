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
