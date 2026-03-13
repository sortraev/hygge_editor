#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void _assert(int cond, const char *fmt, ...) {
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
  }
}

#ifdef DEBUG
#define ASSERT(cond, msg) \
  do {\
      _assert((cond), "Assertion failed: %s (%s:%d)\n", (msg), __FILE__, __LINE__);\
  } while (0)
#else
#define ASSERT(cond, msg)
#endif

#define NOTNULL(expr, msg) ASSERT((void*)(expr) != NULL, msg)

#define ASSERT_(cond) ASSERT((cond), "Assertion failed: " #cond)

#define NOTNULL_(expr) NOTNULL((expr), #expr " is null")

void *_mallocOrDie(size_t allocSize, char *file, int line) {
  void *p = malloc(allocSize);
  _assert(p != NULL, "malloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}
void *_reallocOrDie(void *p, size_t allocSize, char *file, int line) {
  p = realloc(p, allocSize);
  _assert(p != NULL, "realloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}
void *_callocOrDie(size_t n, size_t elemSize, char *file, int line) {
  void *p = calloc(n, elemSize);
  _assert(p != NULL, "calloc() error: %s (%s:%d)\n", strerror(errno), file, line);
  return p;
}

#define mallocOrDie(allocSize) _mallocOrDie((allocSize), __FILE__, __LINE__)
#define reallocOrDie(p, allocSize) _reallocOrDie((p), (allocSize), __FILE__, __LINE__)
#define callocOrDie(n, elemSize) _callocOrDie((n), (elemSize), __FILE__, __LINE__)

#endif // UTIL_H
