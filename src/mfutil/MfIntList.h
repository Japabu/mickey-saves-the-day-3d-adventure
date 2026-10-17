// MfIntList: MfDblList of ints (mfutil.dll, 0x100049a0-0x10004a40).
// Reconstructed (no PDB): signatures from the export names. ??0MfIntList inlines
// the MfDblList constructor.
#ifndef MFUTIL_MFINTLIST_H
#define MFUTIL_MFINTLIST_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfDblList.h"

// size 0x10. The accessors return the data cast to int; their copies are
// folded with the MfDblList ones (Add, Last, First, Next, Current, Prev).
class MFUTIL_API MfIntList : public MfDblList {
public:
    MfIntList();
    virtual ~MfIntList();
    void Add(int n);
    int First(MfDblListItem *&pli);
    int Next(MfDblListItem *&pli);
    int Current(MfDblListItem *&pli);
    int Prev(MfDblListItem *&pli);
    int Last(MfDblListItem *&pli);
};

#endif
