#ifndef ASSERT_H
#define ASSERT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void _assert(int cond, const char *fmt, ...) {
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

#define NOTNULL(expr, msg) ASSERT((void*)(expr) != NULL, msg)

#define STR(x) #x
#define NOTNULL_(expr) NOTNULL((expr), STR(expr) " is null")

#endif // ASSERT_H
