#ifndef INIT_H
#define INIT_H

#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

struct termios ORIG_TERMIOS;

int set_terminal_raw_mode(void) {
  struct termios raw = ORIG_TERMIOS;
  // TODO: go over these flags.
  raw.c_iflag &= ~(IXON);
  raw.c_lflag &= ~(ECHO | ICANON);

  return tcsetattr(fileno(stdin), TCSAFLUSH, &raw);
}

void reset_terminal_mode(void) {
  printf("\x1b[?1049l"); // switch back from alternate screen.
  // printf("\x1b[?25h");   // show cursor.
  tcsetattr(fileno(stdin), TCSAFLUSH, &ORIG_TERMIOS); // restore term attributes.
}

void sigint_handler(int sig) {
  if (sig == SIGINT)
    exit(0);
}

int init_raw_mode(void) {
  /*
   * check that we are writing to a tty; register exit handlers; init
   * ORIG_TERMIOS (for proper resetting to cooked mode); and set raw mode.
   */
  if (!isatty(fileno(stdout))) {
    fprintf(stderr, "stdout is not a tty\n");
    return 1;
  }
  if (tcgetattr(0, &ORIG_TERMIOS) != 0) {
    fprintf(stderr, "Failed to get original termios: %s\n", strerror(errno));
    return 1;
  }
  if (atexit(reset_terminal_mode) != 0) {
    fprintf(stderr, "Failed to set atexit: %s\n", strerror(errno));
    return 1;
  }
  if (set_terminal_raw_mode() != 0) {
    fprintf(stderr, "Failed to set raw mode: %s\n", strerror(errno));
    return 1;
  }

  // printf("\x1b[?25l");   // hide cursor.
  printf("\x1b[?1049h"); // switch to alternate screen.

  return 0;
}

int get_window_dims(int *rows, int *cols) {
  struct winsize _winsize;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &_winsize) == -1)
    return 1;
  *rows = _winsize.ws_row;
  *cols = _winsize.ws_col;
  return 0;
}

#endif // INIT_H
