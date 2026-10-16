// Motion Factory utility library (mfutil.dll): MfStringList (0x1000fa10-0x1000fd70).
#include <stdlib.h>
#include "MfStringList.h"

class MfStringList::MfStringListSearchCB : public MfCallBack {
public:
    virtual int CallBack(void *a, void *b);
};

class MfStringList::MfStringListDeletorCB : public MfCallBack {
public:
    virtual int CallBack(void *a);
};

// 0x1000fa10 ?Add@MfStringList@@UAEPAVMfString@@ABV2@@Z
MfString *MfStringList::Add(const MfString &s)
{
    MfString *p = new MfString(s);
    Append(p);
    return p;
}

// 0x1000fa80 ?Delete@MfStringList@@UAEXABVMfString@@@Z
void MfStringList::Delete(const MfString &s)
{
    MfDblListItem *pli = Search(s);
    MfDblList::Delete(pli);
}

// 0x1000fab0 ?DeleteFirst@MfStringList@@UAEPAXXZ
void *MfStringList::DeleteFirst()
{
    delete (MfString *)MfDblList::DeleteFirst();
    return 0;
}

// ?DeleteLast@MfStringList@@UAEPAXXZ: folded with DeleteFirst by the linker, so
// the original calls MfDblList::DeleteFirst here too.
void *MfStringList::DeleteLast()
{
    delete (MfString *)MfDblList::DeleteFirst();
    return 0;
}

// 0x1000fad0 ?Delete@MfStringList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfStringList::Delete(MfDblListItem *&pli)
{
    delete (MfString *)MfDblList::Delete(pli);
    return 0;
}

// 0x1000fb00 ?Search@MfStringList@@UBEPAVMfDblListItem@@ABVMfString@@@Z
MfDblListItem *MfStringList::Search(const MfString &s) const
{
    MfString str(s);
    MfStringListSearchCB cb;
    return Find(&str, cb);
}

// 0x1000fb80 ?Clear@MfStringList@@UAEXXZ
void MfStringList::Clear()
{
    MfStringListDeletorCB cb;
    MfDblList::Clear(cb);
}

// 0x1000fbc0 ?Sort@MfStringList@@QAEXH@Z
void MfStringList::Sort(int bCaseSensitive)
{
    MfString *pArray = new MfString[GetCount()];
    int n = GetCount();
    int i = 0;
    MfDblListItem *pli;
    for (MfString *p = First(pli); p; p = Next(pli))
        pArray[i++] = *p;
    qsort(pArray, n, sizeof(MfString), bCaseSensitive ? StringCompare : StringCaseCompare);
    Clear();
    for (i = 0; i < n; i++)
        Add(pArray[i]);
    delete[] pArray;
}

// 0x1000fce0 ?StringCaseCompare@MfStringList@@CAHPBX0@Z
int MfStringList::StringCaseCompare(const void *a, const void *b)
{
    return ((const MfString *)a)->CaseCompare(*(const MfString *)b);
}

// 0x1000fcf0 ?StringCompare@MfStringList@@CAHPBX0@Z
int MfStringList::StringCompare(const void *a, const void *b)
{
    return ((const MfString *)a)->Compare(*(const MfString *)b);
}

// 0x1000fd00 ?CallBack@MfStringList@@EAEHABVMfString@@AAV2@@Z
int MfStringList::CallBack(const MfString &token, MfString &rest)
{
    Add(token);
    return 1;
}

// 0x1000fd20 ?CallBack@MfStringListDeletorCB@MfStringList@@UAEHPAX@Z
int MfStringList::MfStringListDeletorCB::CallBack(void *a)
{
    delete (MfString *)a;
    return 1;
}

// 0x1000fd50 ?CallBack@MfStringListSearchCB@MfStringList@@UAEHPAX0@Z
int MfStringList::MfStringListSearchCB::CallBack(void *a, void *b)
{
    return *(MfString *)a == *(MfString *)b ? 1 : 0;
}


// Also matched by this code:
// plugins/psccontentinfo.dll 0x10003820 ?CallBack@MfStringListDeletorCB@MfStringList@@UAEHPAX@Z

// Inline or compiler-generated members this object emits:
// 0x10005d60 ??0MfStringList@@QAE@XZ
