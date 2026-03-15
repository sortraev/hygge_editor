#include "screen.h"

#include <stdio.h>
#include <ctype.h>

#include "util.h"
#include "editor_state.h"
#include "string_buffer.h"

#define SCREEN_BUF_INIT_SIZE 4096
#define STATUS_BAR_BUF_SIZE 256

#define COLOR_RESET "\x1b[0m"

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

  char statusBarBuf[STATUS_BAR_BUF_SIZE];
  int n = snprintf(
      statusBarBuf,
      STATUS_BAR_BUF_SIZE,
      "%s | %lu:%lu | last key: ",
      state->filename ? state->filename : "[No filename]",
      state->cursor.y + 1,
      state->cursor.x + 1);
  n += snprintf(
      statusBarBuf + n,
      STATUS_BAR_BUF_SIZE - n,
      isprint(state->lastKey) ? "'%c'" : "%d",
      state->lastKey);

  snprintf(
      statusBarBuf + n,
      STATUS_BAR_BUF_SIZE - n,
      " | windowOffset = %ld",
      state->windowOffset);
  sbAppendString(screenBuf, statusBarBuf);
}

void _screenSetColor(StringBuffer *screenBuf, char *color) {
  sbAppendString(screenBuf, color);
}

void _screenRenderMsgBar(EditorState *state, StringBuffer *screenBuf) {
  NOTNULL_(state);
  NOTNULL_(screenBuf);
  ASSERT(state->lastMsgLevel != NONE, "Error: unexpected call to _screenRenderMsgBar");

  _screenClearLine(screenBuf);

  // TODO: this assumes windowDims.x is always smaller than the msg buffer.
  // should probably handle this dynamically, somehow ..
  state->msgBuf[state->windowDims.x] = '\0';

  switch (state->lastMsgLevel) {
    case WARN:
      _screenSetColor(screenBuf, COLOR_WARN);
      break;
    case ERROR:
      _screenSetColor(screenBuf, COLOR_ERROR);
      break;
    default:
      break;
  }

  sbAppendString(screenBuf, state->msgBuf);

  _screenSetColor(screenBuf, COLOR_RESET);

  state->lastMsgLevel = NONE;
}

void screenDrawEditorState(EditorState *state) {

  NOTNULL_(state);

  StringBuffer screenBuf = sbEmpty();
  sbInitWithCapacity(&screenBuf, SCREEN_BUF_INIT_SIZE);

  _screenClearScreen(&screenBuf);

  _screenMoveCursorTo(0, 0, &screenBuf);
  _screenRenderEditorWindow(state, &screenBuf);

  int statusBarY = state->windowDims.y;

  _screenMoveCursorTo(statusBarY, 0, &screenBuf);
  _screenRenderStatusBar(state, &screenBuf);

  if (state->lastMsgLevel != NONE) {
    int msgBarY = statusBarY + 1;
    _screenMoveCursorTo(msgBarY, 0, &screenBuf);
    _screenRenderMsgBar(state, &screenBuf);
  }

  // reset term cursor to editor cursor position
  _screenMoveCursorTo(state->cursor.y - state->windowOffset, state->cursor.x, &screenBuf);

  fwrite(screenBuf.s, sizeof(char), screenBuf.len, stdout);
  sbFree(&screenBuf);
}
