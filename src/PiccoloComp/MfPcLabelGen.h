#pragma once
#include "MfString.h"

// Makes the unique jump labels of a compilation: "@@L1", "@@L2", ...
class MfPcLabelGen {
public:
    MfPcLabelGen() : mNext(1) {}
    MfString NewLabel();
    int mNext;                  // +0x00
};
