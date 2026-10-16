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

class TRuntimeStack {
    TRuntimeStackItem stack[30];
    long cursor;
public:
    TRuntimeStack(void);
    ~TRuntimeStack(void);
    int CheckOverflow(void);
    int CheckUnderflow(void);
    void Push(const char *str);
    void Push(short item);
    void Push(float item);
    void Pop(const char **strPtr);
    void Pop(short *itemPtr);
    void Pop(float *itemPtr);
    float PopNumber(void);
    short PopIndex(void);
    short PopAddress(void);
    const char *PopString(void);
};

#endif
