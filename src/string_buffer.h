#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <string.h>
#include "util.h"

typedef struct {
  char *s;
  size_t len; // length of string *excluding* null byte, i.e. len >= 0
  size_t cap; // buffer capacity including room for null byte, i.e. len < cap
} StringBuffer;

StringBuffer sbEmpty(void) {
  return (StringBuffer) { NULL, 0, 0 };
}

// we want an init cap of > 1 to guarantee dynamic expansion always succeeds
// when expansion rate is > 1
#define MIN_INIT_CAP 16
#define MIN_EXPAND MIN_INIT_CAP
#define EXPANSION_RATE 1.5f

int sbInitWithCapacity(StringBuffer *sb, size_t initCap) {
  NOTNULL_(sb);

  initCap = initCap >= MIN_INIT_CAP ? initCap : MIN_INIT_CAP;

  sb->s = callocOrDie(initCap, sizeof(char));

  sb->cap = initCap;
  sb->len = 0;

  return 0;
}

int sbResize(StringBuffer *sb, size_t newCap) {
  NOTNULL_(sb);
  newCap = newCap >= 1 ? newCap : 1;

  sb->s = reallocOrDie(sb->s, newCap * sizeof(char));
  sb->cap = newCap;

  if (sb->len >= sb->cap)
    sb->len = sb->cap - 1;

  memset(sb->s + sb->len, 0, sb->cap - sb->len);

  ASSERT(sb->len < sb->cap, "String buffer len < cap invariant violated");

  return 0;
}

int sbExpand(StringBuffer *sb) {
  NOTNULL_(sb);
  size_t newCap = sb->cap <= 1 ? MIN_EXPAND : sb->cap * EXPANSION_RATE;
  return sbResize(sb, newCap);
}

int sbShrink(StringBuffer *sb) {
  NOTNULL_(sb);
  return sbResize(sb, sb->len + 1);
}

int sbInsertString(StringBuffer *sb, size_t i, char *s) {
  NOTNULL_(sb);
  NOTNULL_(s);

  if (i > sb->len) {
    return 1;
  }

  size_t sLen = strlen(s);
  size_t newLen = sb->len + sLen;
  if (newLen + 1 > sb->cap) { // +1 to account for null byte
    int resizeStatus = sbResize(sb, newLen + 1);
    if (resizeStatus != 0)
      return resizeStatus;
  }

  // make room for the new substring by shifting the tail of the string
  memmove(sb->s + i + sLen, sb->s + i, sb->len - i);
  // insert the new substring
  memcpy(sb->s + i, s, sLen);

  sb->len = newLen;
  return 0;
}
int sbAppendString(StringBuffer *sb, char *s) {
  NOTNULL_(sb);
  return sbInsertString(sb, sb->len, s);
}

int sbInsertChar(StringBuffer *sb, size_t i, char c) {
  NOTNULL_(sb);

  if (i > sb->len) {
    return 1;
  }

  size_t newLen = sb->len + 1;
  if (newLen + 1 > sb->cap) { // +1 to account for null byte
    int resizeStatus = sbExpand(sb);
    if (resizeStatus != 0)
      return resizeStatus;
  }

  // make room for the new byte by shifting the tail-end of the string
  // (including null byte) to the right
  char *src = sb->s + i;
  memmove(src + 1, src, sb->len - i);

  // insert the new byte
  sb->s[i] = c;
  sb->len++;

  return 0;
}
int sbAppendChar(StringBuffer *sb, char c) {
  NOTNULL_(sb);
  return sbInsertChar(sb, sb->len, c);
}


int sbDeleteChar(StringBuffer *sb, size_t i) {
  NOTNULL_(sb);

  if (i >= sb->len) {
    return 1;
  }

  // delete a byte by left-shifting tail-end of the string (including null byte)
  // left onto the deletion point
  char *dst = sb->s + i;
  memmove(dst, dst + 1, sb->len - i);
  sb->len--;

  // TODO: shrink sb? might be a bit expensive to do for every char deletion.
  if (sb->len * EXPANSION_RATE < sb->cap)
    sbShrink(sb);

  return 0;
}

// split a StringBuffer in two, placing the result in tail.
int sbSplit(StringBuffer *sb, StringBuffer *tail, size_t i) {
  NOTNULL_(sb);
  NOTNULL_(tail);

  if (i > sb->len) {
    return 1;
  }

  int insertStatus = sbInsertString(tail, 0, sb->s + i);
  if (insertStatus != 0) {
    return insertStatus;
  }

  memset(sb->s + i, 0, sb->len - i);
  sb->len = i;

  // TODO: what to do if this one fails..?
  sbShrink(sb);

  return 0;
}

void sbFree(StringBuffer *sb) {
  if (sb) {
    free(sb->s);
  }
}

#endif // STRING_BUFFER_H
