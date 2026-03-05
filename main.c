#include <stdio.h> // fprintf, printf, fread, fopen, fclose

#include <ctype.h> // iscntrl

#include "editor.h"
#include "terminal.h"
#include "state.h"

int main(int argc, char **argv) {

  if (initTerminal() != 0) {
    fprintf(stderr, "Failed to init terminal raw mode, exiting\n");
    return 1;
  }

  State *state = stateInit();
  state->filename = argc >= 2 ? argv[1] : NULL;
  state->running = 1;

  while (state->running) {
    refreshScreen(state);
    char c = readKeyBlocking();
    processKey(state, c);
  }

  resetTerminalMode();

  stateFree(state);
  free(state);

  return 0;
}
