#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "string_buffer.h"

typedef enum mode {
  NORMAL,
  INSERT
} EMode;

typedef struct {
  int running;
  EMode mode;

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
  state->mode = INSERT;

  return state;
}

void stateFree(State *state) {
  if (state) {
    sbFree(state->sb);
    free(state->sb);
  }
}

#endif // STATE_H
