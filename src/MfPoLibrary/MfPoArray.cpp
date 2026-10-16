// MfPoArray (MfPoLibrary.dll 10026900-10026de0)
#include "MfPoArray.h"
#include "MfPoBase.h"
#include "MfArrayValue.h"
#include "MfProperty.h"
#include "MfScDataHandle.h"
#include "MfScVmEnv.h"

// 0x10026900 ?ArrayValue@MfPoArray@@QBEPAVMfArrayValue@@XZ
// 0x10026910 ?IsContainer@MfPoArray@@MAEHXZ
// 0x10026920 ??0MfPoArray@@QAE@ABV0@@Z (implicit)
// 0x10026970 ??_EMfPoArray@@UAEPAXI@Z
// 0x100269c0 _$E23
// 0x100269d0 _$E20
// 0x100269f0 _$E22
// 0x10026a00 _$E21

static MfString strClassName("PropertyArray");
MfPoArray* MfPoArray::poExemplar;

// 0x10026a10 ?GetClassNameA@MfPoArray@@EAE?AVMfString@@XZ
MfString MfPoArray::GetClassName()
{
    return strClassName;
}

// 0x10026a40 ??0MfPoArray@@QAE@PAVMfArrayValue@@@Z
MfPoArray::MfPoArray(MfArrayValue *value)
    : MfScDataArray(value->NumItems())
{
    mArray = value;
}

// 0x10026a70 ??0MfPoArray@@AAE@VExemplar@MfScData@@@Z
MfPoArray::MfPoArray(MfScData::Exemplar)
    : MfScDataArray(0), mArray(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;
}

// 0x10026b30 ??1MfPoArray@@UAE@XZ
MfPoArray::~MfPoArray()
{
}

// 0x10026b40 ?Initialize@MfPoArray@@SAXXZ
void MfPoArray::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoArray(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10026bd0 ?Shutdown@MfPoArray@@SAXXZ
void MfPoArray::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x10026c30 ?SetArrayItem@MfPoArray@@MAEHHPAVMfScDataHandle@@H@Z
int MfPoArray::SetArrayItem(int index, MfScDataHandle *handle, int release)
{
    int result = 0;
    if (handle && index >= 0 && index < mArray->NumItems()) {
        MfTypedValue *value = MfPoBase::PiccoloToPropertyValue(handle, 1);
        if (value) {
            mArray->SetItem(index, value);
            result = 1;
        }
        if (release)
            handle->Release();
        return result;
    }
    return 0;
}

// 0x10026ca0 ?GetArrayItem@MfPoArray@@MAEHHPAPAVMfScDataHandle@@H@Z
int MfPoArray::GetArrayItem(int index, MfScDataHandle **out, int addRef)
{
    int result = 0;
    *out = 0;
    if (index >= 0 && index < mArray->NumItems()) {
        MfTypedValue *value = mArray->GetItem(index);
        if (value) {
            MfProperty *prop = new MfProperty("Bunta", value);
            *out = MfPoBase::PropertyToPiccolo(prop);
            prop->DetachValue();
            delete prop;
            if (*out)
                result = 1;
        }
        if (addRef && *out)
            (*out)->AddRef();
        return result;
    }
    return 0;
}
