// MfFSMList: a list of the parts of a behaviour that may own its items and
// notes its changes (RTTI name; vtable 100b5b14; 1007fc60-1007fdd0).
#ifndef RUNTIMELIB_MFFSMLIST_H
#define RUNTIMELIB_MFFSMLIST_H
#include "MfDblList.h"

class MfFSMList : public MfDblList {
public:
    MfFSMList(int ownsItems);
    virtual ~MfFSMList() { Clear(); }
    virtual void *Delete(MfDblListItem *&pli);
    // deletes the items it owns
    virtual void Clear();
    // appends item and marks the list changed (the parser; the argument is
    // evaluated before the test)
    void AddItem(void *item)
    {
        if (mTrackChanges == 1)
            mChanged = 1;
        Append(item);
    }
    void *Last() { return pliLast ? pliLast->GetData() : 0; }
    // removes item; deletes it if the list owns it and keep is not set
    void *Remove(void *item, int keep);

    int mOwnsItems;                                                             // 0x10
    int mTrackChanges;                                                          // 0x14
    int mChanged;                                                               // 0x18
};

#endif
