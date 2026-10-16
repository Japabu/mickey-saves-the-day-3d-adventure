// MfDblList (mfutil.dll 0x10002100-0x100029e0)
#include "MfDblList.h"

// 0x10002100 ??0MfDblList@@QAE@XZ (inline, MfDblList.h)

// 0x10002170 ?Prepend@MfDblList@@UAEXQAX@Z
void MfDblList::Prepend(void *const pData)
{
    Insert(pData, LP_BEFORE, pliFirst);
}

// 0x10002190 ?Add@MfSortedList@@QAEXPAVMfSortedListItem@@@Z (original ?Append@MfDblList@@UAEXQAX@Z)
void MfDblList::Append(void *const pData)
{
    Insert(pData, LP_AFTER, pliLast);
}

// 0x100021b0 ?Add@MfDblList@@UAEXQAX@Z
void MfDblList::Add(void *const pData)
{
    Append(pData);
}

// 0x100021c0 ?GetLast@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z
void *MfDblList::GetLast(MfDblListItem *&pli) const
{
    pli = pliLast;
    return pli ? pli->pData : 0;
}

// 0x100049a0 ?GetPrev@MfDblList@@UBEPAXAAPAVMfDblListItem@@@Z (folded with MfIntList::Prev and MfStringList::Prev, MfIntList.cpp)
void *MfDblList::GetPrev(MfDblListItem *&pli) const
{
    if (pli)
        pli = pli->GetPrev();
    return pli ? pli->GetData() : 0;
}

// 0x100021e0 ??0MfDblList@@AAE@ABV0@@Z
MfDblList::MfDblList(const MfDblList &)
{
}

// 0x100021f0 ??1MfDblList@@UAE@XZ
// (0x10002120 is its vector deleting destructor ??_EMfDblList@@UAEPAXI@Z)
MfDblList::~MfDblList()
{
    Clear();
}

// 0x10002200 ??4MfDblList@@AAEAAV0@ABV0@@Z
MfDblList &MfDblList::operator=(const MfDblList &)
{
    return *this;
}

// 0x10002210 ?Insert@MfDblList@@UAEXPAXW4EnumListPosition@1@PAVMfDblListItem@@@Z
void MfDblList::Insert(void *pData, EnumListPosition pos, MfDblListItem *pli)
{
    if (pliFirst == 0) {
        pliFirst = pliLast = new MfDblListItem(pData);
        nCount++;
        return;
    }
    if (pli == 0)
        pli = (pos == LP_BEFORE) ? pliFirst : pliLast;
    if (pos == LP_BEFORE) {
        MfDblListItem *pliNew = new MfDblListItem(pData);
        nCount++;
        pliNew->SetLinks(pli->pliPrev, pli);
        if (pli == pliFirst)
            pliFirst = pliNew;
    } else {
        MfDblListItem *pliNew = new MfDblListItem(pData);
        nCount++;
        pliNew->SetLinks(pli, pli->pliNext);
        if (pli == pliLast)
            pliLast = pliNew;
    }
}

// 0x10002310 ?Merge@MfDblList@@UAEXAAV1@W4EnumListPosition@1@PAVMfDblListItem@@@Z
void MfDblList::Merge(MfDblList &list, EnumListPosition pos, MfDblListItem *pli)
{
    if (list.nCount == 0 || this == &list)
        return;
    if (pliFirst == 0) {
        pliFirst = list.pliFirst;
        pliLast = list.pliLast;
    } else if (pli == 0) {
        if (pos == LP_BEFORE) {
            pliFirst->pliPrev = list.pliLast;
            list.pliLast->pliNext = pliFirst;
            pliFirst = list.pliFirst;
        } else {
            pliLast->pliNext = list.pliFirst;
            list.pliFirst->pliPrev = pliLast;
            pliLast = list.pliLast;
        }
    } else if (pos == LP_BEFORE) {
        list.pliLast->pliNext = pli;
        list.pliFirst->pliPrev = pli->pliPrev;
        if (pli->pliPrev)
            pli->pliPrev->pliNext = list.pliFirst;
        else
            pliFirst = list.pliFirst;
        pli->pliPrev = list.pliLast;
    } else {
        list.pliFirst->pliPrev = pli;
        list.pliLast->pliNext = pli->pliNext;
        if (pli->pliNext)
            pli->pliNext->pliPrev = list.pliLast;
        else
            pliLast = list.pliLast;
        pli->pliNext = list.pliFirst;
    }
    nCount += list.nCount;
    list.pliFirst = 0;
    list.pliLast = 0;
    list.nCount = 0;
}

