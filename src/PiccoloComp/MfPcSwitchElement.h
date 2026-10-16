#pragma once
#include "MfPcParserItem.h"
#include "MfString.h"
class MfPcExpression;

// A case (or the default) of the switch being compiled (0x18 bytes).
class MfPcSwitchElement : public MfPcParserItem {
public:
    MfPcSwitchElement(int isDefault, MfString const &label, MfPcExpression *value)
        : mIsDefault(isDefault), mLabel(label), mValue(value) {}
    virtual ~MfPcSwitchElement() {}
    int mIsDefault;             // +0x0c
    MfString mLabel;            // +0x10
    MfPcExpression *mValue;     // +0x14
};
