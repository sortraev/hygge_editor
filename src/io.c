#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include "string_buffer.h"
#include "util.h"

#define READ_SIZE 8192

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

  NOTNULL_(filename);
  NOTNULL_(linesOut);
  NOTNULL_(numLinesOut);

  FILE *f = fopen(filename, "r");
  if (!f) {
    if (errno == ENOENT)
      return FILE_NOT_EXISTS;

    fprintf(stderr, "ioLoadFromFile: failed to open '%s': %s\n",
        filename, strerror(errno));

    if (errno == EACCES)
      return BAD_PERMISSIONS;
    return IO_ERROR;
  }

  size_t lineCap = 1;
  StringBuffer *lines = callocOrDie(lineCap, sizeof(StringBuffer));
  size_t i = 0;

  static char buf[READ_SIZE + 1];
  size_t numRead;

  // outer loop reads file in chunks; inner loop splits chunks into lines
  do {
    numRead = fread(buf, sizeof(char), READ_SIZE, f);
    buf[numRead] = '\0';

    char *s = buf;
    while (*s) {

      // NOTE: the use of strchr implies we do not support embedded NULL bytes
      char *nextLinebreak = strchr(s, '\n');

      int linebreakFound = nextLinebreak != NULL;
      if (linebreakFound)
        // if we found a linebreak, break string here before appending
        *nextLinebreak = '\0';

      sbAppendString(lines + i, s);

      if (!linebreakFound)
        break;

      s = nextLinebreak + 1;

      i++;
      if (i >= lineCap) {
        size_t newCap = lineCap * 2;
        lines = reallocOrDie(lines, newCap * sizeof(StringBuffer));
        memset(lines + i,
               0,
               (newCap - lineCap) * sizeof(StringBuffer));
        lineCap = newCap;
      }
    }
  } while (numRead == READ_SIZE);

  // handle non-newline-terminated line at end of input.
  if (lines[i].len > 0) {
    i++;
  }

  size_t numLines = i;
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
}

char ioReadKeyBlocking(void) {
  char c;
  int num_read;
  while ((num_read = fread(&c, sizeof(char), 1, stdin)) != 1) {
    // TODO: handle num_read < 0 and errno != EAGAIN
  }

  // TODO: handle escape sequences (requires adding a timeout on stdin reads)

  return c;
}
