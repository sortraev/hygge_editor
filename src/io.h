#ifndef IO_H
#define IO_H

#include "string_buffer.h"

typedef enum {
  SUCCESS,
  BAD_PERMISSIONS,
  FILE_NOT_EXISTS,
  IO_ERROR,
} IOStatus;

IOStatus ioSaveToFile(char *filename, StringBuffer *lines, size_t numLines);
IOStatus ioLoadFromFile(char *filename, StringBuffer **linesOut, size_t *numLinesOut);

#endif // IO_H
