/*
 * state.c
 *
 *  Created on: 12 apr 2010
 *      Author: fabbe
 */
#include "state.h"
#include <stdlib.h>
struct netemu_state* state_init() {
	struct netemu_state* state;
	state = malloc(sizeof(struct netemu_state*));
	return state;
}

void state_set_state(struct netemu_state *state,void (*stateStartFn)(struct netemu_state*)) {
	if(state->stateDestroyFn != NULL)
		state->stateDestroyFn(state);
	state->prevStateStartFn = state->stateStartFn;
	state->stateStartFn = stateStartFn;
	stateStartFn(state);
}

void state_set_prev_state(struct netemu_state *state) {
	state->stateDestroyFn(state);
	state->prevStateStartFn(state);
	state->prevStateStartFn = state->stateStartFn;
}

void state_destroy(struct netemu_state *state) {
	state->stateDestroyFn(state);
	free(state);
}
