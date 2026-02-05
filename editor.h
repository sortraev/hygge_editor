#ifndef UTIL_H
#define UTIL_H

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

void processKey(State *state, char c) {
  if (c == '\x1b' || c == CTRL_KEY('q')) {
    state->running = 0;
  }
  else if (isprint(c)) {
    sbAppendChar(state->sb, c);
  }
}

void refreshScreen(State *state) {
  printf("\x1b[2J"); // clear screen
  printf("\x1b[H");  // move cursor to top left
  printf("%s", state->sb->s); // print buffer
  printf("\x1b[%d;%dH", state->line + 1, state->col + 1); // move cursor to buffer position
}

#endif // UTIL_H
