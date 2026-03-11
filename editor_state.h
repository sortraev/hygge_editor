#ifndef STATE_H
#define STATE_H

#include <stdlib.h>

#include "util.h"
#include "dims.h"
#include "terminal.h"
// #include "lines.h"
#include "string_buffer.h"

typedef struct {
  int running;

  char *filename;

  StringBuffer *lines;
  size_t numLines;
  size_t cap;

  Dims cursor;

  Dims windowDims;

  int lastKey;

} State;

int linesResize(State *state, size_t newCap) {
  NOTNULL_(state);
  ASSERT(newCap >= state->cap, "linesResize(): downsizing not yet supported");

  StringBuffer *tmp = realloc(state->lines, newCap * sizeof(StringBuffer));
  if (!tmp) {
    fprintf(stderr, "linesResize(): Failed to reallocate lineBufs\n");
    return 1;
  }

  state->lines = tmp;
  state->cap = newCap;

  if (state->numLines >= newCap)
    state->numLines = newCap;

  memset(state->lines + state->numLines,
         0,
         (state->cap - state->numLines) * sizeof(StringBuffer));

  return 0;
}

int _linesInsertStringBuffer(State *state, size_t i, StringBuffer lineBuf) {
  NOTNULL_(state);

  if (i > state->numLines) {
    return 1;
  }

  // expand if necessary
  if (state->numLines + 1 > state->cap) {
    int resizeStatus = linesResize(state, state->numLines + 1);
    if (resizeStatus != 0)
      return resizeStatus;
  }

  // move all lines after insertion point
  for (size_t j = state->numLines; j > i; j--) {
    state->lines[j] = state->lines[j - 1];
  }

  // insert the new line
  state->lines[i] = lineBuf;
  state->numLines++;

  return 0;
}

int linesInsertEmpty(State *state, size_t i) {
  NOTNULL_(state);

  StringBuffer sb = sbEmpty();
  if (sbInitWithCapacity(&sb, 16) != 0) {
    return 1;
  }
  return _linesInsertStringBuffer(state, i, sb);
}

int linesAppendEmpty(State *state) {
  NOTNULL_(state);
  return linesInsertEmpty(state, state->numLines);
}

int linesInsertString(State *state, size_t line, size_t col, char *s) {
  NOTNULL_(state);
  NOTNULL_(s);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;
  return sbInsertString(lineBuf, col, s);
}

int linesInsertChar(State *state, size_t line, size_t col, char c) {
  NOTNULL_(state);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;
  return sbInsertChar(lineBuf, col, c);
}

int linesDeleteChar(State *state, size_t line, size_t col) {
  NOTNULL_(state);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;
  return sbDeleteChar(lineBuf, col);
}

void stateFree(State *state) {
  if (state) {
    for (size_t i = 0; i < state->numLines; i++) {
      sbFree(state->lines + i);
    }
    free(state->lines);
  }
}



State *stateInit(void) {
  State *state = calloc(1, sizeof(State));

  if (!state) {
    return NULL;
  }

  state->lastKey = -1;

  // state->lines = linesEmpty();

  // TODO: placeholder. should insert better handling of empty files.
  linesAppendEmpty(state);

  getWindowDims(&state->windowDims);

  return state;
}

#endif // STATE_H
