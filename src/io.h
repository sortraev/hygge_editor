#ifndef IO_H
#define IO_H

#include <string.h>
#include <errno.h>

#include "string_buffer.h"
#include "editor_state.h"

typedef enum {
  SUCCESS,
  BAD_PERMISSIONS,
  NO_FILENAME,
  IO_ERROR,
  DIRTY
} IOStatus;

IOStatus ioSaveToFile(EditorState *state) {

  NOTNULL_(state);

  char *filename = state->filename;
  if (!filename) {
    return NO_FILENAME;
  }

  FILE *f = fopen(filename, "w");

  if (!f) {
    fprintf(stderr, "Failed to open %s\n", strerror(errno));
    if (errno == EACCES)
      return BAD_PERMISSIONS;
    return IO_ERROR;
  }

  for (size_t i = 0; i < state->numLines; i++) {
    StringBuffer *line = state->lines + i;
    size_t n = line->len;

    if (fwrite(line->s, sizeof(char), n, f) < n 
        || fputc('\n', f) < 0) {
      // TODO: how to handle this error?
      return IO_ERROR;
    }
  }

  if (fclose(f) != 0) {
    fprintf(stderr, "Failed to close file '%s': %s, ignoring ...\n",
        filename, strerror(errno));
  }
  return SUCCESS;
}

#endif // IO_H
