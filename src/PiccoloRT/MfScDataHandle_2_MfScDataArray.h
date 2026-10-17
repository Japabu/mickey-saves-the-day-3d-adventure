// own copy: VC5 compiles MfScDataHandle::GarbageCollectContainer differently with the shared
// headers; MfScDataArray.h as MfScDataHandle_2.cpp saw it at cab9731.
#pragma once
#include "MfScDataObject.h"

// Script array (exported interface).
class __declspec(dllexport) MfScDataArray : public MfScDataObject {
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
    int m_size; // +0x14
};

// The implementation behind CreateDataArray (0x1c bytes).
class MfScDataArrayImp : public MfScDataArray {
public:
    MfScDataArrayImp(int size);         // 0x100049d0
    virtual ~MfScDataArrayImp();
protected:
    virtual MfScLinkData *CountCircLinks(MfScDataHandle *h);
    virtual void ReleaseCircLinks(MfScLinkData *l);
    virtual int GetThroughCount(MfScDataHandle *h);
public:
    virtual int SetAt(int index, MfScDataHandle *value, int noAddRef);
    virtual int GetAt(int index, MfScDataHandle **value, int addRef);
    virtual int IsValid();
private:
    MfScDataHandle **m_items; // +0x18
};
