#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>

#include "editor_state.h"

#define STATUS_BAR_HEIGHT 1

#define CTRL_KEY(key) ((key) & 0x1f)

char readKeyBlocking(void) {
  char c;
  int num_read;
  while ((num_read = fread(&c, sizeof(char), 1, stdin)) != 1) {
    // TODO: handle num_read < 0 and errno != EAGAIN
  }

  // TODO: handle escape sequences (requires adding a timeout on stdin reads)

  return c;
}

void processCursorMovementKey(State *state, int c) {
  NOTNULL_(state);

  // TODO: revisit this logic once zero-line files are supported.
  switch (c) {
    case CTRL_KEY('w'): // up
      if (state->cursor.y > 0)
        state->cursor.y--;
      break;

    case CTRL_KEY('s'): // down
      if (state->cursor.y + 1 < state->lines.numLines)
        state->cursor.y++;
      break;

    case CTRL_KEY('a'): // left
      if (state->cursor.x > 0)
        state->cursor.x--;
      break;

    case CTRL_KEY('d'): // right
      state->cursor.x++;
      break;
  }
  size_t currentLineLen =
    state->cursor.y < state->lines.numLines
      ? state->lines.lineBufs[state->cursor.y].len
      : 0;
  state->cursor.x = MIN(state->cursor.x, currentLineLen);
}

void insertNewline(State *state) {
  NOTNULL_(state);

  // insert a newline by first inserting an empty line, then splitting
  // the current line, placing the tail end onto the new line.
  linesInsertEmpty(&state->lines, state->cursor.y + 1);
  sbSplit(state->lines.lineBufs + state->cursor.y,
          state->lines.lineBufs + state->cursor.y + 1,
          state->cursor.x);
  state->cursor.y++;
  state->cursor.x = 0;
}

void insertChar(State *state, char c) {
  NOTNULL_(state);
  linesInsertChar(&state->lines, state->cursor.y, state->cursor.x, c);
  state->cursor.x++;
}

void deleteChar(State *state) {
  NOTNULL_(state);
  linesDeleteChar(&state->lines, state->cursor.y, state->cursor.x);
}

void processKey(State *state, char c) {
  NOTNULL_(state);

  state->lastKey = c;

  if (isprint(c))
    insertChar(state, c);

  else switch (c) {

    case '\n':
      insertNewline(state);
      break;

    case '\x1b':
    case CTRL_KEY('q'):
      state->running = 0;
      break;

    case CTRL_KEY('x'):
      deleteChar(state);
      break;

    case CTRL_KEY('w'):
    case CTRL_KEY('a'):
    case CTRL_KEY('s'):
    case CTRL_KEY('d'):
      processCursorMovementKey(state, c);
      break;
  }
}

void renderEditorWindow(State *state, StringBuffer *out) {
  NOTNULL_(state);
  NOTNULL_(out);

  size_t editorHeight = state->windowDims.y - STATUS_BAR_HEIGHT;

  size_t i = 0;
  for (; i < MIN(editorHeight, state->lines.numLines); i++) {
    sbAppendString(out, state->lines.lineBufs[i].s);
    sbAppendChar(out, '\n');
  }
  for (; i < editorHeight; i++)
    sbAppendString(out, "~\n");
}

void renderStatusBar(State *state, StringBuffer *out) {
  NOTNULL_(state);
  NOTNULL_(out);

  // move cursor to bottom line and clear line
  sbAppendString(out, "\x1b[999;1H\x1b[K");

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
  sbAppendString(out, statusBarBuf);

  char cursorMovementBuf[12];
  snprintf(
      cursorMovementBuf,
      sizeof(cursorMovementBuf),
      "\x1b[%ld;%ldH",
      state->cursor.y + 1,
      state->cursor.x + 1);
  sbAppendString(out, cursorMovementBuf);
}

void refreshScreen(State *state) {
  NOTNULL_(state);

  StringBuffer sb = sbEmpty();
  sbInitWithCapacity(&sb, 4096);

  sbAppendString(&sb, "\x1b[2J\x1b[H"); // move cursor to top position

  renderEditorWindow(state, &sb);

  renderStatusBar(state, &sb);

  fwrite(sb.s, sizeof(char), sb.len, stdout);
  sbFree(&sb);
}

#endif // EDITOR_H
