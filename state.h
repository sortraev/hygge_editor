#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "init.h"
#include "string_buffer.h"

typedef struct {
  int running;

  FILE *outFile;

  StringBuffer *sb;

  Dims cursor;

  Dims windowDims;

  int lastKey;

} State;

State *stateInit() {
  State *state = calloc(1, sizeof(State));

  if (!state) {
    return NULL;
  }

  state->sb = sbWithCapacity(16);
  state->lastKey = -1;

  get_window_dims(&state->windowDims);

  return state;
}

void stateFree(State *state) {
  if (state) {
    sbFree(state->sb);
    free(state->sb);
  }
}

#endif // STATE_H
