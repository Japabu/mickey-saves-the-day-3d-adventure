// MfIntList: MfDblList of ints (mfutil.dll, 0x100049a0-0x10004a40).
// Reconstructed (no PDB): signatures from the export names. MfDblList declaration
// duplicated from MfDblList.h, with the base constructor inline (??0MfIntList
// has it inlined).
#ifndef MFUTIL_MFINTLIST_H
#define MFUTIL_MFINTLIST_H

#define MFUTIL_API __declspec(dllexport)

// vtable: ~, CallBack(a,b,c) +4, CallBack(a,b) +8, CallBack(a) +0xc
class MFUTIL_API MfCallBack {
public:
    MfCallBack();
    virtual ~MfCallBack();
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

class MFUTIL_API MfDblList {
public:
    enum EnumListPosition { LP_BEFORE = 0, LP_AFTER = 1 };

    // inline here: ??0MfSortedList (0x1000bc90) has the base constructor inlined
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
    virtual void *GetFirst(MfDblListItem *&pli) const;                       // +0x24
    virtual void *GetNext(MfDblListItem *&pli) const;                        // +0x28
    virtual void *GetCurrent(MfDblListItem *&pli) const;                     // +0x2c
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

protected:
    MfDblList(const MfDblList &);
    MfDblList &operator=(const MfDblList &);
    MfDblListItem *GetListPosition(MfDblListItem *pli, int n) const;

    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

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
