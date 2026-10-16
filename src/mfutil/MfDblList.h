// MfDblList: doubly linked list of void* (mfutil.dll, Motion Factory utility classes).
// Member names from the type records in SteamboatApp.pdb.
#ifndef MFUTIL_MFDBLLIST_H
#define MFUTIL_MFDBLLIST_H

#define MFUTIL_API __declspec(dllexport)

// vtable: ~, CallBack(a,b,c) +4, CallBack(a,b) +8, CallBack(a) +0xc
class MFUTIL_API MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *a) { return 0; }
    virtual int CallBack(void *a, void *b) { return 0; }
    virtual int CallBack(void *a, void *b, void *c) { return 0; }
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

class MFUTIL_API MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    MfDblList() : pliFirst(0), pliLast(0), nCount(0) {}
    virtual ~MfDblList();                                                    // +0x00
    int GetCount() const { return nCount; }
    virtual void Prepend(void *const pData);                                 // +0x04
    virtual void Append(void *const pData);                                  // +0x08
    virtual void Insert(void *pData, EnumListPosition pos, MfDblListItem *pli); // +0x0c
    virtual void Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli); // +0x10
    virtual void Add(void *const pData);                                     // +0x14
    virtual void Copy(const MfDblList &list);                                // +0x20
    virtual void Copy(const MfDblList &list, MfCallBack &cb);                // +0x1c
    virtual void Copy(const MfDblList &list, MfListItemInterface &lii);      // +0x18
    virtual void *GetFirst(MfDblListItem *&pli) const                        // +0x24
    {
        pli = pliFirst;
        return pli ? pli->GetData() : 0;
    }
    virtual void *GetNext(MfDblListItem *&pli) const                         // +0x28
    {
        if (pli)
            pli = pli->GetNext();
        return pli ? pli->GetData() : 0;
    }
    virtual void *GetCurrent(MfDblListItem *&pli) const                      // +0x2c
    {
        return pli ? pli->GetData() : 0;
    }
    virtual void *GetPrev(MfDblListItem *&pli) const;                        // +0x30
    virtual void *GetLast(MfDblListItem *&pli) const;                        // +0x34
    virtual void Iterate(MfCallBack &cb) const;                              // +0x3c
    virtual void Iterate(MfListItemInterface &lii) const;                    // +0x38
    virtual void *DeleteFirst();                                             // +0x40
    virtual void *DeleteLast();                                              // +0x44
    virtual void *Delete(MfDblListItem *&pli);                               // +0x48
    virtual void Clear();                                                    // +0x54
    virtual void Clear(MfCallBack &cb);                                      // +0x50
    virtual void Clear(MfListItemInterface &lii);                            // +0x4c
    virtual MfDblListItem *Find(void *pData) const;                          // +0x6c
    virtual MfDblListItem *Find(void *pData, MfCallBack &cb) const;          // +0x68
    virtual MfDblListItem *Find(void *pData, MfListItemInterface &lii) const; // +0x64
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n) const; // +0x60
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const; // +0x5c
    virtual MfDblListItem *Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const; // +0x58

private:
    MfDblList(const MfDblList &);
    MfDblList &operator=(const MfDblList &);
    MfDblListItem *GetListPosition(MfDblListItem *pli, int n) const;
protected:

    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

#endif
