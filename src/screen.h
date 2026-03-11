#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>

#include "util.h"
#include "editor_state.h"
#include "string_buffer.h"

#define STATUS_BAR_HEIGHT 1

void _renderEditorWindow(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);

  size_t editorHeight = state->windowDims.y - STATUS_BAR_HEIGHT;

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

  // move cursor to bottom line and clear line
  sbAppendString(screenBuf, "\x1b[999;1H\x1b[K");

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

  char cursorMovementBuf[12];
  snprintf(
      cursorMovementBuf,
      sizeof(cursorMovementBuf),
      "\x1b[%ld;%ldH",
      state->cursor.y + 1,
      state->cursor.x + 1);
  sbAppendString(screenBuf, cursorMovementBuf);
}

void screenDrawEditorState(EditorState *state) {
  NOTNULL_(state);

  StringBuffer screenBuf = sbEmpty();
  sbInitWithCapacity(&screenBuf, 4096);

  sbAppendString(&screenBuf, "\x1b[2J\x1b[H"); // move cursor to top position

  _renderEditorWindow(state, &screenBuf);
  _renderStatusBar(state, &screenBuf);

  fwrite(screenBuf.s, sizeof(char), screenBuf.len, stdout);
  sbFree(&screenBuf);
}

#undef STATUS_BAR_HEIGHT

#endif // SCREEN_H
