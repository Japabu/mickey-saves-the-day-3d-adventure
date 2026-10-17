// MfScDataArrayImp: the implementation behind MfScDataHandle::CreateDataArray.
#pragma once
#include "MfScDataArray.h"
class MfScDataHandle;
class MfScLinkData;

// The implementation behind CreateDataArray (0x1c bytes; vtable 0x1002079c, not exported).
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
