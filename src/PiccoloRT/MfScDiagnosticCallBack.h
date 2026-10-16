#pragma once
#include "MfString.h"

// One diagnostic message and its level.
class __declspec(dllexport) MfScDiagnosticInfo {
public:
    MfScDiagnosticInfo() : mLevel(0) {}
    MfString const & Message() const;
    int Level() const;
    MfString mMessage;  // +0
    int mLevel;         // +4
};

// Receiver of the diagnostic messages (MfScDiagnostics::AddOutput).
class __declspec(dllexport) MfScDiagnosticCallBack {
public:
    MfScDiagnosticCallBack();
    MfScDiagnosticCallBack(MfScDiagnosticCallBack const &);
    virtual ~MfScDiagnosticCallBack() {}
    virtual void CallBack(MfScDiagnosticInfo const &);
};
