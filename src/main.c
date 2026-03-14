#include <stdio.h>
#include <stdlib.h>

#include "term.h"
#include "editor_state.h"
#include "editor.h"
#include "screen.h"
#include "io.h"

int main(int argc, char **argv) {

  if (termInit() != 0) {
    fprintf(stderr, "Failed to init terminal raw mode, exiting\n");
    return 1;
  }

  EditorState *state = stateInit();
  state->filename = argc >= 2 ? argv[1] : NULL;
  state->running = 1;

  if (argc >= 2) {
    StringBuffer *lines = NULL;
    size_t numLines = 0;
    if (ioLoadFromFile(argv[1], &lines, &numLines) == SUCCESS) {
      state->lines = lines;
      state->lineCap = state->numLines = numLines;
    }
  }

  // TODO: placeholder. should insert better handling of empty files.
  if (state->numLines == 0) {
    stateAppendEmptyLine(state);
    state->dirty = 0;
  }

  while (state->running) {
    screenDrawEditorState(state);
    char c = editorReadKeyBlocking();
    editorProcessKey(state, c);
  }

  termDeinit();

  stateFree(state);
  free(state);

  return 0;
}

