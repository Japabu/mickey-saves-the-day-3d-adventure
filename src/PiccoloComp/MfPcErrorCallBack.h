#pragma once
class MfIPcErrorInfo;

// Receives the errors of a compilation.
class __declspec(dllexport) MfPcErrorCallBack {
public:
    virtual void CallBack(MfIPcErrorInfo *info) = 0;
};
