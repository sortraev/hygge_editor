#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>

#include "util.h"
#include "editor_state.h"
#include "string_buffer.h"

void _moveCursorTo(size_t y, size_t x, StringBuffer *screenBuf) {
  char cursorMovementBuf[12];
  snprintf(
      cursorMovementBuf,
      sizeof(cursorMovementBuf),
      "\x1b[%ld;%ldH",
      y + 1,
      x + 1);
  sbAppendString(screenBuf, cursorMovementBuf);
}

void _renderEditorWindow(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  size_t editorHeight = state->windowDims.y;

  size_t i = 0;
  for (; i < MIN(editorHeight, state->numLines); i++) {
    sbAppendString(screenBuf, state->lines[i].s);
    sbAppendChar(screenBuf, '\n');
  }
  for (; i < editorHeight; i++)
    sbAppendString(screenBuf, "~\n");
}

void _renderStatusBar(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  // clear status bar line
  sbAppendString(screenBuf, "\x1b[K");

  char statusBarBuf[256];
  int n = snprintf(
      statusBarBuf,
      sizeof(statusBarBuf),
      "%s | %lu:%lu | last key: ",
      state->filename ? state->filename : "[No filename]",
      state->cursor.y + 1,
      state->cursor.x + 1);
  snprintf(
      statusBarBuf + n,
      sizeof(statusBarBuf) - n,
      isprint(state->lastKey) ? "'%c'" : "%d",
      state->lastKey);
  sbAppendString(screenBuf, statusBarBuf);
}

void screenDrawEditorState(EditorState *state) {
  NOTNULL_(state);

  StringBuffer screenBuf = sbEmpty();
  sbInitWithCapacity(&screenBuf, 4096);

  sbAppendString(&screenBuf, "\x1b[2J\x1b[H"); // move cursor to top position

  _renderEditorWindow(state, &screenBuf);

  _moveCursorTo(state->windowDims.y, 0, &screenBuf);
  _renderStatusBar(state, &screenBuf);

  _moveCursorTo(state->cursor.y, state->cursor.x, &screenBuf);

  fwrite(screenBuf.s, sizeof(char), screenBuf.len, stdout);
  sbFree(&screenBuf);
}

#undef STATUS_BAR_HEIGHT

#endif // SCREEN_H
