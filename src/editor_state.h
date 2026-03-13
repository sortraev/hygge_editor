#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include <stdlib.h>

#include "util.h"
#include "dims.h"
#include "terminal.h"
#include "string_buffer.h"

typedef struct {
  int running;


  StringBuffer *lines;
  size_t numLines;
  size_t lineCap;

  Dims cursor;
  Dims windowDims;

  char *filename;
  int dirty;

  int lastKey;

} EditorState;

int _stateResizeLines(EditorState *state, size_t newCap) {
  NOTNULL_(state);
  ASSERT(newCap >= state->lineCap, "stateResizeLines(): downsizing not yet supported");

  state->lines = reallocOrDie(state->lines, newCap * sizeof(StringBuffer));
  state->lineCap = newCap;

  if (state->numLines >= newCap)
    state->numLines = newCap;

  memset(state->lines + state->numLines,
         0,
         (state->lineCap - state->numLines) * sizeof(StringBuffer));

  return 0;
}

int _stateInsertLine(EditorState *state, size_t i, StringBuffer lineBuf) {
  NOTNULL_(state);

  if (i > state->numLines) {
    return 1;
  }

  // expand if necessary
  if (state->numLines + 1 > state->lineCap) {
    int resizeStatus = _stateResizeLines(state, state->numLines + 1);
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

  state->dirty = 1;

  return 0;
}

int stateInsertEmptyLine(EditorState *state, size_t i) {
  NOTNULL_(state);

  StringBuffer sb = sbEmpty();
  if (sbInitWithCapacity(&sb, 16) != 0) {
    return 1;
  }
  return _stateInsertLine(state, i, sb);
}

int stateAppendEmptyLine(EditorState *state) {
  NOTNULL_(state);
  return stateInsertEmptyLine(state, state->numLines);
}

int stateInsertString(EditorState *state, size_t line, size_t col, char *s) {
  NOTNULL_(state);
  NOTNULL_(s);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;

  int insertStatus = sbInsertString(lineBuf, col, s);
  state->dirty |= insertStatus == 0;
  return insertStatus;
}

int stateInsertChar(EditorState *state, size_t line, size_t col, char c) {
  NOTNULL_(state);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;

  int insertStatus = sbInsertChar(lineBuf, col, c);
  state->dirty |= insertStatus == 0;
  return insertStatus;
}

int stateDeleteChar(EditorState *state, size_t line, size_t col) {
  NOTNULL_(state);

  if (line >= state->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = state->lines + line;

  int deleteStatus = sbDeleteChar(lineBuf, col);
  state->dirty |= deleteStatus == 0;
  return deleteStatus;
}

void stateFree(EditorState *state) {
  if (state) {
    for (size_t i = 0; i < state->numLines; i++) {
      sbFree(state->lines + i);
    }
    free(state->lines);
  }
}

EditorState *stateInit(void) {
  EditorState *state = callocOrDie(1, sizeof(EditorState));

  getWindowDims(&state->windowDims);

  state->lastKey = -1;

  return state;
}

#endif // EDITOR_STATE_H
