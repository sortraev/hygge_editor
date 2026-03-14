#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "dims.h"
#include "string_buffer.h"

typedef enum {
  NONE,
  INFO,
  WARN,
  ERROR
} MsgType;

typedef struct {
  int running;

  StringBuffer *lines;
  size_t numLines;
  size_t lineCap;

  Dims cursor;
  Dims windowDims;

  size_t windowOffset;

  char *filename;
  int dirty;

  int lastKey;

  MsgType msgType;
  char msgBuf[256]; // TODO: dynamic..?

} EditorState;

EditorState *stateInit(void);
void stateFree(EditorState *state);

int stateInsertEmptyLine(EditorState *state, size_t i); 
int stateAppendEmptyLine(EditorState *state);

int stateInsertString(EditorState *state, size_t line, size_t col, char *s);
int stateInsertChar(EditorState *state, size_t line, size_t col, char c);
int stateDeleteChar(EditorState *state, size_t line, size_t col);

int stateSetMsg(EditorState *state, MsgType msgType, char *msg);
int stateFormatMsg(EditorState *state, MsgType msgType, char *fmt, ...);
int stateClearMsg(EditorState *state);

#endif // EDITOR_STATE_H
