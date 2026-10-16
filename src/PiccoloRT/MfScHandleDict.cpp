// PiccoloRT.dll 0x1000b8a0-0x1000ba33: MfScHandleDict.
#include "MfScHandleDict.h"
#include "MfCallBack.h"
#include "MfDictionary.h"
#include "MfScDataHandle.h"
#include "MfScHandleDictIterator.h"
#include "MfString.h"

// Releases the handles of a pool that is being cleared.
class MfScHandleReleaser : public MfCallBack {
public:
    virtual int CallBack(void *data);
};

// 0x1000b8a0 ?Add@MfScHandleDict@@QAEHABVMfString@@PAVMfScDataHandle@@W4HandleDictPassRef@1@@Z
int MfScHandleDict::Add(MfString const &name, MfScDataHandle *handle, HandleDictPassRef passRef)
{
    if (!MfDictionary::Add(name, handle)) {
        if (passRef == kAddRef && handle && handle != pdhUndefined)
            handle->AddRef();
        return 1;
    }
    return 0;
}

// 0x1000b8e0 ?Replace@MfScHandleDict@@QAEXABVMfString@@PAVMfScDataHandle@@W4HandleDictPassRef@1@@Z
void MfScHandleDict::Replace(MfString const &name, MfScDataHandle *handle, HandleDictPassRef passRef)
{
    MfScDataHandle *old = (MfScDataHandle *)MfDictionary::Replace(name, handle);
    if (passRef == kAddRef && handle && handle != pdhUndefined)
        handle->AddRef();
    if (old && old != pdhUndefined)
        old->Release();
}

// 0x1000b930 ?Delete@MfScHandleDict@@QAEXABVMfString@@PAPAVMfScDataHandle@@@Z
void MfScHandleDict::Delete(MfString const &name, MfScDataHandle **handle)
{
    MfScDataHandle *old = (MfScDataHandle *)MfDictionary::Delete(name);
    if (handle)
        *handle = old;
    else if (old && old != pdhUndefined)
        old->Release();
}

// 0x1000b970 ?Find@MfScHandleDict@@QAEPAVMfScDataHandle@@ABVMfString@@@Z
MfScDataHandle * MfScHandleDict::Find(MfString const &rName)
{
    return (MfScDataHandle *)MfDictionary::Find(rName);
}

// 0x1000b980 ?EnumHandles@MfScHandleDict@@QAEXAAVMfScHandleDictIterator@@@Z
void MfScHandleDict::EnumHandles(MfScHandleDictIterator &iterator)
{
    Iterate(iterator);
}

// 0x1000b990 ?Clear@MfScHandleDict@@QAEXXZ
void MfScHandleDict::Clear()
{
    MfScHandleReleaser releaser;
    ClearItems(&releaser);
}

// 0x1000b9d0 ?CallBack@MfScHandleDictIterator@@UAEHPAX00@Z
int MfScHandleDictIterator::CallBack(void *key, void *data, void *)
{
    MfScDataHandle *handle = (MfScDataHandle *)data;
    if (handle && handle != pdhUndefined)
        handle->AddRef();
    EnumHandle(*(MfString *)key, handle);
    if (handle && handle != pdhUndefined)
        handle->Release();
    return 1;
}

// 0x1000ba10 ?CallBack@MfScHandleReleaser@@UAEHPAX@Z
int MfScHandleReleaser::CallBack(void *data)
{
    MfScDataHandle *handle = (MfScDataHandle *)data;
    if (handle && handle != pdhUndefined)
        handle->Release();
    return 1;
}
