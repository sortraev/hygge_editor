#ifndef TERMINAL_H
#define TERMINAL_H

#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include "dims.h"
#include "util.h"

static int ORIG_TERMIOS_set = 0;
static struct termios ORIG_TERMIOS;

void resetTerminalMode(void) {
  if (ORIG_TERMIOS_set)
    tcsetattr(fileno(stdin), TCSAFLUSH, &ORIG_TERMIOS); // restore term attributes.
  printf("\x1b[?1049l"); // switch back from alternate screen.
}

int _setTerminalRawMode(void) {
  if (tcgetattr(fileno(stdin), &ORIG_TERMIOS) != 0) {
    fprintf(stderr, "Failed to backup termios: %s\n", strerror(errno));
    return 1;
  }
  ORIG_TERMIOS_set = 1;

  if (atexit(resetTerminalMode) != 0) {
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

int initTerminal(void) {

  if (_setTerminalRawMode() != 0) {
    fprintf(stderr, "Failed to set raw mode: %s\n", strerror(errno));
    return 1;
  }

  if (isatty(fileno(stdout))) {
    printf("\x1b[?1049h"); // switch to alternate screen.
  }

  return 0;
}

int getWindowDims(Dims *dims) {
  NOTNULL_(dims);

  struct winsize _winsize;
  if (ioctl(fileno(stdout), TIOCGWINSZ, &_winsize) == -1)
    return 1;
  dims->y = (size_t) _winsize.ws_row;
  dims->x = (size_t) _winsize.ws_col;
  return 0;
}

#endif // TERMINAL_H
