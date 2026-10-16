// MfScVirtualMachine (PiccoloRT.dll): runs compiled Piccolo functions.
#ifndef RUNTIMELIB_MFSCVIRTUALMACHINE_H
#define RUNTIMELIB_MFSCVIRTUALMACHINE_H
class MfScDataHandle;

class __declspec(dllimport) MfScVirtualMachine {
public:
    MfScVirtualMachine();
    virtual ~MfScVirtualMachine();
    // calls function with numArgs arguments; returns its result
    MfScDataHandle *__cdecl ExecuteFunction(MfScDataHandle *function, int numArgs, ...);
    MfScDataHandle *ExecuteFunctionArrayParams(MfScDataHandle *function, int numArgs,
                                               MfScDataHandle **args);

private:
    int m04;                                                                    // 0x04
};

#endif
