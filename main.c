#include <stdio.h> // fprintf, printf, fread, fopen, fclose

#include "terminal.h"
#include "editor_state.h"
#include "editor.h"
#include "screen.h"

int main(int argc, char **argv) {

  if (initTerminal() != 0) {
    fprintf(stderr, "Failed to init terminal raw mode, exiting\n");
    return 1;
  }

  EditorState *state = stateInit();
  state->filename = argc >= 2 ? argv[1] : NULL;
  state->running = 1;

  // TODO: placeholder. should insert better handling of empty files.
  stateAppendEmptyLine(state);

  while (state->running) {
    screenDrawEditorState(state);
    char c = editorReadKeyBlocking();
    editorProcessKey(state, c);
  }

  resetTerminalMode();

  stateFree(state);
  free(state);

  return 0;
}
