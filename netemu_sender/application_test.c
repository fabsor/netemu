/*
 * application_test.c
 *
 *  Created on: 6 apr 2010
 *      Author: fabian
 */
#include "protocol/application.h"
#include "protocol/transport.h"

/**
 *
 */
void init_application_test() {

}
/**
 * Test sending a login request.
 */
void test_login_request() {
	struct login_request* request;
	char* buffer;
	struct protocol_message *messages[1];
	int hej;
	request = netemu_application_create_login_request("netemu",1);
	messages[0] = netemu_application_create_message(hej,"haha",(void*)request);
	buffer = netemu_transport_pack(messages,1);

}
