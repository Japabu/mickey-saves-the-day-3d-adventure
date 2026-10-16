// MfScVariableStack (PiccoloRT.dll): the variable pools a running script
// sees, innermost last; the pools are not owned.
#ifndef RUNTIMELIB_MFSCVARIABLESTACK_H
#define RUNTIMELIB_MFSCVARIABLESTACK_H
class MfScDataHandle;
class MfScHandleDict;
class MfString;

class __declspec(dllimport) MfScVariableStack {
public:
    enum MfScVariableErrorCode { kVarOk = 0, kVarNotFound = 1, kVarReadOnly = 2 };

    MfScVariableStack(int size);
    MfScVariableStack(MfScHandleDict *pool, int size);
    // the pools of outer and then pool
    MfScVariableStack(const MfScVariableStack &outer, MfScHandleDict *pool, int size);
    virtual ~MfScVariableStack();

    MfScVariableErrorCode GetVariable(const MfString &name, MfScDataHandle **h) const;
    MfScVariableErrorCode SetVariable(const MfString &name, MfScDataHandle *h, int passRef);

private:
    int mSize;                                                                  // 0x04
    int mCount;                                                                 // 0x08
    MfScHandleDict **mPools;                                                    // 0x0c
};

#endif
