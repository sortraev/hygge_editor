#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// none of these should be used directly. exported only to facilitate
// the subsequent macros.
void __assert(int cond, const char *fmt, ...);
void *__mallocOrDie(size_t allocSize, char *file, int line);
void *__reallocOrDie(void *p, size_t allocSize, char *file, int line);
void *__callocOrDie(size_t n, size_t elemSize, char *file, int line);

#ifdef DEBUG
#define ASSERT(cond, msg) \
  do {\
      __assert((cond), "Assertion failed: %s (%s:%d)\n", (msg), __FILE__, __LINE__);\
  } while (0)
#else
#define ASSERT(cond, msg)
#endif

#define NOTNULL(expr, msg) ASSERT((void*)(expr) != NULL, msg)

#define ASSERT_(cond) ASSERT((cond), "Assertion failed: " #cond)

#define NOTNULL_(expr) NOTNULL((expr), #expr " is null")

#define mallocOrDie(allocSize) __mallocOrDie((allocSize), __FILE__, __LINE__)
#define reallocOrDie(p, allocSize) __reallocOrDie((p), (allocSize), __FILE__, __LINE__)
#define callocOrDie(n, elemSize) __callocOrDie((n), (elemSize), __FILE__, __LINE__)

#endif // UTIL_H
