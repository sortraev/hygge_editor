#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "terminal.h"
#include "string_buffer.h"

typedef struct {
  int running;

  FILE *f;

  StringBuffer sb;

  Dims cursor;

  Dims windowDims;

  int lastKey;

} State;

State *stateInit(void) {
  State *state = calloc(1, sizeof(State));

  if (!state) {
    return NULL;
  }

  if (sbInitWithCapacity(&state->sb, 16) != 0) {
    fprintf(stderr, "Failed to init state string buffer\n");
    free(state);
    return NULL;
  }

  state->lastKey = -1;

  getWindowDims(&state->windowDims);

  return state;
}

void stateFree(State *state) {
  if (state) {
    sbFree(&state->sb);
  }
}

#endif // STATE_H
