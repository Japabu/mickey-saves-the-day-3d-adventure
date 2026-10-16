// PiccoloRT.dll 0x10016410-0x100166ae: MfScSourceResolver and the default
// list interface of MfScStdLib.
#include "MfScSourceResolver.h"
#include "MfScStdLib.h"
#include "MfDictionary.h"
#include "MfString.h"
#include <string.h>

// 0x10016410 ??0MfScSourceResolver@@QAE@XZ
MfScSourceResolver::MfScSourceResolver()
    : mRefCount(1)
{
    mTypes = new MfDictionary(0x35);
}

// 0x100164a0 ??_GMfScSourceResolver@@UAEPAXI@Z
// 0x100164d0 ??1MfScSourceResolver@@UAE@XZ
MfScSourceResolver::~MfScSourceResolver()
{
    mTypes->Clear(this);
    delete mTypes;
    mTypes = 0;
}

// 0x10016550 ?GetInterfaceName@MfScSourceResolver@@UAEXPADHAAH1@Z
void MfScSourceResolver::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "SourceDebugger", size);
    major = 1;
    minor = 0;
}

// 0x10016580 ?AddRef@MfScSourceResolver@@UAEHXZ
int MfScSourceResolver::AddRef()
{
    return ++mRefCount;
}

// 0x10016590 ?Release@MfScSourceResolver@@UAEHXZ
int MfScSourceResolver::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// 0x100165b0 ?Register@MfScSourceResolver@@UAEHABVMfString@@PAVMfAiSourceType@@@Z
int MfScSourceResolver::Register(const MfString &type, MfAiSourceType *sourceType)
{
    if (mTypes->Add(type, sourceType))
        return 2;
    sourceType->AddRef();
    return 0;
}

// 0x100165e0 ?Unregister@MfScSourceResolver@@UAEHABVMfString@@@Z
int MfScSourceResolver::Unregister(const MfString &type)
{
    MfAiSourceType *sourceType = (MfAiSourceType *)mTypes->Delete(type);
    if (sourceType) {
        sourceType->Release();
        return 0;
    }
    return 1;
}

// 0x10016610 ?Find@MfScSourceResolver@@UAEHABVMfString@@PAPAVMfAiSourceType@@@Z
int MfScSourceResolver::Find(const MfString &type, MfAiSourceType **sourceType)
{
    *sourceType = (MfAiSourceType *)mTypes->Find(type);
    if (*sourceType) {
        (*sourceType)->AddRef();
        return 1;
    }
    return 0;
}

// 0x10016640 ?CallBack@MfScSourceResolver@@UAEHPAX@Z
int MfScSourceResolver::CallBack(void *data)
{
    ((MfAiSourceType *)data)->Release();
    return 1;
}

// 0x10016660 _$E8
// 0x10016670 _$E7
MfScStdLibListInterface MfScStdLib::slliDefault;

// 0x10016680 ?CreateList@MfScStdLibListInterface@@UAEHPAPAVMfDblList@@PAPAVMfScDataHandle@@@Z
int MfScStdLibListInterface::CreateList(MfDblList **list, MfScDataHandle **handle)
{
    *list = 0;
    *handle = 0;
    return 0;
}

// 0x100166a0 ?GetList@MfScStdLibListInterface@@UAEHPAPAVMfDblList@@PAVMfScDataHandle@@@Z
int MfScStdLibListInterface::GetList(MfDblList **list, MfScDataHandle *)
{
    *list = 0;
    return 0;
}
