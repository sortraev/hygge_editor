#ifndef SCREEN_H
#define SCREEN_H

#include "editor_state.h"

#define COLOR_WARN  "\x1b[0;33m" // yellow
#define COLOR_ERROR "\x1b[1;37m\x1b[41m" // bold white with red background

void screenDrawEditorState(EditorState *state);

#endif // SCREEN_H
