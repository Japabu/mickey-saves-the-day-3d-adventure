#pragma once
#include "../mfutil/MfCriticalSection.h"
class MfScDataHandle;
class MfScVmStack;

// One of the three pools data handles are allocated from (0: plain values,
// 1: objects, 2: containers). The handles lie in one block of address space
// that is committed a few pages at a time.
class MfScHandleHeap {
public:
    MfScHandleHeap();
    ~MfScHandleHeap();
    // commits more handles; returns the first of them (0 when out of memory)
    MfScDataHandle *GrowHeap();
    int Contains(MfScDataHandle *handle);
    void Lock() { m_cs.Enter(); }
    void Unlock() { m_cs.Leave(); }
    int m_unused;               // +0x00 never set
    MfScDataHandle *m_free;     // +0x04
    MfScDataHandle *m_nextUnused; // +0x08 handles from here on were never used
    MfScVmStack *m_stack;       // +0x0c
    unsigned long m_numFree;    // +0x10
    unsigned long m_numHandles; // +0x14
    MfCriticalSection m_cs;     // +0x18
};
