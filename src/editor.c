#include "editor_state.h"

#include <stdio.h>
#include <ctype.h> // isprint

#include "io.h"
#include "util.h"

#define CTRL_KEY(key) ((key) & 0x1f)

void _editorProcessCursorMovementKey(EditorState *state, int c) {
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
  // fix cursor X
  size_t currentLineLen =
    state->cursor.y < state->numLines
      ? state->lines[state->cursor.y].len
      : 0;
  state->cursor.x = MIN(state->cursor.x, currentLineLen);

  // fix windowOffset
  if (state->cursor.y > state->windowOffset + state->windowDims.y - 1)
    state->windowOffset++;
  if (state->cursor.y < state->windowOffset)
    state->windowOffset--;
}

void _editorInsertNewline(EditorState *state) {
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

void _editorInsertChar(EditorState *state, char c) {
  NOTNULL_(state);
  stateInsertChar(state, state->cursor.y, state->cursor.x, c);
  state->cursor.x++;
}

void _editorDeleteChar(EditorState *state) {
  NOTNULL_(state);
  stateDeleteChar(state, state->cursor.y, state->cursor.x);
}

void _editorDoSaveToFile(EditorState *state) {
  NOTNULL_(state);

  char *filename = state->filename;
  if (!filename) {
    stateSetMsg(state, WARN, "No filename given, nothing saved!");
  }
  else switch (ioSaveToFile(filename, state->lines, state->numLines)) {
    case SUCCESS:
      state->dirty = 0;
      stateFormatMsg(state, INFO, "\"%s\" successfully written to file", filename);
      break;
    case BAD_PERMISSIONS:
      stateFormatMsg(state, WARN, "Missing write permissions for \"%s\", nothing saved!", filename);
      break;
    default:
      stateFormatMsg(state, ERROR, "Failed to write \"%s\" to file", filename);
      break;
  }
}

void _editorShowHelpMsg(EditorState *state) {
  NOTNULL_(state);
  char *helpMsg = "C-{wasd}: movement | C-x: delete under cursor | C-f: save | C-q: quit | ESC: force quit";
  stateSetMsg(state, INFO, helpMsg);
}

void editorProcessKey(EditorState *state, char c) {
  NOTNULL_(state);

  state->lastKey = c;

  if (isprint(c))
    _editorInsertChar(state, c);

  else switch (c) {
    case '\n':
      _editorInsertNewline(state);
      break;

    case CTRL_KEY('q'):
      if (!state->dirty)
    case '\x1b':
        state->running = 0;
      else
        stateSetMsg(state, WARN, "File has unsaved changes! Use ESC to force quit");
      break;

    case CTRL_KEY('x'):
      _editorDeleteChar(state);
      break;

    case CTRL_KEY('w'):
    case CTRL_KEY('a'):
    case CTRL_KEY('s'):
    case CTRL_KEY('d'):
      _editorProcessCursorMovementKey(state, c);
      break;

    case CTRL_KEY('f'):
      _editorDoSaveToFile(state);
      break;

    case CTRL_KEY('h'):
      _editorShowHelpMsg(state);
      break;
  }
}

#undef CTRL_KEY
