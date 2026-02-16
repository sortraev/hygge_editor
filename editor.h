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

void controlKeyDebugPrint(State *state) {
  if (state->lastControlKey >= 0) {
    printf("\x1b[999;1H"); // move cursor to bottommost line
    printf("Last control key: %d", state->lastControlKey);
  }
}

void processKey(State *state, char c) {
  if (c == '\x1b' || c == CTRL_KEY('q')) {
    state->running = 0;
  }
  else if (iscntrl(c)) {
    state->lastControlKey = c;
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

  controlKeyDebugPrint(state);

  printf("\x1b[%d;%dH", state->cursorY + 1, state->cursorX + 1); // move cursor to buffer position
}

#endif // EDITOR_H
