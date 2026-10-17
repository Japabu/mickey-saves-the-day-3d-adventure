// stack.h: TRuntimeStack, the script runtime stack (PDB layout).
#ifndef STACK_H
#define STACK_H

extern "C" __declspec(dllimport) void MBDebugStr(const char *str);

union TRuntimeStackItem {
    const char *str;
    float num;
    short addr;
    short index;
};

#include "TRuntimeStack.h"

#endif
