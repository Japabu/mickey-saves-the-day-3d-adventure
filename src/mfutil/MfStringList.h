// MfStringList: a MfDblList of heap MfStrings that is also a
// MfStringTokenizerCallBack (MfString::Tokenize appends every token).
// Reconstructed declarations (no PDB): layouts from the code, signatures from the
// export names, nested callback class names from the RTTI type descriptors.
#ifndef MFUTIL_MFSTRINGLIST_H
#define MFUTIL_MFSTRINGLIST_H

#include "MfString.h"
#include "MfDblList.h"

// vtable 0x100172f4: ??_E, CallBack
class MFAPI MfStringTokenizerCallBack {
public:
    MfStringTokenizerCallBack() {}
    virtual ~MfStringTokenizerCallBack() {}
    virtual int CallBack(const MfString &token, MfString &rest) { return 1; }
};

// Size 0x14: MfDblList at +0, MfStringTokenizerCallBack at +0x10.
// vtable 0x100177f4 (secondary 0x100177e0).
class MFAPI MfStringList : public MfDblList, public MfStringTokenizerCallBack {
public:
    // Defined in MfStringList.cpp (not exported).
    class MfStringListSearchCB;  // Search: CallBack(a, b) compares (vtable 0x10017874)
    class MfStringListDeletorCB; // Clear: CallBack(a) deletes (vtable 0x1001788c)

    MfStringList() {}
    virtual ~MfStringList() { Clear(); }
    MfStringList &operator+(const MfString &s) { Add(s); return *this; }
    MfString *First(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetFirst(pli); }
    MfString *Next(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetNext(pli); }
    MfString *Current(MfDblListItem *&pli) const { return (MfString *)MfDblList::GetCurrent(pli); }
    virtual MfString *Add(const MfString &s);                     // +0x70
    virtual void Delete(const MfString &s);                       // +0x74
    virtual void *DeleteFirst();                                  // +0x40
    virtual void *DeleteLast();                                   // +0x44
    virtual void *Delete(MfDblListItem *&pli);                    // +0x48
    virtual void Clear();                                         // +0x54
    virtual MfDblListItem *Search(const MfString &s) const;       // +0x78
    void Sort(int bCaseSensitive);

private:
    static int StringCaseCompare(const void *a, const void *b);
    static int StringCompare(const void *a, const void *b);
    virtual int CallBack(const MfString &token, MfString &rest);
};

#endif
