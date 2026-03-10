#include <stdio.h>
#include <string.h>

#include "string_buffer.h"
#include "assert.h"

int main(void) {

  // uninitialized string buffer
  {
    StringBuffer sb = sbEmpty();
    ASSERT_(sb.s == NULL);
    ASSERT_(sb.len == 0);
    ASSERT_(sb.cap == 0);
  }

  // sbInitWithCapacity
  {
    StringBuffer sb = sbEmpty();

    size_t desiredCap = 137;
    ASSERT_(sbInitWithCapacity(&sb, desiredCap) == 0);
    ASSERT_(sb.cap >= desiredCap);
    ASSERT_(sb.len == 0);
    ASSERT_(sb.s != NULL);
    ASSERT_(sb.s[sb.len] == '\0');
    sbFree(&sb);
  }

  // sbInsertString single
  {
    StringBuffer sb = sbEmpty();
    ASSERT_(sbInsertString(&sb, 0, "foo bar") == 0);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    sbFree(&sb);
  }

  // sbInsertString multiple
  {
    StringBuffer sb = sbEmpty();
    ASSERT_(sbInsertString(&sb, 0, "foo baz") == 0);
    ASSERT_(sbInsertString(&sb, 4, "bar ") == 0);
    ASSERT_(strcmp(sb.s, "foo bar baz") == 0);

    sbFree(&sb);
  }

  // out-of-bounds sbInsertString
  {
    StringBuffer sb = sbEmpty();

    ASSERT_(sbInsertString(&sb, 0, "foo bar") == 0);

    // assert out-of-bounds insertions fail and buffer is unmodified
    ASSERT_(sbInsertString(&sb, 8, "baz") == 1);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    ASSERT_(sbInsertString(&sb, 1337, "baz") == 1);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    sbFree(&sb);
  }

  // sbInsertChar
  {
    StringBuffer sb = sbEmpty();
    ASSERT_(sbInsertString(&sb, 0, "foo bar") == 0);
    ASSERT_(sbInsertChar(&sb, 3, '$') == 0);
    ASSERT_(sbInsertChar(&sb, 3, ' ') == 0);
    ASSERT_(strcmp(sb.s, "foo $ bar") == 0);

    sbFree(&sb);
  }

  // out-of-bounds sbInsertChar
  {
    StringBuffer sb = sbEmpty();

    ASSERT_(sbInsertString(&sb, 0, "foo bar") == 0);

    // assert out-of-bounds insertion fails and buffer is unmodified
    ASSERT_(sbInsertChar(&sb, 8, '$') == 1);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    ASSERT_(sbInsertChar(&sb, 1337, '$') == 1);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    sbFree(&sb);
  }

  // interleaved sbInsertString and sbInsertChar
  {
    StringBuffer sb = sbEmpty();
    ASSERT_(sbInsertString(&sb, 0, "foo baz") == 0);
    ASSERT_(sbInsertString(&sb, 4, "ba") == 0);
    ASSERT_(sbInsertChar(&sb, 3, ',') == 0);
    ASSERT_(sbInsertChar(&sb, 7, ' ') == 0);
    ASSERT_(sbInsertString(&sb, 7, "r,") == 0);

    ASSERT_(strcmp(sb.s, "foo, bar, baz") == 0);

    sbFree(&sb);
  }

  // sbShrink
  {
    StringBuffer sb = sbEmpty();

    size_t initCap = 256;
    sbInitWithCapacity(&sb, initCap);

    ASSERT_(sbInsertString(&sb, 0, "foo") == 0);
    ASSERT_(sb.len == 3);

    ASSERT_(sbShrink(&sb) == 0);
    ASSERT_(sb.cap == sb.len + 1);

    sbFree(&sb);
  }

  // sbExpand
  {
    StringBuffer sb = sbEmpty();

    size_t initCap = 256;
    sbInitWithCapacity(&sb, initCap);

    ASSERT_(sbExpand(&sb) == 0);
    ASSERT_(sb.cap > initCap);

    sbFree(&sb);
  }

  // sbExpand on uninitialized buffer acts as initialization
  {
    StringBuffer sb = sbEmpty();

    ASSERT_(sbExpand(&sb) == 0);
    ASSERT_(sb.cap > 0);
    ASSERT_(sb.len == 0);
    ASSERT_(sb.s != NULL);

    sbFree(&sb);
  }

  // sbResize with a larger new capacity
  {
    StringBuffer sb = sbEmpty();

    char *s = "foo bar baz";
    ASSERT_(sbInsertString(&sb, 0, s) == 0);

    ASSERT_(sbResize(&sb, 128) == 0);
    ASSERT_(sb.cap == 128);

    // assert string preserved and rest of buffer is zeroed out
    ASSERT_(strcmp(sb.s, s) == 0);
    for (size_t i = strlen(s); i < sb.cap; i++) {
      ASSERT_(sb.s[i] == '\0');
    }

    sbFree(&sb);
  }

  // sbResize with a smaller new capacity truncates buffer
  {
    StringBuffer sb = sbEmpty();

    char *s = "foo bar baz";
    ASSERT_(sbInsertString(&sb, 0, s) == 0);

    ASSERT_(sb.cap > 8);

    ASSERT_(sbResize(&sb, 8) == 0);
    ASSERT_(sb.cap == 8);
    ASSERT_(sb.len < sb.cap);
    ASSERT_(strcmp(sb.s, "foo bar") == 0);

    sbFree(&sb);
  }

  // TODO: sbDeleteChar tests

  // sbSplit
  {
    StringBuffer sb = sbEmpty();
    StringBuffer tail = sbEmpty();
    ASSERT_(sbInsertString(&sb, 0, "foo bar") == 0);
    ASSERT_(sbSplit(&sb, &tail, 3) == 0);

    ASSERT_(strcmp(sb.s, "foo") == 0);
    ASSERT_(strcmp(tail.s, " bar") == 0);

    sbFree(&sb);
    sbFree(&tail);
  }

  // sbSplit at the end of buffer
  {
    StringBuffer sb = sbEmpty();
    StringBuffer tail = sbEmpty();

    char *s = "foo bar";
    ASSERT_(sbInsertString(&sb, 0, s) == 0);
    ASSERT_(sbSplit(&sb, &tail, strlen(s)) == 0);

    ASSERT_(strcmp(sb.s, s) == 0);
    ASSERT_(strcmp(tail.s, "") == 0);

    sbFree(&sb);
    sbFree(&tail);
  }

  // out-of-bounds sbSplit
  {
    StringBuffer sb = sbEmpty();
    StringBuffer tail = sbEmpty();

    char *s = "foo bar";
    ASSERT_(sbInsertString(&sb, 0, s) == 0);
    ASSERT_(sbSplit(&sb, &tail, 10) == 1);

    ASSERT_(strcmp(sb.s, s) == 0);
    ASSERT_(tail.s == NULL);
    ASSERT_(tail.len == 0);
    ASSERT_(tail.cap == 0);

    sbFree(&sb);
    sbFree(&tail);
  }

  printf("StringBuffer tests: Success\n");
}
