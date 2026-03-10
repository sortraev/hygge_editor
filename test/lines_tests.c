#include <stdio.h>
#include <string.h>

#include "lines.h"
#include "assert.h"

int main(void) {

  // empty Lines
  {
    Lines lines = linesEmpty();

    ASSERT_(lines.lineBufs == NULL);
    ASSERT_(lines.numLines == 0);
    ASSERT_(lines.cap == 0);

    linesFree(&lines);
  }

  // append empty line
  {
    Lines lines = linesEmpty();

    ASSERT_(linesAppendEmpty(&lines) == 0);
    ASSERT_(lines.lineBufs != NULL);
    ASSERT_(lines.cap >= 1);
    ASSERT_(lines.numLines == 1);

    // assert new line has a non-null but empty string
    ASSERT_(lines.lineBufs[0].s != NULL);
    ASSERT_(lines.lineBufs[0].len == 0);
    ASSERT_(lines.lineBufs[0].cap > 0);

    linesFree(&lines);
  }

  // insert string in line
  {
    Lines lines = linesEmpty();

    ASSERT_(linesAppendEmpty(&lines) == 0);
    ASSERT_(linesInsertString(&lines, 0, 0, "foo bar") == 0);
    ASSERT_(strcmp(lines.lineBufs[0].s, "foo bar") == 0);

    linesFree(&lines);
  }

  // insert string in arbitrary line
  {
    Lines lines = linesEmpty();

    ASSERT_(linesAppendEmpty(&lines) == 0);
    ASSERT_(linesAppendEmpty(&lines) == 0);
    ASSERT_(linesAppendEmpty(&lines) == 0);
    ASSERT_(linesInsertString(&lines, 1, 0, "foo") == 0);
    ASSERT_(linesInsertString(&lines, 1, 3, " bar") == 0);

    ASSERT_(lines.lineBufs[0].s != NULL);
    ASSERT_(lines.lineBufs[1].s != NULL);
    ASSERT_(lines.lineBufs[2].s != NULL);

    ASSERT_(strcmp(lines.lineBufs[0].s, "") == 0);
    ASSERT_(strcmp(lines.lineBufs[1].s, "foo bar") == 0);
    ASSERT_(strcmp(lines.lineBufs[2].s, "") == 0);

    linesFree(&lines);
  }

  printf("Lines tests: Success\n");
}
