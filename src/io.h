#ifndef IO_H
#define IO_H

#include <string.h>
#include <errno.h>

#include "string_buffer.h"
#include "util.h"

typedef enum {
  SUCCESS,
  BAD_PERMISSIONS,
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

  NOTNULL_(filename);
  NOTNULL_(linesOut);
  NOTNULL_(numLinesOut);

  FILE *f = fopen(filename, "r");
  if (!f) {
    fprintf(stderr, "ioLoadFromFile: failed to open '%s': %s\n",
        filename, strerror(errno));
    if (errno == EACCES)
      return BAD_PERMISSIONS;
    return IO_ERROR;
  }

  StringBuffer *lines = NULL;
  size_t numLines = 0;

  char *s = NULL;
  ssize_t numRead;
  size_t allocSize;
  while ((numRead = getline(&s, &allocSize, f)) >= 0) {

    // if s contained a newline, truncate it
    if (s[numRead - 1] == '\n') {
      s[--numRead] = '\0';
    }

    numLines++;
    lines = reallocOrDie(lines, numLines * sizeof(StringBuffer));

    StringBuffer nextLine = { .s = s, .len = numRead, .cap = allocSize };
    sbShrink(&nextLine);

    lines[numLines - 1] = nextLine;

    s = NULL;
  }
  // getline allocates even on EOF or error, hence this last `s` must be freed
  free(s);

  IOStatus status = SUCCESS;
  if (feof(f)) {
    *linesOut = lines;
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

#endif // IO_H
