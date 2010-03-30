/*
 * netemu_receiver.c
 *
 *  Created on: 30 mar 2010
 *      Author: fabian
 */

/**
 * Create a new receiver. This will create a new socket and bind to the provided host and
 * port.
 */
void netemu_receiver_new(char* host, int port) {

}

/*! Called to receive data. */
void netemu_receiver_recv() {

}

/**
 * Register a function that will act as a listener. The function will be called when data is received.
 * The function must be thread safe.
 */
void netemu_receiver_register_recv_fn(void (* listenerFn)(char*, size_t, struct netemu_sender)) {

}
