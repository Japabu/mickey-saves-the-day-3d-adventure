// MfDictionary: hash table of (MfString key, void *data) entries, derived from
// MfHashTable (mfutil.dll). Member names from the type records in SteamboatApp.pdb;
// layouts from the code. MfHashTable's inline members (BucketIndex, the implicit
// copy constructor/operator=, default ctor closure, ??_E) are emitted in this object
// (MfDictionary.cpp).
#ifndef MFUTIL_MFDICTIONARY_H
#define MFUTIL_MFDICTIONARY_H

#ifndef MFUTIL_API
#define MFUTIL_API __declspec(dllimport)
#endif
#include "MfCallBack.h"
#include "MfString.h"
#include "MfHashTable.h"

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
