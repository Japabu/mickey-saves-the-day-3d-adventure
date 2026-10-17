// MfScPropertyExtList: the property extensions of a script class.
#pragma once
#include "PiccoloRT_api.h"
#include "MfDblList.h"
class MfScPropertyExt;

// The property extensions of a class; owns them (they are freed with operator delete).
class PICCOLORT_API MfScPropertyExtList : public MfDblList {
public:
    MfScPropertyExtList() {}
    virtual ~MfScPropertyExtList() { Clear(); }
    MfScPropertyExt *First(MfDblListItem *&pos) const
    {
        pos = pliFirst;
        return pos ? (MfScPropertyExt *)pos->pData : 0;
    }
    MfScPropertyExt *Next(MfDblListItem *&pos) const
    {
        if (pos)
            pos = pos->pliNext;
        return pos ? (MfScPropertyExt *)pos->pData : 0;
    }
    MfScPropertyExt *Current(MfDblListItem *&pos) const
    {
        return pos ? (MfScPropertyExt *)pos->pData : 0;
    }
    MfScPropertyExt *Prev(MfDblListItem *&pos) const
    {
        if (pos)
            pos = pos->pliPrev;
        return pos ? (MfScPropertyExt *)pos->pData : 0;
    }
    MfScPropertyExt *Last(MfDblListItem *&pos) const
    {
        pos = pliLast;
        return pos ? (MfScPropertyExt *)pos->pData : 0;
    }
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pos);
    virtual void Clear();
};
