#pragma once
#include "MfString.h"

// Name of a local variable, its frame slot and the code level it is declared at (0xc bytes).
class MfScVariableDebugInfo {
public:
    MfScVariableDebugInfo() : mSlot(-1), mLevel(-1) {}
    MfScVariableDebugInfo(const MfString &name, int slot, int level) : mName(name), mSlot(slot), mLevel(level) {}
    MfString mName;     // +0
    int mSlot;          // +4
    int mLevel;         // +8
};
