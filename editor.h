#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>

#include "state.h"

#define KEY_ESCAPE 27

#define CTRL_KEY(key) ((key) & 0x1f)

char readKeyBlocking() {
  char c;
  int num_read;
  while ((num_read = fread(&c, sizeof(char), 1, stdin)) != 1) {
    // TODO: handle num_read < 0
  }

  return c;
}


void debugPrint(State *state) {
  printf("\x1b[999;1H"); // move cursor to bottommost line
  printf("\x1b[K");      // clear line

  if (isprint(state->lastKey))
    printf("Last key: '%c'", state->lastKey);
  else
    printf("Last key: %d", state->lastKey);
  printf(", cursor (y, x) = (%d, %d)", state->cursorY, state->cursorX);
}

void processCursorMovementKey(State *state, int c) {
  switch (c) {
    case CTRL_KEY('k'): // up
      if (state->cursorY > 0)
        state->cursorY--;
      break;
    case CTRL_KEY('j'): // down
      if (state->cursorY < state->windowHeight - 1)
        state->cursorY++;
      break;
    case CTRL_KEY('h'): // left
      if (state->cursorX > 0)
        state->cursorX--;
      break;
    case CTRL_KEY('l'): // right
      if (state->cursorX < state->windowWidth - 1)
        state->cursorX++;
      break;
  }
}

void processKey(State *state, char c) {
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
  for (int i = 0; i < state->windowHeight - 1; i++)
    printf("~\n");
  if (state->windowHeight > 0)
    printf("~");
}

void refreshScreen(State *state) {
  printf("\x1b[2J"); // clear screen
  printf("\x1b[H");  // move cursor to top left

  drawEditorRows(state);

  debugPrint(state);

  printf("\x1b[%d;%dH", state->cursorY + 1, state->cursorX + 1); // move cursor to buffer position
}

#endif // EDITOR_H
