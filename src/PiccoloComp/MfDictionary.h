#pragma once
#include "MfHashTable.h"
#include "enums.h"
class MfCallBack;
class MfString;

class __declspec(dllimport) MfDictionary : public MfHashTable {   // own copy: src/mfutil/MfDictionary.h includes src/mfutil/MfString.h, which conflicts with the older MfString kept here
public:
    virtual ~MfDictionary();
protected:
    virtual MfHTErrorCode Search(void *, void * *, int *, int *, MfHashTable::MfChainNode * *);
    virtual void DeleteDataItem(void *, MfCallBack *);
    virtual void * GetKey(void *);
    virtual void * GetData(void *);
public:
    MfDictionary & operator=(MfDictionary const &);
    MfDictionary(MfDictionary const &);
    MfDictionary(int);
    void * Add(MfString const &, void *);
    void * Delete(MfString const &);
    void * Find(MfString const &);
    void * Replace(MfString const &, void *);
    void Clear(MfCallBack *);
};
