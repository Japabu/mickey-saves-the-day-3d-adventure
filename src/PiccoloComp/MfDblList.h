#pragma once
class MfCallBack;
class MfListItemInterface;

#include "../mfutil/MfDblListItem.h"

// mfutil.dll: doubly linked list of void *. The constructor is inline here.
// VC5 lays overloaded virtuals out in reverse order of declaration.
class __declspec(dllimport) MfDblList {
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
    virtual void *GetFirst(MfDblListItem *&pli) const;                       // +0x24
    virtual void *GetNext(MfDblListItem *&pli) const;                        // +0x28
    virtual void *GetCurrent(MfDblListItem *&pli) const;                     // +0x2c
    virtual void *GetPrev(MfDblListItem *&pli) const;                        // +0x30
    virtual void *GetLast(MfDblListItem *&pli) const;                        // +0x34
    // Non-virtual inline iteration. These are not exported by mfutil.dll: the
    // header PiccoloComp was built with has them inline only.
    void *First(MfDblListItem *&pli) const
    {
        void *p;
        pli = pliFirst;
        if (pli)
            p = pli->GetData();
        else
            p = 0;
        return p;
    }
    void *Next(MfDblListItem *&pli) const
    {
        void *p;
        if (pli)
            pli = pli->GetNext();
        if (pli)
            p = pli->GetData();
        else
            p = 0;
        return p;
    }
    void *Last(MfDblListItem *&pli) const
    {
        void *p;
        pli = pliLast;
        if (pli)
            p = pli->GetData();
        else
            p = 0;
        return p;
    }
    void *Prev(MfDblListItem *&pli) const
    {
        void *p;
        if (pli)
            pli = pli->GetPrev();
        if (pli)
            p = pli->GetData();
        else
            p = 0;
        return p;
    }
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
    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc

private:
    MfDblList(const MfDblList &);
    MfDblList &operator=(const MfDblList &);
    MfDblListItem *GetListPosition(MfDblListItem *pli, int n) const;
};
