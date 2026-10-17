// MfScDiagnosticCallBack: a receiver of the diagnostic messages.
#pragma once
#include "PiccoloRT_api.h"
#include "MfString.h"
#include "MfScDiagnosticInfo.h"

// Receiver of the diagnostic messages (MfScDiagnostics::AddOutput).
class PICCOLORT_API MfScDiagnosticCallBack {
public:
    MfScDiagnosticCallBack();
    MfScDiagnosticCallBack(MfScDiagnosticCallBack const &);
    virtual ~MfScDiagnosticCallBack() {}
    virtual void CallBack(MfScDiagnosticInfo const &);
};
