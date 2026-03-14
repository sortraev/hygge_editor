#ifndef EDITOR_H
#define EDITOR_H

#include <stdio.h>
#include <ctype.h> // isprint

#include "editor_state.h"
#include "io.h"

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

void _editorSetMsgBuf(EditorState *state, MsgType msgType, char *msg) {
  NOTNULL_(state);
  NOTNULL_(msg);

  strncpy(state->msgBuf, msg, 256);
  state->msgType = msgType;
}
void _editorClearMsgBuf(EditorState *state) {
  NOTNULL_(state);
  _editorSetMsgBuf(state, INFO, "");
}

void _editorFormatMsgBuf(EditorState *state, MsgType msgType, char *fmt, ...) {
  NOTNULL_(state);
  NOTNULL_(fmt);

  va_list args;
  va_start(args, fmt);
  vsnprintf(state->msgBuf, 256, fmt, args);
  va_end(args);
  state->msgType = msgType;
}

void _editorDoSaveToFile(EditorState *state) {
  NOTNULL_(state);

  char *filename = state->filename;
  if (!filename) {
    _editorSetMsgBuf(state, WARN, "No filename given, nothing saved!");
  }
  else switch (ioSaveToFile(filename, state->lines, state->numLines)) {
    case SUCCESS:
      state->dirty = 0;
      _editorFormatMsgBuf(state, INFO, "\"%s\" successfully written to file", filename);
      break;
    case BAD_PERMISSIONS:
      _editorFormatMsgBuf(state, WARN, "Missing write permissions for \"%s\", nothing saved!", filename);
      break;
    default:
      _editorFormatMsgBuf(state, ERROR, "Failed to write \"%s\" to file", filename);
      break;
  }
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
        _editorSetMsgBuf(state, WARN, "File has unsaved changes! Use ESC to force quit");
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
  }
}

char editorReadKeyBlocking(void) {
  char c;
  int num_read;
  while ((num_read = fread(&c, sizeof(char), 1, stdin)) != 1) {
    // TODO: handle num_read < 0 and errno != EAGAIN
  }

  // TODO: handle escape sequences (requires adding a timeout on stdin reads)

  return c;
}

#undef CTRL_KEY

#endif // EDITOR_H
