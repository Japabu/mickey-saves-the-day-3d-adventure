// PiccoloRT.dll 0x10016a80-0x10016d54: MfScVariableStack.
#include "MfScVariableStack.h"
#include "MfScHandleDict.h"
#include "MfScDataHandle.h"

// 0x10016a80 ??0MfScVariableStack@@QAE@H@Z
MfScVariableStack::MfScVariableStack(int size)
    : mSize(size), mCount(0), mPools(0)
{
    if (size <= 0)
        mSize = 10;
    mPools = new MfScHandleDict *[mSize];
    for (int i = 0; i < mSize; i++)
        mPools[i] = 0;
}

// 0x10016ae0 ??0MfScVariableStack@@QAE@PAVMfScHandleDict@@H@Z
MfScVariableStack::MfScVariableStack(MfScHandleDict *pool, int size)
    : mSize(size), mCount(0), mPools(0)
{
    if (size == -1)
        mSize = 1;
    else if (size == 0)
        mSize = 10;
    mPools = new MfScHandleDict *[mSize];
    mPools[mCount++] = pool;
    for (int i = mCount; i < mSize; i++)
        mPools[i] = 0;
}

// 0x10016b60 ??0MfScVariableStack@@QAE@ABV0@PAVMfScHandleDict@@H@Z
MfScVariableStack::MfScVariableStack(const MfScVariableStack &outer, MfScHandleDict *pool, int size)
    : mSize(size), mCount(0), mPools(0)
{
    if (size == -1)
        mSize = outer.mSize + 1;
    else if (size == 0)
        mSize = 10;
    if (mSize < outer.mSize + 1)
        mSize = outer.mSize + 1;
    mPools = new MfScHandleDict *[mSize];
    for (mCount = 0; mCount < outer.mCount; mCount++)
        mPools[mCount] = outer.mPools[mCount];
    mPools[mCount++] = pool;
    for (int i = mCount; i < mSize; i++)
        mPools[i] = 0;
}

// 0x10016c30 ??1MfScVariableStack@@UAE@XZ
MfScVariableStack::~MfScVariableStack()
{
    delete mPools;
}

// 0x10016c50 ?GetVariable@MfScVariableStack@@QBE?AW4MfScVariableErrorCode@1@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScVariableStack::MfScVariableErrorCode MfScVariableStack::GetVariable(const MfString &name, MfScDataHandle **h) const
{
    for (int i = mCount - 1; i >= 0; i--) {
        MfScDataHandle *value = mPools[i]->Find(name);
        if (value == pdhUndefined) {
            *h = 0;
            return kVarOk;
        }
        if (value) {
            value->AddRef();
            *h = value;
            return kVarOk;
        }
    }
    *h = 0;
    return kVarNotFound;
}

// 0x10016cc0 ?SetVariable@MfScVariableStack@@QAE?AW4MfScVariableErrorCode@1@ABVMfString@@PAVMfScDataHandle@@H@Z
MfScVariableStack::MfScVariableErrorCode MfScVariableStack::SetVariable(const MfString &name, MfScDataHandle *h, int passRef)
{
    MfScDataHandle *value = 0;
    int i;
    for (i = mCount - 1; i >= 0; i--) {
        value = mPools[i]->Find(name);
        if (value)
            break;
    }
    if (value) {
        if (value != pdhUndefined && value->IsReadOnly()) {
            if (h && passRef)
                h->Release();
            return kVarReadOnly;
        }
        mPools[i]->Replace(name, h ? h : pdhUndefined,
                           passRef ? MfScHandleDict::kPassRef : MfScHandleDict::kAddRef);
        return kVarOk;
    }
    return kVarNotFound;
}
