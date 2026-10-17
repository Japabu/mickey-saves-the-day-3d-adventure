// MfScConstUsageInfo: a named constant a routine uses (debug information).
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfString.h"
#include "MfVariant.h"

// A named constant a routine uses (0x10 bytes).
class PICCOLORT_API MfScConstUsageInfo {
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
