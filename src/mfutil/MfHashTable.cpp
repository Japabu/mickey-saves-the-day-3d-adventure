// MfHashTable (mfutil.dll 0x10003c20-0x10004240)
#include <string.h>
#include "MfHashTable.h"

int MfHashTable::aiPrimes[50] = {
    53, 101, 151, 211, 251, 307, 353, 401, 457, 503,
    557, 601, 653, 701, 751, 809, 853, 907, 953, 1009,
    1051, 1103, 1151, 1201, 1259, 1301, 1361, 1409, 1451, 1511,
    1553, 1601, 1657, 1709, 1753, 1801, 1861, 1901, 1951, 2003,
    2053, 2111, 2153, 2203, 2251, 2309, 2351, 2411, 2459, 2503
};

// 0x10003c20 ??0MfHashTable@@QAE@HH@Z
MfHashTable::MfHashTable(int nBuckets, int nSize)
    : nBucketSize(nSize)
{
    nNumberOfBuckets = SearchForClosestPrime(nBuckets);
    apBuckets = (void **)operator new(nBucketSize * nNumberOfBuckets * sizeof(void *));
    apmfcnChain = (MfChainNode **)operator new(nNumberOfBuckets * sizeof(MfChainNode *));
    memset(apBuckets, 0, nNumberOfBuckets * nBucketSize * sizeof(void *));
    memset(apmfcnChain, 0, nNumberOfBuckets * sizeof(MfChainNode *));
}

// 0x10003cb0 ?SearchForClosestPrime@MfHashTable@@CAHH@Z
int MfHashTable::SearchForClosestPrime(int n)
{
    if (n >= aiPrimes[49])
        return n;
    int iLow = 0;
    int iMid = 25;
    int iHigh = 50;
    int bFound = 0;
    int iPrime;
    do {
        iPrime = aiPrimes[iMid];
        if (iMid > 0 && iMid < 50) {
            int bBelow = n <= aiPrimes[iMid - 1];
            int bAbove = n > iPrime;
            if (bBelow || bAbove) {
                if (bBelow) {
                    iHigh = iMid;
                    iMid = (iMid + iLow) / 2;
                } else if (bAbove) {
                    iLow = iMid;
                    iMid = (iMid + iHigh) / 2;
                }
            } else
                bFound = 1;
        } else
            bFound = 1;
    } while (!bFound);
    return iPrime;
}

// 0x10003d40 ??1MfHashTable@@UAE@XZ
MfHashTable::~MfHashTable()
{
    Clear(1);
    operator delete(apBuckets);
    operator delete(apmfcnChain);
}

// 0x10003d70 ?AddItem@MfHashTable@@IAE?AW4MfHTErrorCode@@PAXPAPAX@Z
MfHTErrorCode MfHashTable::AddItem(void *pItem, void **ppOld)
{
    void *pFound;
    int iBucket, iSlot;
    MfChainNode *pPrev;
    if (Search(pItem, &pFound, &iBucket, &iSlot, &pPrev) == MFHT_EC_ALREADY_IN_TABLE) {
        *ppOld = pFound;
        return MFHT_EC_ALREADY_IN_TABLE;
    }
    if (iSlot >= 0) {
        apBuckets[BucketIndex(iBucket, iSlot)] = *ppOld;
        return MFHT_EC_NO_ERROR;
    }
    MfChainNode *pNode = new MfChainNode(*ppOld);
    if (pPrev)
        pPrev->SetNext(pNode);
    else
        apmfcnChain[iBucket] = pNode;
    return MFHT_EC_NO_ERROR;
}

// 0x10003e20 ?ReplaceItem@MfHashTable@@IAE?AW4MfHTErrorCode@@PAXPAPAX@Z
MfHTErrorCode MfHashTable::ReplaceItem(void *pItem, void **ppOld)
{
    void *pFound;
    int iBucket, iSlot;
    MfChainNode *pPrev;
    if (Search(pItem, &pFound, &iBucket, &iSlot, &pPrev) == MFHT_EC_ALREADY_IN_TABLE) {
        if (iSlot >= 0)
            apBuckets[BucketIndex(iBucket, iSlot)] = *ppOld;
        else if (pPrev)
            pPrev->GetNext()->SetData(*ppOld);
        else
            apmfcnChain[iBucket]->SetData(*ppOld);
        *ppOld = pFound;
        return MFHT_EC_NO_ERROR;
    }
    if (iSlot >= 0)
        apBuckets[BucketIndex(iBucket, iSlot)] = *ppOld;
    else {
        MfChainNode *pNode = new MfChainNode(*ppOld);
        if (pPrev)
            pPrev->SetNext(pNode);
        else
            apmfcnChain[iBucket] = pNode;
    }
    *ppOld = 0;
    return MFHT_EC_NO_ERROR;
}

