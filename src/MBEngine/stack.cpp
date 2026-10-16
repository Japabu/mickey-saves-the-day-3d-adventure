// stack.cpp: TRuntimeStack (MBEngine.dll, .\MBEngineRelease\stack.obj)
#include "stack.h"

// 0x1003c390 ??0TRuntimeStack@@QAE@XZ
TRuntimeStack::TRuntimeStack(void)
{
    cursor = 0;
}

// 0x1003c3a0 ??1TRuntimeStack@@QAE@XZ
TRuntimeStack::~TRuntimeStack(void)
{
}

// 0x1003c3b0 ?CheckOverflow@TRuntimeStack@@QAEHXZ
int TRuntimeStack::CheckOverflow(void)
{
    if (cursor == 30) {
        MBDebugStr("script stack overflow!\n");
        return 0;
    }
    return 1;
}

// 0x1003c3d0 ?CheckUnderflow@TRuntimeStack@@QAEHXZ
int TRuntimeStack::CheckUnderflow(void)
{
    if (cursor == 0) {
        MBDebugStr("script stack underflow!\n");
        return 0;
    }
    return 1;
}

// 0x1003c3f0 ?Push@TRuntimeStack@@QAEXM@Z
void TRuntimeStack::Push(float item)
{
    if (CheckOverflow()) {
        stack[cursor].num = item;
        cursor++;
    }
}

// 0x1003c410 ?Push@TRuntimeStack@@QAEXF@Z
void TRuntimeStack::Push(short item)
{
    if (CheckOverflow()) {
        stack[cursor].index = item;
        cursor++;
    }
}

// 0x1003c440 ?Push@TRuntimeStack@@QAEXPBD@Z
void TRuntimeStack::Push(const char *str)
{
    if (CheckOverflow()) {
        stack[cursor].str = str;
        cursor++;
    }
}

// 0x1003c470 ?Pop@TRuntimeStack@@QAEXPAM@Z
void TRuntimeStack::Pop(float *itemPtr)
{
    if (CheckUnderflow()) {
        cursor--;
        *itemPtr = stack[cursor].num;
    }
}

// 0x1003c490 ?Pop@TRuntimeStack@@QAEXPAF@Z
void TRuntimeStack::Pop(short *itemPtr)
{
    if (CheckUnderflow()) {
        cursor--;
        *itemPtr = stack[cursor].index;
    }
}

// 0x1003c4c0 ?Pop@TRuntimeStack@@QAEXPAPBD@Z
void TRuntimeStack::Pop(const char **strPtr)
{
    if (CheckUnderflow()) {
        cursor--;
        *strPtr = stack[cursor].str;
    }
}
