#ifndef IO_H
#define IO_H

#include <string.h>
#include <errno.h>

#include "string_buffer.h"
#include "util.h"

typedef enum {
  SUCCESS,
  BAD_PERMISSIONS,
  FILE_NOT_EXISTS,
  IO_ERROR,
} IOStatus;

IOStatus ioSaveToFile(char *filename, StringBuffer *lines, size_t numLines) {

  NOTNULL_(filename);
  NOTNULL_(lines);

  FILE *f = fopen(filename, "w");
  if (!f) {
    fprintf(stderr, "ioSaveToFile: failed to open '%s': %s\n",
        filename, strerror(errno));
    if (errno == EACCES)
      return BAD_PERMISSIONS;
    return IO_ERROR;
  }

  IOStatus status = SUCCESS;
  for (size_t i = 0; i < numLines; i++) {
    size_t n = lines[i].len;
    if (fwrite(lines[i].s, sizeof(char), n, f) < n
        || fputc('\n', f) < 0) {
      fprintf(stderr, "ioSaveToFile: failure during write\n");
      status = IO_ERROR;
      break;
    }
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "ioSaveToFile: failed to close file '%s': %s, ignoring ...\n",
        filename, strerror(errno));
  }
  return status;
}

IOStatus ioLoadFromFile(char *filename, StringBuffer **linesOut, size_t *numLinesOut) {
#define BUF_SIZE 8192

  NOTNULL_(filename);
  NOTNULL_(linesOut);
  NOTNULL_(numLinesOut);

  FILE *f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "ioLoadFromFile: failed to open '%s': %s\n",
        filename, strerror(errno));

    switch (errno) {
      case ENOENT: return FILE_NOT_EXISTS;
      case EACCES: return BAD_PERMISSIONS;
      default: return IO_ERROR;
    }
  }

  size_t lineCap = 1;
  StringBuffer *lines = callocOrDie(lineCap, sizeof(StringBuffer));
  size_t numLines = 0;

  static char buf[BUF_SIZE + 1];
  size_t numRead;

  // outer loop reads file in chunks; inner loop splits chunks into lines
  do {
    numRead = fread(buf, sizeof(char), BUF_SIZE, f);
    buf[numRead] = '\0';

    char *s = buf;
    while (*s) {

      // NOTE: the use of strchrnul implies we do not support embedded NULL bytes
      char *nextLinebreak = strchrnul(s, '\n');

      int linebreakFound = *nextLinebreak == '\n';

      // break the line and append it!
      *nextLinebreak = '\0';
      sbAppendString(lines + numLines, s);

      if (!linebreakFound) {
        break;
      }

      s = nextLinebreak + 1;

      numLines++;
      if (numLines >= lineCap) {
        size_t newCap = lineCap * 2;
        lines = reallocOrDie(lines, newCap * sizeof(StringBuffer));
        memset(lines + numLines,
               0,
               (newCap - lineCap) * sizeof(StringBuffer));
        lineCap = newCap;
      }
    }
  } while (numRead == BUF_SIZE);

  // handle non-newline-terminated line at end of input
  if (lines[numLines].len > 0) {
    numLines++;
  }

  IOStatus status = SUCCESS;
  if (feof(f)) {
    if (numLines > 0) {
      *linesOut = reallocOrDie(lines, numLines * sizeof(StringBuffer));
    }
    *numLinesOut = numLines;
  }
  else {
    // TODO: find out how to best handle this case
    free(lines);
    status = IO_ERROR;
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "ioLoadFromFile: failed to close file '%s': %s, ignoring ...\n",
        filename, strerror(errno));
  }

  return status;
#undef BUF_SIZE
}

#endif // IO_H
