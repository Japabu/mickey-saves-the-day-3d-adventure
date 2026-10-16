#pragma once
#include "enums.h"
class MfCallBack;

class __declspec(dllimport) MfHashTable {
public:
    class MfChainNode;
    virtual ~MfHashTable();
protected:
    virtual MfHTErrorCode Search(void *, void * *, int *, int *, MfChainNode * *) = 0;
    virtual void DeleteDataItem(void *, MfCallBack *) = 0;
    virtual void * GetKey(void *) = 0;
    virtual void * GetData(void *) = 0;
public:
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
    int field_4;
    int field_8;
    int field_c;
    int field_10;
};
