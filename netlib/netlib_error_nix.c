#include "headers/netlib_error.h"
#include <errno.h>

int last_error;

int netlib_get_last_error() {
	last_error = errno;
	return last_error;
}

void netlib_set_last_error(int error) {
	last_error = error;
}

void netlib_set_last_mapped_error(int error) {

}
