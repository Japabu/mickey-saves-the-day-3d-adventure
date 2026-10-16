#pragma once
// MfDblList, imported from mfutil.dll. Declaration order as in mfutil's
// MfDblList_unit.h (VC5 lays overloaded virtuals out in reverse order).
// Inline here (the callers in MfActorLib inline them): the constructor,
// GetFirst/GetNext (a static list is walked through pliFirst/pliNext) and
// Prepend/Add (their virtual Insert/Append call stays virtual).
class MfCallBack;
class MfListItemInterface;

class MfDblListItem {
public:
    void *pData;            // +0
    MfDblListItem *pliPrev; // +4
    MfDblListItem *pliNext; // +8
};

class __declspec(dllimport) MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();
    int GetCount() const { return nCount; }
    virtual void Prepend(void *const pData) { Insert(pData, LP_BEFORE, pliFirst); }
    virtual void Append(void *const pData);
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli);
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli);
    virtual void Add(void *const pData) { Append(pData); }
    virtual void Copy(const MfDblList &list);
    virtual void Copy(const MfDblList &list, MfCallBack &cb);
    virtual void Copy(const MfDblList &list, MfListItemInterface &lii);
    virtual void *GetFirst(MfDblListItem *&pli) const
    {
        pli = pliFirst;
        return pli ? pli->pData : 0;
    }
    virtual void *GetNext(MfDblListItem *&pli) const
    {
        if (pli)
            pli = pli->pliNext;
        return pli ? pli->pData : 0;
    }
    virtual void *GetCurrent(MfDblListItem *&pli) const;
    virtual void *GetPrev(MfDblListItem *&pli) const;
    virtual void *GetLast(MfDblListItem *&pli) const;
    virtual void Iterate(MfCallBack &cb) const;
    virtual void Iterate(MfListItemInterface &lii) const;
    virtual void *DeleteFirst();
    virtual void *DeleteLast();
    virtual void *Delete(MfDblListItem *&pli);
    virtual void Clear();
    virtual void Clear(MfCallBack &cb);
    virtual void Clear(MfListItemInterface &lii);
    virtual MfDblListItem *Find(void *pData) const;
    virtual MfDblListItem *Find(void *pData, MfCallBack &cb) const;
    virtual MfDblListItem *Find(void *pData, MfListItemInterface &lii) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const;
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const;

private:
    MfDblList(const MfDblList &);
    MfDblList &operator=(const MfDblList &);
    MfDblListItem *GetListPosition(MfDblListItem *pli, int n) const;

protected:
    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};
