// MfScVmEnv (PiccoloRT.dll): the execution state of a script thread. Only
// the variable stack is used here.
#ifndef RUNTIMELIB_MFSCVMENV_H
#define RUNTIMELIB_MFSCVMENV_H
#include "MfScVariableStack.h"
class MfScDataHandle;
class MfString;

class __declspec(dllimport) MfScVmEnv {
public:
    MfScVmEnv();
    virtual ~MfScVmEnv();
    static MfScVariableStack::MfScVariableErrorCode __cdecl
        SetGlobalVariable(const MfString &name, MfScDataHandle *value, int, int);
    static MfScVariableStack::MfScVariableErrorCode __cdecl
        GetGlobalVariable(const MfString &name, MfScDataHandle **value);
    static MfScVariableStack::MfScVariableErrorCode __cdecl
        ClearGlobalVariable(const MfString &name, int);
    // raises a script exception (MfPoView's constructor: 15, invalid object)
    void ThrowException(int code, const MfString &message);

    int mId;                                                                    // 0x04
    unsigned long mIp;                                                          // 0x08
    MfScDataHandle **mSp;                                                       // 0x0c
    unsigned long *mFrame;                                                      // 0x10
    unsigned long mArgs;                                                        // 0x14
    MfScDataHandle *mFunction;                                                  // 0x18
    MfScDataHandle *mObject;                                                    // 0x1c
    MfScDataHandle *mRegisters[4];                                              // 0x20
    // the variables the running code sees
    MfScVariableStack *mVarStack;                                               // 0x30
    unsigned long *mStackBase;                                                  // 0x34
    long mStackSize;                                                            // 0x38
    void *mStack;                                                               // 0x3c
    void *mException;                                                           // 0x40
    // an alternative return value of the running script (MfPoHFSM)
    int mAltReturnSet;                                                          // 0x44
    int mAltReturnValue;                                                        // 0x48
};

#endif
