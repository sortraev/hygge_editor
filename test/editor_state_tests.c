#include <stdio.h>
#include <string.h>

#include "editor_state.h"
#include "util.h"

int main(void) {

  // empty Lines
  {
    EditorState *state = stateInit();

    ASSERT_(state->lines != NULL);
    ASSERT_(state->lineCap >= state->numLines);

    stateFree(state);
  }

  // append empty line
  {
    EditorState *state = stateInit();

    size_t numLines = state->numLines;
    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(state->lines != NULL);
    ASSERT_(state->numLines == numLines + 1);

    // assert new line has a non-null but empty string
    ASSERT_(state->lines[0].s != NULL);
    ASSERT_(state->lines[0].len == 0);
    ASSERT_(state->lines[0].cap > 0);

    stateFree(state);
  }

  // insert string in line
  {
    EditorState *state = stateInit();

    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(stateInsertString(state, 0, 0, "foo bar") == 0);
    ASSERT_(strcmp(state->lines[0].s, "foo bar") == 0);

    stateFree(state);
  }

  // insert string in arbitrary line
  {
    EditorState *state = stateInit();

    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(stateInsertString(state, 1, 0, "foo") == 0);
    ASSERT_(stateInsertString(state, 1, 3, " bar") == 0);

    ASSERT_(state->lines[0].s != NULL);
    ASSERT_(state->lines[1].s != NULL);
    ASSERT_(state->lines[2].s != NULL);

    ASSERT_(strcmp(state->lines[0].s, "") == 0);
    ASSERT_(strcmp(state->lines[1].s, "foo bar") == 0);
    ASSERT_(strcmp(state->lines[2].s, "") == 0);

    stateFree(state);
  }

  printf("EditorState tests: Success\n");
}