// 0x10003f40 ?DeleteItem@MfHashTable@@IAE?AW4MfHTErrorCode@@PAXPAPAX@Z
MfHTErrorCode MfHashTable::DeleteItem(void *pKey, void **ppOld)
{
    int iBucket, iSlot;
    MfChainNode *pPrev;
    MfChainNode *pNode;
    if (Search(pKey, ppOld, &iBucket, &iSlot, &pPrev) == MFHT_EC_NOT_IN_TABLE)
        return MFHT_EC_NOT_IN_TABLE;
    if (iSlot >= 0) {
        void **pp = &apBuckets[BucketIndex(iBucket, iSlot)];
        for (int i = iSlot; i < nBucketSize - 1 && *pp; i++, pp++)
            *pp = pp[1];
        pNode = apmfcnChain[iBucket];
        if (pNode) {
            *pp = pNode->GetData();
            apmfcnChain[iBucket] = pNode->GetNext();
            delete pNode;
        } else
            *pp = 0;
    } else {
        if (pPrev) {
            pNode = pPrev->GetNext();
            pPrev->SetNext(pNode->GetNext());
        } else {
            pNode = apmfcnChain[iBucket];
            apmfcnChain[iBucket] = pNode->GetNext();
        }
        delete pNode;
    }
    return MFHT_EC_NO_ERROR;
}

// 0x10004030 ?FindItem@MfHashTable@@IAE?AW4MfHTErrorCode@@PAXPAPAX@Z
MfHTErrorCode MfHashTable::FindItem(void *pKey, void **ppItem)
{
    int iBucket, iSlot;
    MfChainNode *pPrev;
    if (Search(pKey, ppItem, &iBucket, &iSlot, &pPrev) == MFHT_EC_NOT_IN_TABLE)
        return MFHT_EC_NOT_IN_TABLE;
    return MFHT_EC_NO_ERROR;
}

// 0x10004060 ?Iterate@MfHashTable@@QAEXAAVMfCallBack@@@Z
void MfHashTable::Iterate(MfCallBack &cb)
{
    void *pKey, *pData;
    for (int iBucket = 0; iBucket < nNumberOfBuckets; iBucket++) {
        void **pp = &apBuckets[BucketIndex(iBucket, 0)];
        if (*pp) {
            for (int iSlot = 0; *pp && iSlot < nBucketSize; iSlot++) {
                pKey = GetKey(*pp);
                pData = GetData(*pp);
                cb.CallBack(pKey, pData, *pp);
                pp++;
            }
            for (MfChainNode *pNode = apmfcnChain[iBucket]; pNode; pNode = pNode->GetNext()) {
                pKey = GetKey(pNode->GetData());
                pData = GetData(pNode->GetData());
                cb.CallBack(pKey, pData, pNode->GetData());
            }
        }
    }
}

// 0x10004130 ?Clear@MfHashTable@@IAEXH@Z
void MfHashTable::Clear(int bFreeOnly)
{
    if (!bFreeOnly)
        memset(apBuckets, 0, nNumberOfBuckets * nBucketSize * sizeof(void *));
    for (int i = 0; i < nNumberOfBuckets; i++) {
        MfChainNode *pNode = apmfcnChain[i];
        while (pNode) {
            MfChainNode *pNext = pNode->GetNext();
            delete pNode;
            pNode = pNext;
        }
    }
    if (!bFreeOnly)
        memset(apmfcnChain, 0, nNumberOfBuckets * sizeof(MfChainNode *));
}

// Callback that hands every item back to the table's DeleteDataItem.
// vtable 0x10017444: ??_E (0x10004960), CallBack(3), CallBack(2), CallBack(1)
class MfHTDeleteCallBack : public MfCallBack {
public:
    MfHTDeleteCallBack(MfCallBack *cb, MfHashTable *pTable) : pmfcb(cb), pmfht(pTable) {}
    // 0x10004220 ?CallBack@MfHTDeleteCallBack@@UAEHPAX00@Z
    virtual int CallBack(void *pKey, void *pData, void *pItem);
    MfCallBack *pmfcb;       // +4
    MfHashTable *pmfht;      // +8
};

// 0x100041b0 ?ClearItems@MfHashTable@@IAEXPAVMfCallBack@@@Z
void MfHashTable::ClearItems(MfCallBack *cb)
{
    MfHTDeleteCallBack mfcb(cb, this);
    Iterate(mfcb);
    Clear(0);
}

// 0x10004210 ??1MfCallBack@@UAE@XZ  (inline, MfHashTable.h)

// 0x10004220 ?CallBack@MfHTDeleteCallBack@@UAEHPAX00@Z (original (no) export name)
int MfHTDeleteCallBack::CallBack(void *pKey, void *pData, void *pItem)
{
    pmfht->DeleteDataItem(pItem, pmfcb);
    return 1;
}
