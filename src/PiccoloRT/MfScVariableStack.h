// MfScVariableStack: the variable pools visible to running script code.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
class MfScDataHandle;
class MfScHandleDict;
class MfString;

// The variable pools visible to running code, innermost last. The pools are
// not owned.
class PICCOLORT_API MfScVariableStack {
public:
    enum MfScVariableErrorCode {
        kVarOk = 0,
        kVarNotFound = 1,
        kVarReadOnly = 2,
        kVarOverflow = 3,       // PushVarPool: no room
        kVarUnderflow = 4,      // PopVarPool: no pool
        kVarBadIndex = 5        // Get/SetVarPoolAtIndex
    };
    MfScVariableStack(int size);
    MfScVariableStack(MfScHandleDict *pool, int size);
    MfScVariableStack(const MfScVariableStack &outer, MfScHandleDict *pool, int size);
    virtual ~MfScVariableStack();

    MfScVariableErrorCode GetVariable(const MfString &name, MfScDataHandle **h) const;
    MfScVariableErrorCode SetVariable(const MfString &name, MfScDataHandle *h, int passRef);
    MfScVariableErrorCode PushVarPool(MfScHandleDict *pool);
    MfScVariableErrorCode PopVarPool();
    MfScVariableErrorCode GetVarPoolAtIndex(int index, MfScHandleDict **pool);
    MfScVariableErrorCode SetVarPoolAtIndex(int index, MfScHandleDict *pool);

private:
    int mSize;                  // +0x04
    int mCount;                 // +0x08
    MfScHandleDict **mPools;    // +0x0c
};
