#ifndef EDITOR_H
#define EDITOR_H

#include "editor_state.h"

void editorProcessKey(EditorState *state, char c);
char editorReadKeyBlocking(void);

#endif // EDITOR_H