// 0x10002410 ?Copy@MfDblList@@UAEXABV1@@Z
void MfDblList::Copy(const MfDblList &list)
{
    for (MfDblListItem *pli = list.pliFirst; pli; pli = pli->pliNext)
        Add(pli->pData);
}

// 0x10002440 ?Copy@MfDblList@@UAEXABV1@AAVMfCallBack@@@Z
void MfDblList::Copy(const MfDblList &list, MfCallBack &cb)
{
    for (MfDblListItem *pli = list.pliFirst; pli; pli = pli->pliNext) {
        void *pNew;
        cb.CallBack(pli->pData, &pNew);
        Add(pNew);
    }
}

// 0x10002480 ?Copy@MfDblList@@UAEXABV1@AAVMfListItemInterface@@@Z
void MfDblList::Copy(const MfDblList &list, MfListItemInterface &lii)
{
    for (MfDblListItem *pli = list.pliFirst; pli; pli = pli->pliNext) {
        void *pNew = lii.Copy(pli->pData);
        if (pNew)
            Add(pNew);
    }
}

// 0x100024c0 ?Iterate@MfDblList@@UBEXAAVMfCallBack@@@Z
void MfDblList::Iterate(MfCallBack &cb) const
{
    MfDblListItem *pli;
    for (void *p = GetFirst(pli); p; p = GetNext(pli))
        if (!cb.CallBack(p))
            return;
}

// 0x10002500 ?Iterate@MfDblList@@UBEXAAVMfListItemInterface@@@Z
void MfDblList::Iterate(MfListItemInterface &lii) const
{
    MfDblListItem *pli;
    for (void *p = GetFirst(pli); p; p = GetNext(pli))
        if (!lii.Iterate(p))
            return;
}

// 0x10002540 ?DeleteFirst@MfDblList@@UAEPAXXZ
void *MfDblList::DeleteFirst()
{
    void *pData = 0;
    if (pliFirst) {
        pData = pliFirst->pData;
        MfDblListItem *pliNext = pliFirst->pliNext;
        if (pliFirst == pliLast)
            pliLast = 0;
        else
            pliNext->pliPrev = 0;
        delete pliFirst;
        pliFirst = pliNext;
        nCount--;
    }
    return pData;
}

// 0x10002580 ?DeleteLast@MfDblList@@UAEPAXXZ
void *MfDblList::DeleteLast()
{
    void *pData = 0;
    if (pliLast) {
        pData = pliLast->pData;
        MfDblListItem *pliPrev = pliLast->pliPrev;
        if (pliLast == pliFirst)
            pliFirst = 0;
        else
            pliPrev->pliNext = 0;
        delete pliLast;
        pliLast = pliPrev;
        nCount--;
    }
    return pData;
}

// 0x100025c0 ?Delete@MfDblList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfDblList::Delete(MfDblListItem *&pli)
{
    void *pData = 0;
    if (pli) {
        pData = pli->pData;
        if (pli->pliPrev)
            pli->pliPrev->pliNext = pli->pliNext;
        if (pli->pliNext)
            pli->pliNext->pliPrev = pli->pliPrev;
        if (pli == pliFirst)
            pliFirst = pli->pliNext;
        if (pli == pliLast)
            pliLast = pli->pliPrev;
        MfDblListItem *pliNext = pli->pliNext;
        delete pli;
        nCount--;
        pli = pliNext;
    }
    return pData;
}

