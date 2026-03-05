#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "terminal.h"
#include "string_buffer.h"

typedef struct {
  int running;

  FILE *f;
  char *filename;

  Dims cursor;

  Dims windowDims;

  int lastKey;

} State;

State *stateInit(void) {
  State *state = calloc(1, sizeof(State));

  if (!state) {
    return NULL;
  }

  state->lastKey = -1;

  getWindowDims(&state->windowDims);

  return state;
}

void stateFree(State *state) {
  if (state) {
    // for now, do nothing.
    // TODO: close state->f?
  }
}

#endif // STATE_H
