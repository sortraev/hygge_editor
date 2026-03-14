#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

void __assert(int cond, const char *fmt, ...) {
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
  }
}

void *__mallocOrDie(size_t allocSize, char *file, int line) {
  void *p = malloc(allocSize);
  __assert(p != NULL, "malloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}
void *__reallocOrDie(void *p, size_t allocSize, char *file, int line) {
  p = realloc(p, allocSize);
  __assert(p != NULL || allocSize == 0,
      "realloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}
void *__callocOrDie(size_t n, size_t elemSize, char *file, int line) {
  void *p = calloc(n, elemSize);
  __assert(p != NULL, "calloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}
