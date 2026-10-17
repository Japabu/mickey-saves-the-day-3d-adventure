#pragma once
// MfListItem: base class of the objects an MfList holds.
#include "mfutil_api.h"

class MFUTIL_API MfListItem {
public:
    MfListItem();
    MfListItem(const MfListItem &);
    virtual ~MfListItem() {}
};
