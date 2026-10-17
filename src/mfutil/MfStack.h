// MfStack: LIFO stack on top of MfDblList (mfutil.dll, 0x1000bde0-0x1000bee0).
// ??0MfStack inlines the MfDblList constructor.
#ifndef MFUTIL_MFSTACK_H
#define MFUTIL_MFSTACK_H

#include "mfutil_api.h"
#include "MfDblList.h"

// size 0x14
class MFUTIL_API MfStack : public MfDblList {
public:
    MfStack(int nMax = -1);
    // no user-declared destructor: the implicit one (0x1000be00) has no vptr store
    int Size() const { return GetCount(); }
    int GetMaxSize() const;     // exported
    int Push(void *pData);
    void *Pop();
    int IsFull();
    int IsEmpty();

    int nMax; // +0x10 (-1: unbounded)
};

#endif
