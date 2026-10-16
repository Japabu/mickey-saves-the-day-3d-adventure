// Motion Factory utility library (mfutil.dll): MfStrStack, a LIFO stack of heap
// MfStrings on top of MfDblList. Reconstructed declarations (no PDB); MfString,
// MfCallBack and MfDblList duplicated from MfStringList.h.
#ifndef MFUTIL_MFSTRSTACK_H
#define MFUTIL_MFSTRSTACK_H

#include <stdlib.h>

#define MFAPI __declspec(dllexport)

class MfListItemInterface;

class MFAPI MfString {
public:
    MfString();
    MfString(const MfString &s);
    ~MfString();
    MfString &operator=(const MfString &s);
    int operator==(const MfString &s) const;
    int CaseCompare(const MfString &s) const;
    int Compare(const MfString &s) const;
private:
    void *m_ref;
};

// vtable: ~, CallBack(a,b,c) +4, CallBack(a,b) +8, CallBack(a) +0xc
class MFAPI MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *p1) { return 0; }
    virtual int CallBack(void *p1, void *p2) { return 0; }
    virtual int CallBack(void *p1, void *p2, void *p3) { return 0; }
};

// vtable 0x100172f4: ??_E, CallBack
class MFAPI MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

class MfDblListItem {
public:
    void *GetData() const { return pData; }
    MfDblListItem *GetNext() const { return pliNext; }

    void *pData;            // +0
    MfDblListItem *pliPrev; // +4
    MfDblListItem *pliNext; // +8
};

// See MfDblList.h. GetFirst/GetNext are inline here: Sort inlines them.
class MFAPI MfDblList {
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

private:
    MfDblList(const MfDblList &);
    MfDblList &operator=(const MfDblList &);

    MfDblListItem *pliFirst; // +4
    MfDblListItem *pliLast;  // +8
    int nCount;              // +0xc
};

// Size 0x10. vtable 0x100178a4; Clear() overrides MfDblList::Clear (+0x54).
class MFAPI MfStrStack : public MfDblList {
public:
    // Clear: CallBack(a) deletes the MfString (vtable 0x1001791c)
    class MfStrStackDeletorCB;

    MfStrStack() {}
    virtual ~MfStrStack();
    void Push(const MfString &s);
    MfString Pop();
    virtual void Clear();
};

#endif
