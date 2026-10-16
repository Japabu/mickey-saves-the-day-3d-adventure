#pragma once
#include "MfDblList.h"

// mfutil.dll: a stack of void * (0x14 bytes); the constructor is inline here.
class __declspec(dllimport) MfStack : public MfDblList {
public:
    MfStack(int max = -1) : nMax(max) {}
    virtual ~MfStack();
    int Push(void *data);
    void *Pop();
    int IsFull();
    int IsEmpty();
    int nMax;                   // +0x10 -1: unbounded
};
