#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>

#include "util.h"
#include "editor_state.h"
#include "string_buffer.h"

void _screenMoveCursorTo(size_t y, size_t x, StringBuffer *screenBuf) {
  char cursorMovementBuf[12];
  snprintf(
      cursorMovementBuf,
      sizeof(cursorMovementBuf),
      "\x1b[%ld;%ldH",
      y + 1,
      x + 1);
  sbAppendString(screenBuf, cursorMovementBuf);
}

void _screenClearScreen(StringBuffer *screenBuf) {
  sbAppendString(screenBuf, "\x1b[2J");
}

void _screenClearLine(StringBuffer *screenBuf) {
  sbAppendString(screenBuf, "\x1b[K");
}

void _screenRenderEditorWindow(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  size_t editorHeight = state->windowDims.y;

  size_t i = 0;
  size_t bound = MIN(editorHeight, state->numLines - state->windowOffset);
  for (; i < bound; i++) {
    sbAppendString(screenBuf, state->lines[state->windowOffset + i].s);
    sbAppendChar(screenBuf, '\n');
  }
  for (; i < editorHeight; i++)
    sbAppendString(screenBuf, "~\n");
}

void _screenRenderStatusBar(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  _screenClearLine(screenBuf);

  char statusBarBuf[256];
  int n = snprintf(
      statusBarBuf,
      sizeof(statusBarBuf),
      "%s | %lu:%lu | last key: ",
      state->filename ? state->filename : "[No filename]",
      state->cursor.y + 1,
      state->cursor.x + 1);
  n += snprintf(
      statusBarBuf + n,
      sizeof(statusBarBuf) - n,
      isprint(state->lastKey) ? "'%c'" : "%d",
      state->lastKey);

  snprintf(
      statusBarBuf + n,
      sizeof(statusBarBuf) - n,
      " | windowOffset = %ld",
      state->windowOffset);
  sbAppendString(screenBuf, statusBarBuf);
}

void _screenRenderMsgBar(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  _screenClearLine(screenBuf);

  // TODO: this assumes windowDims.x is always smaller than the msg buffer.
  // should probably handle this dynamically, somehow ..
  state->msgBuf[state->windowDims.x] = '\0';

  sbAppendString(screenBuf, state->msgBuf);
  state->msgBufDirty = 0;
}

void screenDrawEditorState(EditorState *state) {
  NOTNULL_(state);

  StringBuffer screenBuf = sbEmpty();
  sbInitWithCapacity(&screenBuf, 4096);

  _screenClearScreen(&screenBuf);

  _screenMoveCursorTo(0, 0, &screenBuf);
  _screenRenderEditorWindow(state, &screenBuf);

  int statusBarY = state->windowDims.y;

  _screenMoveCursorTo(statusBarY, 0, &screenBuf);
  _screenRenderStatusBar(state, &screenBuf);

  if (state->msgBufDirty) {
    int msgBarY = statusBarY + 1;
    _screenMoveCursorTo(msgBarY, 0, &screenBuf);
    _screenRenderMsgBar(state, &screenBuf);
  }

  // reset term cursor to editor cursor position
  _screenMoveCursorTo(state->cursor.y - state->windowOffset, state->cursor.x, &screenBuf);

  fwrite(screenBuf.s, sizeof(char), screenBuf.len, stdout);
  sbFree(&screenBuf);
}

#undef STATUS_BAR_HEIGHT

#endif // SCREEN_H
