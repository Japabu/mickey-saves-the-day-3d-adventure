// MfHashTable (mfutil.dll 0x10003c20-0x10004240): open hash table with fixed-size
// buckets (apBuckets, nNumberOfBuckets x nBucketSize item pointers) plus an overflow
// chain per bucket (apmfcnChain). Member names as in MfDictionary.h (SteamboatApp.pdb
// type records); layouts from the code.
#ifndef MFUTIL_MFHASHTABLE_H
#define MFUTIL_MFHASHTABLE_H

#define MFUTIL_API __declspec(dllexport)

// vtable 0x100172d4: ??_E, CallBack(3), CallBack(2), CallBack(1)
class MFUTIL_API MfCallBack {
public:
    MfCallBack() {}
    virtual ~MfCallBack() {}
    virtual int CallBack(void *p1) { return 0; }
    virtual int CallBack(void *p1, void *p2) { return 0; }
    virtual int CallBack(void *p1, void *p2, void *p3) { return 0; }
};

enum MfHTErrorCode {
    MFHT_EC_NO_ERROR,
    MFHT_EC_ALREADY_IN_TABLE,
    MFHT_EC_NOT_IN_TABLE
};

class MfHTDeleteCallBack;

// size 0x14; vtable 0x10017384: ??_E, Search, DeleteDataItem, GetKey, GetData
class MFUTIL_API MfHashTable {
public:
    class MfChainNode {
    public:
        MfChainNode(void *p) : pData(p), pmfcnNext(0) {}
        MfChainNode *GetNext() { return pmfcnNext; }
        void SetNext(MfChainNode *p) { pmfcnNext = p; }
        void *GetData() { return pData; }
        void SetData(void *p) { pData = p; }
        void *pData;             // +0
        MfChainNode *pmfcnNext;  // +4
    };

    MfHashTable(int nBuckets = 0x35, int nSize = 4);
    virtual ~MfHashTable();
    void Iterate(MfCallBack &cb);

protected:
    MfHTErrorCode AddItem(void *pItem, void **ppOld);
    MfHTErrorCode ReplaceItem(void *pItem, void **ppOld);
    MfHTErrorCode DeleteItem(void *pKey, void **ppOld);
    MfHTErrorCode FindItem(void *pKey, void **ppItem);
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev) = 0;
    int BucketIndex(int iBucket, int iSlot) { return nBucketSize * iBucket + iSlot; }
    void Clear(int bFreeOnly);
    void ClearItems(MfCallBack *cb);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb) = 0;
    virtual void *GetKey(void *pItem) = 0;
    virtual void *GetData(void *pItem) = 0;

    int nNumberOfBuckets;        // +4
    int nBucketSize;             // +8
    void **apBuckets;            // +0xc
    MfChainNode **apmfcnChain;   // +0x10

private:
    friend class MfHTDeleteCallBack;
    static int SearchForClosestPrime(int n);
    static int aiPrimes[];       // 50 primes, 53..2503 (0x10024208)
};

#endif

// own copy: VC5 compiles MfHashTable::Clear(int) differently with the shared headers
// (MfCallBack.h, MfHashTable.h); MfHashTable.cpp keeps the declarations it was matched with.
