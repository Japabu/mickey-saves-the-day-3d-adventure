// TRuntimeStack.h - the script runtime's value stack.
#ifndef MBENGINE_TRUNTIMESTACK_H
#define MBENGINE_TRUNTIMESTACK_H

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
