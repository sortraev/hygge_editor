#ifndef STRING_BUF_H
#define STRING_BUF_H

#include "string.h"

typedef struct {
  char *s;
  size_t len;
  size_t cap;
} StringBuffer;

StringBuffer sbEmpty() {
  return (StringBuffer) { NULL, 0, 0 };
}

StringBuffer *sbWithCapacity(size_t initCap) {
  StringBuffer *sb = malloc(sizeof(StringBuffer));
  if (!sb) {
    return NULL;
  }

  initCap = initCap < 1 ? 1 : initCap;
  sb->s = malloc(initCap * sizeof(char));
  if (!sb->s) {
    free(sb);
    return NULL;
  }

  sb->cap = initCap;
  sb->len = 0;
  sb->s[0] = '\0';

  return sb;
}

int sbResize(StringBuffer *sb, size_t newMaxLen) {
  size_t newCap = newMaxLen + 1;
  char *tmp = realloc(sb->s, newCap * sizeof(char));
  if (!tmp) {
    fprintf(stderr, "StringBuffer resize(): Failed to reallocate\n");
    return 1;
  }
  sb->s = tmp;
  sb->cap = newCap;

  if (sb->len > newCap - 1)
    sb->len = newCap - 1;
  memset(sb->s + sb->len, 0, sb->cap - sb->len);

  return 0;
}

int sbShrink(StringBuffer *sb) {
  return sbResize(sb, sb->len);
}

int sbAppendString(StringBuffer *sb, char *s) {

  size_t newLen = sb->len + strlen(s);
  if (newLen + 1 > sb->cap) {
    sbResize(sb, newLen);
  }

  strcpy(sb->s + sb->len, s);
  sb->len = newLen;
  sb->s[newLen] = '\0';
  return 1;
}

int sbAppendChar(StringBuffer *sb, char c) {
  size_t newLen = sb->len + 1;
  if (newLen + 1 > sb->cap) {
    sbResize(sb, (size_t) sb->len * 1.5);
  }
  sb->s[sb->len++] = c;
  return 0;
}

void sbFree(StringBuffer *sb) {
  free(sb->s);
}

int sbTest() {

  StringBuffer *sb = sbWithCapacity(0);

  printf("%s\n", sb->s);

  sbAppendString(sb, "Hej");
  sbAppendString(sb, " ");
  sbAppendString(sb, "Anders");
  sbAppendString(sb, "Anders");
  printf("%s\n", sb->s);
  sbAppendString(sb, "Anders");
  sbAppendString(sb, "Anders");
  printf("%s\n", sb->s);
  sbResize(sb, strlen("Hej Anders"));
  printf("%s\n", sb->s);

  sbAppendChar(sb, '$');
  sbAppendChar(sb, '$');
  sbAppendChar(sb, '$');
  sbAppendChar(sb, '$');
  sbAppendChar(sb, '$');
  printf("%s\n", sb->s);

  sbFree(sb);
  free(sb);

  return 0;
}

#endif // STRING_BUF_H
