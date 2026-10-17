// MfDictionary (mfutil.dll 0x100029e0-0x10002f00)
// The first functions are inline/implicit members of the exported classes that this
// object emitted (no source of their own here; they come from MfDictionary.h):
#include <stdlib.h>
#include "MfDictionary.h"

// 0x100029e0 ?BucketIndex@MfHashTable@@IAEHHH@Z           (inline, MfDictionary.h)
// 0x10002a00 ??0MfHashTable@@QAE@ABV0@@Z                   (implicit)
// 0x10002a30 ??_FMfHashTable@@QAEXXZ                       (default ctor closure)
// 0x10002a40 ??_EMfHashTable@@UAEPAXI@Z                    (vector deleting dtor)
// 0x10002a90 ?Clear@MfDictionary@@QAEXPAVMfCallBack@@@Z    (inline, MfDictionary.h)
// 0x10002aa0 ??0MfDictionary@@QAE@ABV0@@Z                  (implicit)
// 0x10002ad0 ??4MfDictionary@@QAEAAV0@ABV0@@Z              (implicit)
// 0x10002b00 ??_FMfDictionary@@QAEXXZ                      (default ctor closure)
// 0x10002b10 ??_EMfDictionary@@UAEPAXI@Z                   (vector deleting dtor)

// 0x10002b60 ??0MfDictionary@@QAE@H@Z
MfDictionary::MfDictionary(int nBuckets)
    : MfHashTable(nBuckets, 4)
{
}

// 0x10002b80 ??1MfDictionary@@UAE@XZ
MfDictionary::~MfDictionary()
{
    Clear();
}

// 0x10002be0 ?Add@MfDictionary@@QAEPAXABVMfString@@PAX@Z
void *MfDictionary::Add(const MfString &key, void *pData)
{
    void *pResult = 0;
    MfDictionaryEntry *pEntry = new MfDictionaryEntry(key, pData);
    void *pOld = pEntry;
    MfHTErrorCode ec = AddItem(pEntry, &pOld);
    if (ec != MFHT_EC_NO_ERROR) {
        delete pEntry;
        pResult = ((MfDictionaryEntry *)pOld)->GetData();
    }
    return pResult;
}

// 0x10002c80 ?Replace@MfDictionary@@QAEPAXABVMfString@@PAX@Z
void *MfDictionary::Replace(const MfString &key, void *pData)
{
    void *pResult = 0;
    MfDictionaryEntry *pEntry = new MfDictionaryEntry(key, pData);
    MfDictionaryEntry *pOld = pEntry;
    ReplaceItem(pEntry, (void **)&pOld);
    if (pOld) {
        pResult = pOld->GetData();
        delete pOld;
    }
    return pResult;
}

// 0x10002d20 ?Delete@MfDictionary@@QAEPAXABVMfString@@@Z
// 0x10002d60 ??1MfDictionaryEntry@MfDictionary@@QAE@XZ (inline, MfDictionary.h; emitted
//   out of line as `jmp ??1MfString`, called from Delete and DeleteDataItem)
void *MfDictionary::Delete(const MfString &key)
{
    void *pResult = 0;
    MfDictionaryEntry *pOld;
    if (DeleteItem((void *)&key, (void **)&pOld) == MFHT_EC_NO_ERROR) {
        pResult = pOld->GetData();
        delete pOld;
    }
    return pResult;
}

// 0x10002d70 ?Find@MfDictionary@@QAEPAXABVMfString@@@Z
void *MfDictionary::Find(const MfString &key)
{
    void *pResult = 0;
    MfDictionaryEntry *pItem;
    if (FindItem((void *)&key, (void **)&pItem) == MFHT_EC_NO_ERROR)
        pResult = pItem->GetData();
    return pResult;
}

// 0x10002da0 ?DeleteDataItem@MfDictionary@@MAEXPAXPAVMfCallBack@@@Z
void MfDictionary::DeleteDataItem(void *pItem, MfCallBack *cb)
{
    MfDictionaryEntry *pEntry = (MfDictionaryEntry *)pItem;
    if (cb)
        cb->CallBack(pEntry->GetData());
    delete pEntry;
}

// 0x10002dd0 ?GetKey@MfDictionary@@MAEPAXPAX@Z
void *MfDictionary::GetKey(void *pItem)
{
    return ((MfDictionaryEntry *)pItem)->GetKeyPointer();
}

// 0x10002de0 ?GetData@MfDictionary@@MAEPAXPAX@Z
void *MfDictionary::GetData(void *pItem)
{
    return ((MfDictionaryEntry *)pItem)->GetData();
}

// 0x10002df0 ?Search@MfDictionary@@MAE?AW4MfHTErrorCode@@PAXPAPAXPAH2PAPAVMfChainNode@MfHashTable@@@Z
MfHTErrorCode MfDictionary::Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                   MfChainNode **ppPrev)
{
    *ppItem = 0;
    *piBucket = ((MfString *)pKey)->Hash(nNumberOfBuckets);
    *piSlot = 0;
    void **ppBucket = &apBuckets[BucketIndex(*piBucket, 0)];
    if (*ppBucket == 0)
        return MFHT_EC_NOT_IN_TABLE;
    void *pItem;
    for (*piSlot = 0; (pItem = *ppBucket) != 0 && *piSlot < nBucketSize; ppBucket++, (*piSlot)++) {
        if (*(MfString *)pItem == *(MfString *)pKey) {
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
        if (*(MfString *)pItem == *(MfString *)pKey) {
            *ppItem = pItem;
            return MFHT_EC_ALREADY_IN_TABLE;
        }
        *ppPrev = pNode;
    }
    return MFHT_EC_NOT_IN_TABLE;
}
