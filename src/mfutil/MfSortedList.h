// MfSortedList: MfDblList kept sorted by MfSortedListItem key (mfutil.dll).
// MfSortedList::Insert walks the list directly (MfDblList's protected members),
// as the original does.
#ifndef MFUTIL_MFSORTEDLIST_H
#define MFUTIL_MFSORTEDLIST_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfString.h"
#include "MfDblList.h"
#include "MfSortedListItem.h"

class MFUTIL_API MfSortedList : public MfDblList {
public:
    MfSortedList() {}
    virtual ~MfSortedList() {}
    void Add(MfSortedListItem *p) { MfDblList::Append(p); }
    void Append(MfSortedListItem *p) { MfDblList::Append(p); }
    // Add/Append/Prepend share one body with MfDblList::Append (0x10002190, folded)
    void Prepend(MfSortedListItem *p) { MfDblList::Append(p); }
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli);
};

#endif
