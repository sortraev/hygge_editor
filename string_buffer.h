#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <string.h>
#include "assert.h"

typedef struct {
  char *s;
  size_t len;
  size_t cap;
} StringBuffer;

StringBuffer sbEmpty(void) {
  return (StringBuffer) { NULL, 0, 0 };
}

// we want an init cap of > 1 to guarantee dynamic expansion always succeeds
// when expansion rate is > 1
#define MIN_INIT_CAP 2
#define EXPANSION_RATE 1.5f

int sbInitWithCapacity(StringBuffer *sb, size_t initCap) {
  NOTNULL_(sb);

  initCap = initCap >= MIN_INIT_CAP ? initCap : MIN_INIT_CAP;

  sb->s = calloc(initCap, sizeof(char));
  if (!sb->s && initCap > 0) {
    return 1;
  }

  sb->cap = initCap;
  sb->len = 0;

  return 0;
}

int sbResize(StringBuffer *sb, size_t newCap) {
  NOTNULL_(sb);

  if (newCap <= 0) {
    fprintf(stderr, "sbResize(): invalid newCap: %ld\n", newCap);
    return 1;
  }

  char *tmp = realloc(sb->s, newCap * sizeof(char));
  if (!tmp) {
    fprintf(stderr, "sbResize(): failed to reallocate\n");
    return 1;
  }

  sb->s = tmp;
  sb->cap = newCap;

  if (sb->len >= sb->cap)
    sb->len = sb->cap - 1;

  memset(sb->s + sb->len, 0, sb->cap - sb->len);

  ASSERT(sb->len < sb->cap, "String buffer len < cap invariant violated");

  return 0;
}

int sbShrink(StringBuffer *sb) {
  NOTNULL_(sb);
  return sbResize(sb, sb->len + 1);
}

int sbAppendString(StringBuffer *sb, char *s) {
  NOTNULL_(sb);
  NOTNULL_(s);

  size_t newLen = sb->len + strlen(s);
  if (newLen + 1 > sb->cap) { // +1 to account for null byte
    int resizeStatus = sbResize(sb, newLen + 1);
    if (resizeStatus != 0)
      return resizeStatus;
  }

  strcpy(sb->s + sb->len, s);
  sb->len = newLen;
  sb->s[newLen] = '\0';
  return 0;
}

int sbAppendChar(StringBuffer *sb, char c) {
  NOTNULL_(sb);

  size_t newLen = sb->len + 1;
  if (newLen + 1 > sb->cap) { // +1 to account for null byte
    int resizeStatus = sbResize(sb, (size_t) sb->cap * EXPANSION_RATE);
    if (resizeStatus != 0)
      return resizeStatus;
  }
  sb->s[sb->len++] = c;
  return 0;
}

void sbFree(StringBuffer *sb) {
  if (sb) {
    free(sb->s);
  }
}

#endif // STRING_BUFFER_H
