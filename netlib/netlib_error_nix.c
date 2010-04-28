#include "headers/netlib_error.h"

int last_error;

int netlib_get_last_error() {
	return last_error;
}

void netlib_set_last_error(int error) {
	last_error = error;
}

void netlib_set_last_mapped_error(int error) {

}
