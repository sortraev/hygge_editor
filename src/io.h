#ifndef IO_H
#define IO_H

#include <string.h>
#include <errno.h>

#include "string_buffer.h"

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
    fprintf(stderr, "ioSaveToFile: failed to open %s\n", strerror(errno));
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

#endif // IO_H
