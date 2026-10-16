#pragma once
#include "MfDblList.h"
#include "MfListItemInterface.h"
#include "MfString.h"

// One call of a script thread's call stack (MfScVmEnv::GetCurrentCallStack).
class MfScVmCallStackEntry {
public:
    MfScVmCallStackEntry() : mOffset(0), mLine(-1) {}
    ~MfScVmCallStackEntry();

    MfString mFunction;             // +0x00
    MfString mModule;               // +0x04
    int mOffset;                    // +0x08 the bytecode offset of the call
    int mLine;                      // +0x0c its source line, -1 if unknown
};

// The calls of a script thread, innermost first; owns its entries.
class MfScVmCallStackList : public MfDblList, public MfListItemInterface {
public:
    virtual ~MfScVmCallStackList() { Clear(); }
    virtual void Clear();
    virtual void Destroy(void *item);
};
