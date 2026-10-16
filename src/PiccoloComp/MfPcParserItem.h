#pragma once
#include "MfDblList.h"

// Base of everything the parser allocates. Every item registers itself in a
// global list so that the compiler can free what a failed parse left behind.
class MfPcParserItem {
public:
    MfPcParserItem();
    virtual ~MfPcParserItem();
    static void Initialize();
    static void FreeItems();

    int mRegistered;    // +0x04 1 while the item is in pdlItems
    int mKeep;          // +0x08 not freed by FreeItems
    static MfDblList *pdlItems;
};
