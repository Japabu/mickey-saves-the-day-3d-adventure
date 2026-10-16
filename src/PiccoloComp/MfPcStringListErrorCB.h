#pragma once
#include "MfPcErrorCallBack.h"
class MfStringList;

// Collects the errors into a string list.
class __declspec(dllexport) MfPcStringListErrorCB : public MfPcErrorCallBack {
public:
    MfPcStringListErrorCB(MfStringList &errors, int brief) : mErrors(&errors), mBrief(brief) {}
    virtual void CallBack(MfIPcErrorInfo *info);
    MfStringList const &Errors() const { return *mErrors; }
    MfStringList *mErrors;      // +0x04
    int mBrief;             // +0x08
};
