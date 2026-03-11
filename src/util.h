#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef DEBUG
static void _assert(int cond, const char *fmt, ...) {
  if (!cond) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
  }
}
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

#endif // UTIL_H
