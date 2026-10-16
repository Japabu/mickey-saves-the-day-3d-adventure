#pragma once
#include "MfPcErrorCallBack.h"
class MfString;

// Collects the errors into a string.
class __declspec(dllexport) MfPcStringErrorCB : public MfPcErrorCallBack {
public:
    MfPcStringErrorCB(MfString &errors, int brief) : mErrors(&errors), mBrief(brief) {}
    virtual void CallBack(MfIPcErrorInfo *info);
    MfString const &Errors() const { return *mErrors; }
    MfString *mErrors;          // +0x04
    int mBrief;             // +0x08
};
