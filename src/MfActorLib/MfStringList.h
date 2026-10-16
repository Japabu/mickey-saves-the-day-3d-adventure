#pragma once
// MfStringList (a MfDblList of MfString *), imported from mfutil.dll. Virtual
// order and inline members as in mfutil's MfStringList.h.
#include "MfString.h"
#include "MfDblList.h"

class __declspec(dllimport) MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

class __declspec(dllimport) MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    MfString *First(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetFirst(pli); }
    MfString *Next(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetNext(pli); }
    MfString *Current(MfDblListItem *&) const;
    MfString *Prev(MfDblListItem *&) const;
    MfString *Last(MfDblListItem *&) const;
    MfStringList &operator+(const MfString &);
    virtual MfString *Add(const MfString &s);
    virtual void Delete(const MfString &s);
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
    virtual MfDblListItem *Search(const MfString &s) const;
    void Sort(int bCaseSensitive);
private:
    static int StringCaseCompare(const void *a, const void *b);
    static int StringCompare(const void *a, const void *b);
    virtual int CallBack(const MfString &token, MfString &rest);
};
