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
        return pli ? (int)pli->GetData() : 0;
    }
    int Next(MfDblListItem *&pli) const
    {
        if (pli) {
            pli = pli->GetNext();
            if (pli)
                return (int)pli->GetData();
        }
        return 0;
    }
};
