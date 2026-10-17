// MfSortedListItem: mfutil's item of a sorted list (PiccoloRT's view, until mfutil has its own header).
#pragma once
#include "MfString.h"

// Header-only list interface of mfutil (vtable emitted locally).
class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetKey() const = 0;
};
