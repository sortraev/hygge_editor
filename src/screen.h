#ifndef SCREEN_H
#define SCREEN_H

#include "editor_state.h"

#define COLOR_WARN  "\x1b[0;33m" // yellow
#define COLOR_ERROR "\x1b[41m\x1b[1;30m" // bold black with red background

void screenDrawEditorState(EditorState *state);

#endif // SCREEN_H
