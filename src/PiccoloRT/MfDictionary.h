#pragma once
#include "MfHashTable.h"
class MfCallBack;
class MfString;

// mfutil.dll: hash table keyed by MfString.
class __declspec(dllimport) MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets = 0x35);
    virtual ~MfDictionary();
    void * Add(MfString const &, void *);
    void * Delete(MfString const &);
    void * Find(MfString const &);
    void * Replace(MfString const &, void *);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }
protected:
    virtual MfHTErrorCode Search(void *, void * *, int *, int *, MfHashTable::MfChainNode * *);
    virtual void DeleteDataItem(void *, MfCallBack *);
    virtual void * GetKey(void *);
    virtual void * GetData(void *);
};
