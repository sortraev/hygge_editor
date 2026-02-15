#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "string_buffer.h"

typedef struct {
  int running;

  FILE *outFile;

  StringBuffer *sb;

  int cursorY;
  int cursorX;

  int windowHeight;
  int windowWidth;

} State;

State *stateInit() {
  State *state = malloc(sizeof(State));

  if (!state) {
    return NULL;
  }

  state->sb = sbWithCapacity(16);

  return state;
}

void stateFree(State *state) {
  if (state) {
    sbFree(state->sb);
    free(state->sb);
  }
}

#endif // STATE_H
