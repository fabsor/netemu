/*
 * state.h
 *
 *  Created on: 8 apr 2010
 *      Author: fabbe
 */

/**
 * @file
 * this is just a mock of an idea I had that we could create a state machine.
 */

#ifndef STATE_H_
#define STATE_H_

struct netemu_state {
	void *state;
	void (*stateStartFn)(struct netemu_state* state);
	void (*prevStateStartFn)(struct netemu_state* state);
	void (*stateDestroyFn)(struct netemu_state* state);
	void (*stateUpdateFn)(struct netemu_state* state);
};


struct netemu_state* state_init();

void state_set_state(struct netemu_state *state,void (*stateStartFn)(struct netemu_state*));

void state_set_prev_state(struct netemu_state *state);

void state_destroy(struct netemu_state *state);

#endif /* STATE_H_ */
