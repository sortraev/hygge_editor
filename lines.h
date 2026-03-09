#ifndef LINES_H
#define LINES_H

#include <stdarg.h>

#include "assert.h"
#include "util.h"
#include "string_buffer.h"

typedef struct {
  StringBuffer *lineBufs;
  size_t numLines;
  size_t cap;
} Lines;

Lines linesEmpty() {
  return (Lines) { NULL, 0, 0 };
}

int linesResize(Lines *lines, size_t newCap) {
  NOTNULL_(lines);
  ASSERT(newCap >= lines->cap, "linesResize(): downsizing not yet supported");

  StringBuffer *tmp = realloc(lines->lineBufs, newCap * sizeof(StringBuffer));
  if (!tmp) {
    fprintf(stderr, "linesResize(): Failed to reallocate lineBufs\n");
    return 1;
  }

  lines->lineBufs = tmp;
  lines->cap = newCap;

  if (lines->numLines >= newCap)
    lines->numLines = newCap;

  memset(lines->lineBufs + lines->numLines,
         0,
         (lines->cap - lines->numLines) * sizeof(StringBuffer));

  return 0;
}

int _linesInsertStringBuffer(Lines *lines, size_t i, StringBuffer lineBuf) {
  NOTNULL_(lines);

  if (i > lines->numLines) {
    return 1;
  }

  // expand if necessary
  if (lines->numLines + 1 > lines->cap) {
    int resizeStatus = linesResize(lines, lines->numLines + 1);
    if (resizeStatus != 0)
      return resizeStatus;
  }

  // move all lines after insertion point
  for (size_t j = lines->numLines; j > i; j--) {
    lines->lineBufs[j] = lines->lineBufs[j - 1];
  }

  // insert the new line
  lines->lineBufs[i] = lineBuf;
  lines->numLines++;

  return 0;
}

int linesInsertEmpty(Lines *lines, size_t i) {
  NOTNULL_(lines);

  StringBuffer sb = sbEmpty();
  if (sbInitWithCapacity(&sb, 16) != 0) {
    return 1;
  }
  return _linesInsertStringBuffer(lines, i, sb);
}

int linesAppendEmpty(Lines *lines) {
  NOTNULL_(lines);
  return linesInsertEmpty(lines, lines->numLines);
}

int linesInsertString(Lines *lines, size_t line, size_t col, char *s) {
  NOTNULL_(lines);
  NOTNULL_(s);

  if (line >= lines->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = lines->lineBufs + line;
  return sbInsertString(lineBuf, col, s);
}

int linesInsertChar(Lines *lines, size_t line, size_t col, char c) {
  NOTNULL_(lines);

  if (line >= lines->numLines) {
    return 1;
  }

  StringBuffer *lineBuf = lines->lineBufs + line;
  return sbInsertChar(lineBuf, col, c);
}

void linesFree(Lines *lines) {
  NOTNULL_(lines);
  if (lines) {
    for (size_t i = 0; i < lines->numLines; i++) {
      sbFree(lines->lineBufs + i);
    }
    free(lines->lineBufs);
  }
}

char *linesGetLine(Lines *lines, size_t i) {
  if (i >= lines->numLines) {
    return NULL;
  }
  return lines->lineBufs[i].s;
}

#endif // LINES_H
