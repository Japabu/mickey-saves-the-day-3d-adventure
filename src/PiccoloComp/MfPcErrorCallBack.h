#pragma once
// MfPcErrorCallBack: the receiver of a compilation's errors (pure, no code).
#include "PiccoloComp_api.h"
class MfIPcErrorInfo;

// Receives the errors of a compilation.
class PICCOLOCOMP_API MfPcErrorCallBack {
public:
    virtual void CallBack(MfIPcErrorInfo *info) = 0;
};
