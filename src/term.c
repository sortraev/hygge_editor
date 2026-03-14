#include "term.h"

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <string.h>
#include <errno.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "dims.h"
#include "util.h"

static int ORIG_TERMIOS_set = 0;
static struct termios ORIG_TERMIOS;

int _setTerminalRawMode(void) {
  if (tcgetattr(fileno(stdin), &ORIG_TERMIOS) != 0) {
    fprintf(stderr, "Failed to backup termios: %s\n", strerror(errno));
    return 1;
  }
  ORIG_TERMIOS_set = 1;

  if (atexit(termDeinit) != 0) {
    fprintf(stderr, "Failed to set terminal mode exit handler: %s\n", strerror(errno));
    return 1;
  }
  struct termios raw = ORIG_TERMIOS;

  // TODO: go over these flags.
  // might want to add a timeout on reads in order to handle escape sequences.
  raw.c_iflag &= ~(IXON);
  raw.c_lflag &= ~(ECHO | ICANON);

  return tcsetattr(fileno(stdin), TCSAFLUSH, &raw);
}

void termDeinit(void) {
  if (ORIG_TERMIOS_set)
    tcsetattr(fileno(stdin), TCSAFLUSH, &ORIG_TERMIOS); // restore term attributes.
  printf("\x1b[?1049l"); // switch back from alternate screen.
}

int termInit(void) {

  if (_setTerminalRawMode() != 0) {
    fprintf(stderr, "Failed to set raw mode: %s\n", strerror(errno));
    return 1;
  }

  if (isatty(fileno(stdout))) {
    printf("\x1b[?1049h"); // switch to alternate screen.
  }

  return 0;
}

int termGetWindowDims(Dims *dims) {
  NOTNULL_(dims);

  struct winsize _winsize;
  if (ioctl(fileno(stdout), TIOCGWINSZ, &_winsize) == -1)
    return 1;

  dims->y = (size_t) _winsize.ws_row;
  dims->x = (size_t) _winsize.ws_col;
  return 0;
}
