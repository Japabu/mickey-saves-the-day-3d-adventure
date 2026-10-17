#pragma once
// MfPcErrorCallBack: the receiver of a compilation's errors (pure, no code).
#ifndef PICCOLOCOMP_API
#define PICCOLOCOMP_API __declspec(dllimport)
#endif
class MfIPcErrorInfo;

// Receives the errors of a compilation.
class PICCOLOCOMP_API MfPcErrorCallBack {
public:
    virtual void CallBack(MfIPcErrorInfo *info) = 0;
};
