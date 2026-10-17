// MfListItemInterface: mfutil's list item interface (PiccoloRT's view, until mfutil has its own header).
#pragma once

// Header-only list interface of mfutil (vtable emitted locally). Slot 2 is
// Destroy: MfScFunctionRegistryEntry/Module export their overrides under that
// name.
class MfListItemInterface {
public:
    virtual void *Copy(void *item) { return 0; }
    virtual int Iterate(void *item) { return 1; }
    virtual void Destroy(void *item) {}
    virtual int Matches(void *item, void *key) { return 0; }
};

#include "LiiExemplar.h"
