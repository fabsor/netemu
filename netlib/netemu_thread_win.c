#include "netemu_thread.h"

int netemu_thread_new(netemu_thread* identifier, unsigned long (NETEMU_API *start_fn) (void *), void *arg) {
	return CreateThread(NULL, 0, start_fn, arg, 0, identifier);
}

int netemu_thread_stop(netemu_thread* identifier) {
	unsigned long long i;
	return (int)TerminateThread(identifier, 0);
}