// 0x10002630 ?Clear@MfDblList@@UAEXXZ
void MfDblList::Clear()
{
    MfDblListItem *pli = 0;
    GetFirst(pli);
    while (pli) {
        MfDblListItem *pliNext = pli->pliNext;
        delete pli;
        pli = pliNext;
    }
    pliLast = 0;
    pliFirst = 0;
    nCount = 0;
}

// 0x10002680 ?Clear@MfDblList@@UAEXAAVMfCallBack@@@Z
void MfDblList::Clear(MfCallBack &cb)
{
    MfDblListItem *pli = 0;
    GetFirst(pli);
    while (pli) {
        MfDblListItem *pliNext = pli->pliNext;
        cb.CallBack(pli->pData);
        delete pli;
        pli = pliNext;
    }
    pliLast = 0;
    pliFirst = 0;
    nCount = 0;
}

// 0x100026e0 ?Clear@MfDblList@@UAEXAAVMfListItemInterface@@@Z
void MfDblList::Clear(MfListItemInterface &lii)
{
    MfDblListItem *pli = 0;
    GetFirst(pli);
    while (pli) {
        MfDblListItem *pliNext = pli->pliNext;
        lii.Destroy(pli->pData);
        delete pli;
        pli = pliNext;
    }
    pliLast = 0;
    pliFirst = 0;
    nCount = 0;
}

// 0x10002740 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAX@Z
MfDblListItem *MfDblList::Find(void *pData) const
{
    return Find(pData, 0, 0);
}

// 0x10002760 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAXAAVMfCallBack@@@Z
MfDblListItem *MfDblList::Find(void *pData, MfCallBack &cb) const
{
    return Find(pData, 0, 0, cb);
}

// 0x10002780 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAXAAVMfListItemInterface@@@Z
MfDblListItem *MfDblList::Find(void *pData, MfListItemInterface &lii) const
{
    return Find(pData, 0, 0, lii);
}

// 0x100027a0 ?GetListPosition@MfDblList@@ABEPAVMfDblListItem@@PAV2@H@Z
inline MfDblListItem *MfDblList::GetListPosition(MfDblListItem *pli, int n) const
{
    MfDblListItem *pliPos = 0;
    switch (n) {
    case 0:
        GetFirst(pliPos);
        break;
    case 1:
        pliPos = pli;
        break;
    case 2:
        pliPos = pli;
        GetNext(pliPos);
        break;
    }
    return pliPos;
}

// 0x10002800 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAXPAV2@H@Z
MfDblListItem *MfDblList::Find(void *pData, MfDblListItem *pli, int n) const
{
    pli = GetListPosition(pli, n);
    while (pli) {
        if (pData == pli->pData)
            return pli;
        GetNext(pli);
    }
    return 0;
}

// 0x10002880 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAXPAV2@HAAVMfCallBack@@@Z
MfDblListItem *MfDblList::Find(void *pData, MfDblListItem *pli, int n, MfCallBack &cb) const
{
    MfDblListItem *pliCur = 0;
    pliCur = GetListPosition(pli, n);
    while (pliCur) {
        if (cb.CallBack(pData, pliCur->pData) == 1)
            return pliCur;
        GetNext(pliCur);
    }
    return 0;
}

// 0x10002930 ?Find@MfDblList@@UBEPAVMfDblListItem@@PAXPAV2@HAAVMfListItemInterface@@@Z
MfDblListItem *MfDblList::Find(void *pData, MfDblListItem *pli, int n, MfListItemInterface &lii) const
{
    MfDblListItem *pliCur = 0;
    pliCur = GetListPosition(pli, n);
    while (pliCur) {
        if (lii.Matches(pData, pliCur->pData) == 1)
            return pliCur;
        GetNext(pliCur);
    }
    return 0;
}


// Matched by this code:
// 0x10002120 ??_EMfDblList@@UAEPAXI@Z