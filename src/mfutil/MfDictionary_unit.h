// MfDictionary: hash table of (MfString key, void *data) entries, derived from
// MfHashTable (mfutil.dll). Member names from the type records in SteamboatApp.pdb;
// layouts from the code. MfHashTable's inline members (BucketIndex, the implicit
// copy constructor/operator=, default ctor closure, ??_E) are emitted in this object.
#ifndef MFUTIL_MFDICTIONARY_H
#define MFUTIL_MFDICTIONARY_H

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

// size 4 (pointer to the shared MfStringRef); only what this unit uses
class MFUTIL_API MfString {
public:
    MfString(const MfString &s);
    ~MfString();
    int operator==(const MfString &s) const;
    unsigned long Hash(int size) const;
private:
    void *m_ref;
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

// vtable 0x1001739c
class MFUTIL_API MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets = 0x35);
    virtual ~MfDictionary();
    void *Add(const MfString &key, void *pData);
    void *Replace(const MfString &key, void *pData);
    void *Delete(const MfString &key);
    void *Find(const MfString &key);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }

protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppPrev);

    class MfDictionaryEntry {
    public:
        MfDictionaryEntry(const MfString &key, void *p) : mfsKey(key), pData(p) {}
        ~MfDictionaryEntry() {}
        MfString &GetKey() { return mfsKey; }
        MfString *GetKeyPointer() { return &mfsKey; }
        void *GetData() { return pData; }
        MfString mfsKey;   // +0
        void *pData;       // +4
    };

    virtual void DeleteDataItem(void *pItem, MfCallBack *cb);
    virtual void *GetKey(void *pItem);
    virtual void *GetData(void *pItem);
};

#endif
