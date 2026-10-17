#pragma once
#include "MfString.h"

// PiccoloRT.dll: name of a local variable, its frame slot and the code level
// it is declared at (0xc bytes).
class MfScVariableDebugInfo {   // own copy: src/PiccoloRT/MfScVariableDebugInfo.h includes PiccoloRT's MfString, which conflicts with the MfString kept here
public:
    MfScVariableDebugInfo(MfString const &name, int slot, int level)
        : mName(name), mSlot(slot), mLevel(level) {}
    MfString mName;     // +0
    int mSlot;          // +4
    int mLevel;         // +8
};
