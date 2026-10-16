// PORT: the list stores ints in its data pointers; they come back through intptr_t (a pointer no longer fits in an int).
#pragma once
#include "MfDblList.h"

// mfutil.dll: a list of ints (the data pointers hold the values).
class __declspec(dllimport) MfIntList : public MfDblList {
public:
    MfIntList();
    virtual ~MfIntList();
    int First(MfDblListItem *&pli) const
    {
        pli = pliFirst;
        return pli ? (int)(intptr_t)pli->GetData() : 0; // PORT: intptr_t
    }
    int Next(MfDblListItem *&pli) const
    {
        if (pli) {
            pli = pli->GetNext();
            if (pli)
                return (int)(intptr_t)pli->GetData(); // PORT: intptr_t
        }
        return 0;
    }
};
