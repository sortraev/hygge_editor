#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "dims.h"
#include "terminal.h"
#include "lines.h"

typedef struct {
  int running;

  FILE *f;
  char *filename;

  Lines lines;

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

  state->lines = linesEmpty();

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
