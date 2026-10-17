// MfIndex: hash table of (unsigned long key, void *data) entries, derived from
// MfHashTable (mfutil.dll). Layouts from the code.
#ifndef MFUTIL_MFINDEX_H
#define MFUTIL_MFINDEX_H

#include "mfutil_api.h"
#include "MfCallBack.h"
#include "MfHashTable.h"

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
