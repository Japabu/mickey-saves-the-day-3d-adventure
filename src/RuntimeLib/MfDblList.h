// MfDblList (mfutil.dll): doubly linked list of void*. Declaration as in
// src/mfutil (member names from SteamboatApp.pdb); the constructor is inline in
// the original header (callers store the imported vtable and clear the links).
#ifndef RUNTIMELIB_MFDBLLIST_H
#define RUNTIMELIB_MFDBLLIST_H

// vtable: ~, CallBack(a,b,c) +4, CallBack(a,b) +8, CallBack(a) +0xc
class __declspec(dllimport) MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *a);
    virtual int CallBack(void *a, void *b);
    virtual int CallBack(void *a, void *b, void *c);
};

// vtable: Copy +0, Iterate +4, Destroy +8, Matches +0xc
class MfListItemInterface {
public:
    virtual void *Copy(void *pData) = 0;
    virtual int Iterate(void *pData) = 0;
    virtual void Destroy(void *pData) = 0;
    virtual int Matches(void *pKey, void *pData) = 0;
};

class MfDblListItem {
public:
    MfDblListItem(void *p) { pliPrev = 0; pData = p; pliNext = 0; }
    void *GetData() const { return pData; }
    MfDblListItem *GetPrev() const { return pliPrev; }
    MfDblListItem *GetNext() const { return pliNext; }
    void SetData(void *p) { pData = p; }
    void SetPrev(MfDblListItem *p) { pliPrev = p; }
    void SetNext(MfDblListItem *p) { pliNext = p; }
    void SetLinks(MfDblListItem *prev, MfDblListItem *next)
    {
        pliPrev = prev;
        pliNext = next;
        if (pliPrev) pliPrev->pliNext = this;
        if (pliNext) pliNext->pliPrev = this;
    }

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
    virtual void Append(void *const pData) { Insert(pData, LP_AFTER, pliLast); }
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli);
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli);
    virtual void Add(void *const pData);
    virtual void Copy(const MfDblList &list);
    virtual void Copy(const MfDblList &list, MfCallBack &cb);
    virtual void Copy(const MfDblList &list, MfListItemInterface &lii);
    virtual void *GetFirst(MfDblListItem *&pli) const
    {
        pli = pliFirst;
        if (pli)
            return pli->GetData();
        return 0;
    }
    virtual void *GetNext(MfDblListItem *&pli) const
    {
        if (pli) {
            pli = pli->GetNext();
            if (pli)
                return pli->GetData();
        }
        return 0;
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
    int nCount;
};

#endif
