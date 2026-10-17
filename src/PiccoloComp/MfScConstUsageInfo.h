#pragma once
#include "MfString.h"
#include "MfVariant.h"

// PiccoloRT.dll: a named constant a routine uses (0x10 bytes).
class __declspec(dllimport) MfScConstUsageInfo {   // own copy: src/PiccoloRT/MfScConstUsageInfo.h includes PiccoloRT's MfString, which conflicts with the MfString kept here
public:
    MfScConstUsageInfo();
    MfScConstUsageInfo(MfString const &name, MfVariant const &value)
        : mName(name), mValue(value) {}
    MfScConstUsageInfo(MfScConstUsageInfo const &);
    virtual ~MfScConstUsageInfo() {}
    MfString mName;     // +4
    MfVariant mValue;   // +8
};
