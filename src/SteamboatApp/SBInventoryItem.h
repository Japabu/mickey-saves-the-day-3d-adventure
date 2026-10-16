#pragma once
// SBInventoryItem.h: an item of the inventory kind; it adds nothing to SBItem but its type (declaration from
// SteamboatApp.pdb).
#include "SBItem.h"

class SBInventoryItem : public SBItem {
public:
    SBInventoryItem() {}
    ~SBInventoryItem() {}
    virtual void MakePolymorphic();
};
