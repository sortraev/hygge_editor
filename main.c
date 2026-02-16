#include <stdio.h> // fprintf, printf, fread, fopen, fclose

#include <ctype.h> // iscntrl

#include "editor.h"
#include "terminal.h"
#include "state.h"

int main(int argc, char **argv) {

  FILE *f = stdout;

  if (argc >= 2) {
    char *filename = argv[1];
    f = fopen(filename, "rw");
    if (!f) {
      fprintf(stderr, "Failed to open file \"%s\".\n", filename);
      return 1;
    }
  }

  if (init_terminal_mode() != 0) {
    fprintf(stderr, "Failed to init terminal raw mode, exiting\n");
    fclose(f);
    return 1;
  }

  State *state = stateInit();
  state->outFile = f;
  state->running = 1;

  while (state->running) {
    refreshScreen(state);
    char c = readKeyBlocking();
    processKey(state, c);
  }

  reset_terminal_mode();

  stateFree(state);
  free(state);
  fclose(f);

  return 0;
}
