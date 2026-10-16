#pragma once
#include "MfDblList.h"
class MfScDataHandle;
class MfScPropTokenTable;
class MfString;

// Property extension of a script class (interface; vtable 0x100208c4).
class __declspec(dllexport) MfScPropertyExt {
public:
    MfScPropertyExt() {}
    virtual int GetProperty(MfString const &name, MfScDataHandle **value) = 0;
    virtual int SetProperty(MfScDataHandle *self, MfString const &name, MfScDataHandle *value, int passRef) = 0;
    virtual MfScPropTokenTable * GetPropTokenTable() { return 0; }
    int LookupPropertyToken(MfString const &);
};

// The property extensions of a class; owns them (they are freed with operator delete).
class __declspec(dllexport) MfScPropertyExtList : public MfDblList {
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
