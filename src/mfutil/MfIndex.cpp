// MfIndex (mfutil.dll 0x10004410-0x10004760)
// GetKey/GetData of MfIndex are folded with MfDictionary's (0x10002dd0, 0x10002de0).
#include <stdlib.h>
#include "MfIndex.h"

// 0x10004410 ??0MfIndex@@QAE@ABV0@@Z    (implicit copy constructor)
// 0x10004440 ??_FMfIndex@@QAEXXZ        (default ctor closure)
// 0x10004450 ??_EMfIndex@@UAEPAXI@Z     (vector deleting dtor)

// 0x100044a0 ??0MfIndex@@QAE@H@Z
MfIndex::MfIndex(int nBuckets)
    : MfHashTable(nBuckets, 4)
{
}

// 0x100044c0 ??1MfIndex@@UAE@XZ
MfIndex::~MfIndex()
{
    Clear();
}

// 0x10004520 ?Add@MfIndex@@QAEPAXKPAX@Z
void *MfIndex::Add(unsigned long key, void *pData)
{
    void *pResult = 0;
    MfIndexEntry *pEntry = new MfIndexEntry(key, pData);
    void *pOld = pEntry;
    MfHTErrorCode ec = AddItem((void *)&key, &pOld);
    if (ec != MFHT_EC_NO_ERROR) {
        delete pEntry;
        pResult = ((MfIndexEntry *)pOld)->GetData();
    }
    return pResult;
}

// 0x10004580 ?Replace@MfIndex@@QAEPAXKPAX@Z
void *MfIndex::Replace(unsigned long key, void *pData)
{
    void *pResult = 0;
    MfIndexEntry *pEntry = new MfIndexEntry(key, pData);
    MfIndexEntry *pOld = pEntry;
    ReplaceItem((void *)&key, (void **)&pOld);
    if (pOld) {
        pResult = pOld->GetData();
        delete pOld;
    }
    return pResult;
}

// 0x100045e0 ?Delete@MfIndex@@QAEPAXK@Z
void *MfIndex::Delete(unsigned long key)
{
    void *pResult = 0;
    MfIndexEntry *pOld;
    if (DeleteItem((void *)&key, (void **)&pOld) == MFHT_EC_NO_ERROR) {
        pResult = pOld->GetData();
        delete pOld;
    }
    return pResult;
}

// 0x10004610 ?Find@MfIndex@@QAEPAXK@Z
void *MfIndex::Find(unsigned long key)
{
    void *pResult = 0;
    MfIndexEntry *pItem;
    if (FindItem((void *)&key, (void **)&pItem) == MFHT_EC_NO_ERROR)
        pResult = pItem->GetData();
    return pResult;
}

// 0x10004640 ?DeleteDataItem@MfIndex@@MAEXPAXPAVMfCallBack@@@Z
void MfIndex::DeleteDataItem(void *pItem, MfCallBack *cb)
{
    MfIndexEntry *pEntry = (MfIndexEntry *)pItem;
    if (cb)
        cb->CallBack(pEntry->GetData());
    delete pEntry;
}

// 0x10004670 ?Search@MfIndex@@MAE?AW4MfHTErrorCode@@PAXPAPAXPAH2PAPAVMfChainNode@MfHashTable@@@Z
MfHTErrorCode MfIndex::Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                              MfChainNode **ppPrev)
{
    *ppItem = 0;
    unsigned long key = *(unsigned long *)pKey;
    unsigned long nk = ~key;
    *piBucket = ((((((nk << 4) ^ key) << 4) ^ nk) << 4) ^ key) % (unsigned long)nNumberOfBuckets;
    *piSlot = 0;
    void **ppBucket = &apBuckets[BucketIndex(*piBucket, 0)];
    if (*ppBucket == 0)
        return MFHT_EC_NOT_IN_TABLE;
    void *pItem;
    for (*piSlot = 0; (pItem = *ppBucket) != 0 && *piSlot < nBucketSize; ppBucket++, (*piSlot)++) {
        if (((MfIndexEntry *)pItem)->GetKey() == key) {
            *ppPrev = 0;
            *ppItem = pItem;
            return MFHT_EC_ALREADY_IN_TABLE;
        }
    }
    if (*piSlot == nBucketSize)
        *piSlot = -1;
    *ppPrev = 0;
    for (MfChainNode *pNode = apmfcnChain[*piBucket]; pNode; pNode = pNode->GetNext()) {
        pItem = pNode->GetData();
        if (((MfIndexEntry *)pItem)->GetKey() == key) {
            *ppItem = pItem;
            return MFHT_EC_ALREADY_IN_TABLE;
        }
        *ppPrev = pNode;
    }
    return MFHT_EC_NOT_IN_TABLE;
}

// 0x10002dd0 ?GetKey@MfIndex@@MAEPAXPAX@Z (folded with MfDictionary::GetKey)
void *MfIndex::GetKey(void *pItem)
{
    return ((MfIndexEntry *)pItem)->GetKeyPointer();
}

// 0x10002de0 ?GetData@MfIndex@@MAEPAXPAX@Z (folded with MfDictionary::GetData)
void *MfIndex::GetData(void *pItem)
{
    return ((MfIndexEntry *)pItem)->GetData();
}
