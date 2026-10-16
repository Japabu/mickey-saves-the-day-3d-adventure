#pragma once
#include "MfString.h"
#include "MfVariant.h"

// A named constant a routine uses (0x10 bytes).
class __declspec(dllexport) MfScConstUsageInfo {
public:
    MfScConstUsageInfo();
    MfScConstUsageInfo(MfString const &name, MfVariant const &value) : mName(name), mValue(value) {}
    MfScConstUsageInfo(MfScConstUsageInfo const &);
    virtual ~MfScConstUsageInfo() {}
    MfScConstUsageInfo const &operator=(MfScConstUsageInfo const &other)
    {
        mName = other.mName;
        mValue = other.mValue;
        return *this;
    }
    MfString const &Name() const { return mName; }
    MfVariant const &Value() const { return mValue; }
    MfString mName;     // +4
    MfVariant mValue;   // +8
};
