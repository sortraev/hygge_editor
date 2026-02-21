#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>

#include "state.h"

#define KEY_ESCAPE 27

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
  printf(", cursor (y, x) = (%d, %d)", state->cursor.y, state->cursor.x);
}

void processCursorMovementKey(State *state, int c) {
  NOTNULL_(state);

  switch (c) {
    case CTRL_KEY('k'): // up
      if (state->cursor.y > 0)
        state->cursor.y--;
      break;
    case CTRL_KEY('j'): // down
      if (state->cursor.y < state->windowDims.y - 1)
        state->cursor.y++;
      break;
    case CTRL_KEY('h'): // left
      if (state->cursor.x > 0)
        state->cursor.x--;
      break;
    case CTRL_KEY('l'): // right
      if (state->cursor.x < state->windowDims.x - 1)
        state->cursor.x++;
      break;
  }
}

void processKey(State *state, char c) {
  NOTNULL_(state);

  state->lastKey = c;
  switch (c) {
    case '\x1b':
    case CTRL_KEY('q'):
      state->running = 0;
      break;
    case CTRL_KEY('h'):
    case CTRL_KEY('j'):
    case CTRL_KEY('k'):
    case CTRL_KEY('l'):
      processCursorMovementKey(state, c);
      break;
  }
}

void drawEditorRows(State *state) {
  NOTNULL_(state);

  for (int i = 0; i < state->windowDims.y - 1; i++)
    printf("~\n");
  if (state->windowDims.y > 0)
    printf("~");
}

void refreshScreen(State *state) {
  NOTNULL_(state);

  printf("\x1b[2J"); // clear screen
  printf("\x1b[H");  // move cursor to top left

  drawEditorRows(state);

  debugPrint(state);

  printf("\x1b[%d;%dH", state->cursor.y + 1, state->cursor.x + 1); // move cursor to buffer position
}

#endif // EDITOR_H
