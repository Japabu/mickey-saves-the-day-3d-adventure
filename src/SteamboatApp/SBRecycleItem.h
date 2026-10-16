#pragma once
// SBRecycleItem.h: an item of the recycle kind; it adds nothing to SBItem but its type (declaration from
// SteamboatApp.pdb).
#include "SBItem.h"

class SBRecycleItem : public SBItem {
public:
    SBRecycleItem() {}
    ~SBRecycleItem() {}
    virtual void MakePolymorphic();
};
