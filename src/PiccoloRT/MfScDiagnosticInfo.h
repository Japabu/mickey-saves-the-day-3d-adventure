// MfScDiagnosticInfo: one diagnostic message and its level.
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfString.h"

// One diagnostic message and its level.
class PICCOLORT_API MfScDiagnosticInfo {
public:
    MfScDiagnosticInfo() : mLevel(0) {}
    MfString const & Message() const;
    int Level() const;
    MfString mMessage;  // +0
    int mLevel;         // +4
};
