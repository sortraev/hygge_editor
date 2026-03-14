#ifndef TERMINAL_H
#define TERMINAL_H

#include "dims.h"

void termDeinit(void);
int termInit(void);

int termGetWindowDims(Dims *dims);

#endif // TERMINAL_H
