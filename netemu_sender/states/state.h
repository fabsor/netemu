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
	struct netemu_state* prevState;
	struct netemu_state* nextState;
	void* state;
	void (*stateStartFn)(struct netemu_state* state);
	void (*stateDestroyFn)(struct netemu_state* state);
	void (*stateUpdateFn)(struct netemu_state* state);
};



void state_set_state(struct netemu_state *state,void (*stateStartFn)(struct state* state));

void state_set_prev_state();

#endif /* STATE_H_ */
