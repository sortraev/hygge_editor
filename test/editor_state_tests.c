#include <stdio.h>
#include <string.h>

#include "editor_state.h"
#include "util.h"

int main(void) {

  // init state
  {
    EditorState *state = stateInit();

    ASSERT_(state->lines == NULL);
    ASSERT_(state->numLines == 0);
    ASSERT_(state->lineCap == 0);

    stateFree(state);
  }

  // stateAppendEmptyLine
  {
    EditorState *state = stateInit();

    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(state->lines != NULL);
    ASSERT_(state->numLines == 1);

    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(state->numLines == 2);

    // assert new lines have non-null but empty content
    for (int i = 0; i < state->numLines; i++) {
      ASSERT_(state->lines[i].s != NULL);
      ASSERT_(state->lines[i].len == 0);
      ASSERT_(state->lines[i].cap > 0);
    }

    stateFree(state);
  }

  // stateInsertString
  {
    EditorState *state = stateInit();

    ASSERT_(stateAppendEmptyLine(state) == 0);
    ASSERT_(stateInsertString(state, 0, 0, "foo bar") == 0);

    ASSERT_(state->numLines == 1);
    ASSERT_(state->lines[0].len == 7);
    ASSERT_(state->lines[0].cap > 7);
    ASSERT_(strcmp(state->lines[0].s, "foo bar") == 0);

    stateFree(state);
  }

  // stateInsertString in arbitrary line
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

  // out-of-bounds stateInsertString
  {
    EditorState *state = stateInit();

    size_t numLines = 3;
    for (int i = 0; i < numLines; i++)
      ASSERT_(stateAppendEmptyLine(state) == 0);

    // line index out of bounds
    ASSERT_(stateInsertString(state, numLines + 3, 0, "foo") == 1);

    // col index out of bounds
    ASSERT_(stateInsertString(state, 2, 13, "foo") == 1);

    // both indices out of bounds
    ASSERT_(stateInsertString(state, numLines + 3, 13, "foo") == 1);

    // assert no line has been touched
    for (int i = 0; i < state->numLines; i++) {
      ASSERT_(state->lines[i].s != NULL);
      ASSERT_(state->lines[i].len == 0);
      ASSERT_(strcmp(state->lines[0].s, "") == 0);
    }

    stateFree(state);
  }

  // TODO: stateInsertChar
  // TODO: out-of-bounds stateInsertChar

  // TODO: stateDeleteChar
  // TODO: out-of-bounds stateDeleteChar

  // TODO: _stateResizeLines
  {
  }

  printf("EditorState tests: Success\n");
}
