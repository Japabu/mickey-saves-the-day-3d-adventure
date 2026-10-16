// PORT: declared with the real member types of mfutil's MfHashTable (two of them are pointers), so the size agrees on 64-bit.
#pragma once
#include "enums.h"
template <class T0> class field_;
template <class T0> class unknown_;
class MfCallBack;

class __declspec(dllimport) MfHashTable {
public:
    class MfChainNode;
    virtual ~MfHashTable();
    virtual MfHTErrorCode Search(void *, void * *, int *, int *, MfChainNode * *) = 0;
    virtual void DeleteDataItem(void *, MfCallBack *) = 0;
    virtual void * GetKey(void *) = 0;
    virtual void * GetData(void *) = 0;
    virtual void _slot5();
    MfHashTable & operator=(MfHashTable const &);
    MfHashTable(MfHashTable const &);
    MfHashTable(int, int);
    void Iterate(MfCallBack &);
protected:
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
public:
    // PORT: the member types of mfutil's MfHashTable (src/mfutil/MfHashTable_unit.h), so
    // MfDictionary is as big as mfutil's constructor expects (32 bytes on 64-bit, not 24)
    int field_4;                    // nNumberOfBuckets
    int field_8;                    // nBucketSize
    void **field_c;                 // apBuckets
    MfChainNode **field_10;         // apmfcnChain
};
