#pragma once
#include "MfDblList.h"
class MfSortedListItem;

// mfutil.dll: list kept in key order. Only members are imported; the class
// itself is not, so PiccoloRT emits its vtable (0x1002063c) and destructor.
class MfSortedList : public MfDblList {
public:
    MfSortedList() {}
    __declspec(dllimport) virtual ~MfSortedList();
    __declspec(dllimport) virtual void Insert(void *item, EnumListPosition pos, MfDblListItem *where);
    void Add(MfSortedListItem *item) { Insert(item, LP_AFTER, pliLast); }
};
