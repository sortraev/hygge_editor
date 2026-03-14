#ifndef STRING_BUFFER_H
#define STRING_BUFFER_H

#include <stddef.h>

typedef struct {
  char *s;
  size_t len; // length of string *excluding* null byte, i.e. len >= 0
  size_t cap; // buffer capacity including room for null byte, i.e. len < cap
} StringBuffer;

StringBuffer sbEmpty(void);
int sbInitWithCapacity(StringBuffer *sb, size_t initCap);
void sbFree(StringBuffer *sb);

int sbInsertString(StringBuffer *sb, size_t i, char *s);
int sbAppendString(StringBuffer *sb, char *s);

int sbInsertChar(StringBuffer *sb, size_t i, char c);
int sbAppendChar(StringBuffer *sb, char c);

int sbDeleteChar(StringBuffer *sb, size_t i);

// split a StringBuffer `sb` at index i, placing the tail in `tail`
int sbSplit(StringBuffer *sb, StringBuffer *tail, size_t i);

// shrink StringBuffer allocation size to fit content
int sbShrink(StringBuffer *sb);

#endif // STRING_BUFFER_H
