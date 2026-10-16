// MfDictionary (mfutil.dll): a hash table keyed by MfString.
#pragma once
#include "MfHashTable.h"
class MfCallBack;
class MfString;

class __declspec(dllimport) MfDictionary : public MfHashTable {
public:
    MfDictionary(int nBuckets = 0x35);
    virtual ~MfDictionary();
    void *Add(const MfString &, void *);
    void *Delete(const MfString &);
    void *Find(const MfString &);
    void *Replace(const MfString &, void *);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }
protected:
    virtual MfHTErrorCode Search(void *, void **, int *, int *, MfHashTable::MfChainNode **);
    virtual void DeleteDataItem(void *, MfCallBack *);
    virtual void *GetKey(void *);
    virtual void *GetData(void *);
};
