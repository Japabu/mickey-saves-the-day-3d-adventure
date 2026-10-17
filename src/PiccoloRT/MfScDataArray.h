// MfScDataArray: the script array (with its implementation, MfScDataArrayImp).
#pragma once
#ifndef PICCOLORT_API
#define PICCOLORT_API __declspec(dllimport)
#endif
#include "MfScDataObject.h"

// Script array (exported interface).
class PICCOLORT_API MfScDataArray : public MfScDataObject {
public:
    virtual ~MfScDataArray();
    virtual MfPropertyErrorCode GetProperty(MfString const &name, MfScDataHandle **value);
    virtual MfPropertyErrorCode SetProperty(MfScDataHandle *self, MfString const &name,
                                            MfScDataHandle *value, int create);
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef) = 0;
    virtual int GetAt(int index, MfScDataHandle **value, int addRef) = 0;
    virtual int IsValid() = 0;
    int Size() const { return m_size; }
protected:
    MfScDataArray(int size);
    virtual MfString GetClassNameA();   // "Array"
    int m_size; // +0x14
};

#include "MfScDataArrayImp.h"
