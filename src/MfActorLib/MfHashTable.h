#pragma once
// MfHashTable, MfDictionary (a hash table keyed by MfString) and MfIndex (keyed by
// unsigned long), imported from mfutil.dll (see mfutil/MfHashTable_unit.h, ...).
#include "enums.h"
class MfCallBack;
class MfString;

class __declspec(dllimport) MfHashTable {
    friend class MfPropertyTable;   // ClearItems
    friend class MfProtoDB;
public:
    class MfChainNode;
    MfHashTable(int nBuckets, int nSlots);
    virtual ~MfHashTable();
    // Calls cb.CallBack(key, data, 0) for every item.
    void Iterate(MfCallBack &cb);
protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfChainNode **ppNode) = 0;
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb) = 0;
    virtual void *GetKey(void *pItem) = 0;
    virtual void *GetData(void *pItem) = 0;
    MfHTErrorCode AddItem(void *pKey, void **ppItem);
    MfHTErrorCode DeleteItem(void *pKey, void **ppItem);
    MfHTErrorCode FindItem(void *pKey, void **ppItem);
    MfHTErrorCode ReplaceItem(void *pKey, void **ppItem);
    void Clear(int nBuckets);
    // Empties the table, calling cb.CallBack(data) for every item.
    void ClearItems(MfCallBack *cb);
private:
    int nBucketCount;       // +4
    int nBucketSize;        // +8
    void *pBuckets;         // +0xc
    int nItemCount;         // +0x10
};

class __declspec(dllimport) MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets);
    virtual ~MfDictionary();
    void *Add(const MfString &key, void *data);
    void *Delete(const MfString &key);
    void *Find(const MfString &key);
    void *Replace(const MfString &key, void *data);
    void Clear(MfCallBack *cb);
protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfHashTable::MfChainNode **ppNode);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb);
    virtual void *GetKey(void *pItem);
    virtual void *GetData(void *pItem);
};

// Keyed by unsigned long.
class __declspec(dllimport) MfIndex : public MfHashTable {
public:
    MfIndex(int nBuckets = 53);
    virtual ~MfIndex();
    void *Add(unsigned long key, void *data);
    void *Replace(unsigned long key, void *data);
    void *Delete(unsigned long key);
    void *Find(unsigned long key);
protected:
    virtual MfHTErrorCode Search(void *pKey, void **ppItem, int *piBucket, int *piSlot,
                                 MfHashTable::MfChainNode **ppNode);
    virtual void DeleteDataItem(void *pItem, MfCallBack *cb);
    virtual void *GetKey(void *pItem);
    virtual void *GetData(void *pItem);
};
