// MfScDataMethod (PiccoloRT.dll): the base of the callable script data
// (bytecode routines, C functions, C++ methods).
#ifndef RUNTIMELIB_MFSCDATAMETHOD_H
#define RUNTIMELIB_MFSCDATAMETHOD_H
#include "MfScData.h"

class __declspec(dllimport) MfScDataMethod : public MfScData {
public:
    enum MfScMethodType { kNone, kFunction, kMethod, kExtMethod };
    MfScDataMethod(MfScMethodType type, int numArgs);
    virtual ~MfScDataMethod();

protected:
    int mMethodType;                                                            // 0x0c
    MfScDataHandle **mConstants;                                                // 0x10
    int mNumConstants;                                                          // 0x14
    int mNumArgs;                                                               // 0x18
};

#endif
