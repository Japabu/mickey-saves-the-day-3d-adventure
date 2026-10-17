// MfSortedListItem: an item stored in an MfSortedList (mfutil.dll; not exported).
// vtable: ~ +0, GetSortKey +4 (returns MfString by value).
#ifndef MFUTIL_MFSORTEDLISTITEM_H
#define MFUTIL_MFSORTEDLISTITEM_H

#include "MfString.h"

class MfSortedListItem {
public:
    virtual ~MfSortedListItem() {}
    virtual MfString GetSortKey() const = 0;
};

#endif
