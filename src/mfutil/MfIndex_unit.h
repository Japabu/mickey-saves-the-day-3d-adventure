// MfIndex: hash table of (unsigned long key, void *data) entries, derived from
// MfHashTable (mfutil.dll). Layouts from the code; MfHashTable as in MfDictionary.h.
#ifndef MFUTIL_MFINDEX_H
#define MFUTIL_MFINDEX_H

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

enum MfHTErrorCode {
    MFHT_EC_NO_ERROR,
    MFHT_EC_ALREADY_IN_TABLE,
    MFHT_EC_NOT_IN_TABLE
};

// size 0x14; vtable 0x10017384: ??_E, Search, DeleteDataItem, GetKey, GetData
class MFUTIL_API MfHashTable {
public:
    class MfChainNode {
    public:
        MfChainNode *GetNext() { return pmfcnNext; }
        void SetNext(MfChainNode *p) { pmfcnNext = p; }
        void *GetData() { return pData; }
        void SetData(void *p) { pData = p; }
        void *pData;             // +0
        MfChainNode *pmfcnNext;  // +4
    };

    MfHashTable(int nBuckets = 0x35, int nSize = 4);
    virtual ~MfHashTable();
    void Iterate(MfCallBack *cb);

protected:
    MfHTErrorCode AddItem(void *pItem, void **ppOld);
    MfHTErrorCode ReplaceItem(void *pItem, void **ppOld);
    MfHTErrorCode DeleteItem(void *pKey, void **ppOld);
    MfHTErrorCode FindItem(void *pKey, void **ppItem);
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev) = 0;
    int BucketIndex(int iBucket, int iSlot) { return nBucketSize * iBucket + iSlot; }
    void ClearItems(MfCallBack *cb);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb) = 0;
    virtual void *GetKey(void *pItem) = 0;
    virtual void *GetData(void *pItem) = 0;

    int nNumberOfBuckets;        // +4
    int nBucketSize;             // +8
    void **apBuckets;            // +0xc
    MfChainNode **apmfcnChain;   // +0x10
};

// size 0x14; vtable 0x10017474
class MFUTIL_API MfIndex : public MfHashTable {
public:
    MfIndex(int nBuckets = 0x35);
    virtual ~MfIndex();
    void *Add(unsigned long key, void *pData);
    void *Replace(unsigned long key, void *pData);
    void *Delete(unsigned long key);
    void *Find(unsigned long key);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }

protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev);

    // size 8
    class MfIndexEntry {
    public:
        MfIndexEntry(unsigned long key, void *p) : ulKey(key), pData(p) {}
        unsigned long GetKey() { return ulKey; }
        unsigned long *GetKeyPointer() { return &ulKey; }
        void *GetData() { return pData; }
        unsigned long ulKey;  // +0
        void *pData;          // +4
    };

    virtual void DeleteDataItem(void *pItem, MfCallBack *cb);
    virtual void *GetKey(void *pItem);
    virtual void *GetData(void *pItem);
};

#endif
