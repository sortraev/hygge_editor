#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>

#include "state.h"

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

void debugPrint(State *state) {
  NOTNULL_(state);

  printf("\x1b[999;1H"); // move cursor to bottommost line
  printf("\x1b[K");      // clear line

  if (isprint(state->lastKey))
    printf("Last key: '%c'", state->lastKey);
  else
    printf("Last key: %d", state->lastKey);
  printf(", cursor (y, x) = (%lu, %lu)", state->cursor.y, state->cursor.x);
  printf(", numLines = %lu", state->lines.numLines);
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
  // TODO: handle splitting of line here
  NOTNULL_(state);
  state->cursor.y++;
  state->cursor.x = 0;
  linesInsertEmpty(&state->lines, state->cursor.y);
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
  if (c == '\n')
    insertNewline(state);

  else if (isprint(c))
    insertChar(state, c);

  else switch (c) {

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

void drawEditorRows(State *state) {
  NOTNULL_(state);

  size_t lineOffset = 0; // TODO

  size_t editorHeight = state->windowDims.y - STATUS_BAR_HEIGHT;

  size_t i = 0;
  for (; i < MIN(editorHeight, state->lines.numLines); i++)
    printf("%s\n", state->lines.lineBufs[i].s);
  for (; i < editorHeight; i++)
    printf("~\n");
}

void refreshScreen(State *state) {
  NOTNULL_(state);

  printf("\x1b[2J"); // clear screen
  printf("\x1b[H");  // move cursor to top left

  drawEditorRows(state);

  debugPrint(state);

  printf("\x1b[%ld;%ldH", state->cursor.y + 1, state->cursor.x + 1); // move cursor to buffer position
}

#endif // EDITOR_H
