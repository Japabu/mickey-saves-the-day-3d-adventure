#pragma once
#include "enums.h"
class MfCallBack;

// mfutil.dll. No user-declared copy constructor: PiccoloRT builds the
// implicit one inline (it stores the imported ??_7MfHashTable@@6B@).
class __declspec(dllimport) MfHashTable {
public:
    class MfChainNode;
    MfHashTable(int, int);
    virtual ~MfHashTable();
    void Iterate(MfCallBack &);
protected:
    virtual MfHTErrorCode Search(void *, void * *, int *, int *, MfChainNode * *) = 0;
    virtual void DeleteDataItem(void *, MfCallBack *) = 0;
    virtual void * GetKey(void *) = 0;
    virtual void * GetData(void *) = 0;
    MfHTErrorCode AddItem(void *, void * *);
    MfHTErrorCode DeleteItem(void *, void * *);
    MfHTErrorCode FindItem(void *, void * *);
    MfHTErrorCode ReplaceItem(void *, void * *);
    int BucketIndex(int, int);
    void Clear(int);
    void ClearItems(MfCallBack *);
private:
    static int __cdecl SearchForClosestPrime(int);
    static int * aiPrimes;
    void **apBuckets;           // +0x04
    int nNumberOfBuckets;       // +0x08
    int nBucketSize;            // +0x0c
    MfChainNode **apmfcnChain;  // +0x10
};
