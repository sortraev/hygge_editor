#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>

#include "editor_state.h"

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

void processCursorMovementKey(EditorState *state, int c) {
  NOTNULL_(state);

  // TODO: revisit this logic once zero-line files are supported.
  switch (c) {
    case CTRL_KEY('w'): // up
      if (state->cursor.y > 0)
        state->cursor.y--;
      break;

    case CTRL_KEY('s'): // down
      if (state->cursor.y + 1 < state->numLines)
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
    state->cursor.y < state->numLines
      ? state->lines[state->cursor.y].len
      : 0;
  state->cursor.x = MIN(state->cursor.x, currentLineLen);
}

void editorInsertNewline(EditorState *state) {
  NOTNULL_(state);

  // insert a newline by first inserting an empty line, then splitting
  // the current line, placing the tail end onto the new line.
  stateInsertEmptyLine(state, state->cursor.y + 1);
  sbSplit(state->lines + state->cursor.y,
          state->lines + state->cursor.y + 1,
          state->cursor.x);
  state->cursor.y++;
  state->cursor.x = 0;
}

void editorInsertChar(EditorState *state, char c) {
  NOTNULL_(state);
  stateInsertChar(state, state->cursor.y, state->cursor.x, c);
  state->cursor.x++;
}

void editorDeleteChar(EditorState *state) {
  NOTNULL_(state);
  stateDeleteChar(state, state->cursor.y, state->cursor.x);
}

void processKey(EditorState *state, char c) {
  NOTNULL_(state);

  state->lastKey = c;

  if (isprint(c))
    editorInsertChar(state, c);

  else switch (c) {

    case '\n':
      editorInsertNewline(state);
      break;

    case '\x1b':
    case CTRL_KEY('q'):
      state->running = 0;
      break;

    case CTRL_KEY('x'):
      editorDeleteChar(state);
      break;

    case CTRL_KEY('w'):
    case CTRL_KEY('a'):
    case CTRL_KEY('s'):
    case CTRL_KEY('d'):
      processCursorMovementKey(state, c);
      break;
  }
}

#endif // EDITOR_H
