#include "editor_state.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"
#include "dims.h"
#include "term.h"
#include "string_buffer.h"

int _resizeLines(EditorState *state, size_t newCap) {
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

int _insertLine(EditorState *state, size_t i, StringBuffer lineBuf) {
  NOTNULL_(state);

  if (i > state->numLines) {
    return 1;
  }

  // expand if necessary
  if (state->numLines + 1 > state->lineCap) {
    int resizeStatus = _resizeLines(state, state->numLines + 1);
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

  termGetWindowDims(&state->windowDims);

  state->lastKey = -1;

  return state;
}

int stateInsertEmptyLine(EditorState *state, size_t i) {
  NOTNULL_(state);

  StringBuffer sb = sbEmpty();
  if (sbInitWithCapacity(&sb, 16) != 0) {
    return 1;
  }
  return _insertLine(state, i, sb);
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

int stateSetMsg(EditorState *state, MsgType msgType, char *msg) {
  NOTNULL_(state);
  NOTNULL_(msg);

  strncpy(state->msgBuf, msg, 256);
  state->msgType = msgType;
  return 0;
}

int stateFormatMsg(EditorState *state, MsgType msgType, char *fmt, ...) {
  NOTNULL_(state);
  NOTNULL_(fmt);

  va_list args;
  va_start(args, fmt);
  vsnprintf(state->msgBuf, 256, fmt, args);
  va_end(args);

  state->msgType = msgType;

  return 0;
}

int stateClearMsg(EditorState *state) {
  NOTNULL_(state);
  return stateSetMsg(state, INFO, "");
}
