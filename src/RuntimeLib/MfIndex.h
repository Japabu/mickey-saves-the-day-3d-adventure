// MfIndex (mfutil.dll): a hash table from unsigned long keys to pointers.
#ifndef RUNTIMELIB_MFINDEX_H
#define RUNTIMELIB_MFINDEX_H
#include "MfHashTable.h"

class __declspec(dllimport) MfIndex : public MfHashTable {
public:
    MfIndex(int size);
    MfIndex(const MfIndex &);
    virtual ~MfIndex();
    MfIndex &operator=(const MfIndex &);
    void *Add(unsigned long key, void *data);
    void *Find(unsigned long key);
    void *Delete(unsigned long key);
    void *Replace(unsigned long key, void *data);
    void Clear(MfCallBack *cb = 0) { ClearItems(cb); }
protected:
    virtual MfHTErrorCode Search(void *, void **, int *, int *, MfChainNode **);
    virtual void DeleteDataItem(void *, MfCallBack *);
    virtual void *GetKey(void *);
    virtual void *GetData(void *);
};

#endif
