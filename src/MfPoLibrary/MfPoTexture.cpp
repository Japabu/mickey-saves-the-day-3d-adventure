// MfPoTexture (MfPoLibrary.dll 0x1003ada0-0x1003bc40; its inline members are
// emitted first by MfPoLibrary.cpp)
#include "MfPoTexture.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x100300c0 ?GetPropTokenTable@MfPoTexture@@EAEPAVMfScPropTokenTable@@XZ
// 0x100300d0 ??0MfPoTexture@@QAE@ABV0@@Z (implicit)
// 0x10030160 ??4MfPoTexture@@QAEAAV0@ABV0@@Z (implicit)
// 0x100301a0 ??_EMfPoTexture@@UAEPAXI@Z

// 0x1003ada0 _$E26
// 0x1003adb0 _$E23
// 0x1003add0 _$E25
// 0x1003ade0 _$E24
static MfString strClassName("Texture");
MfPoTexture *MfPoTexture::poExemplar;
MfScPropTokenTable *MfPoTexture::pttPropTokens;

// 0x1003adf0 ?GetClassNameA@MfPoTexture@@EAE?AVMfString@@XZ
MfString MfPoTexture::GetClassName()
{
    return strClassName;
}

// 0x1003ae20 ??0MfPoTexture@@QAE@UMfTexture@@@Z
MfPoTexture::MfPoTexture(MfTexture texture) : MfPoBase(1)
{
    mTexture = texture;
}

// 0x1003aec0 ??0MfPoTexture@@AAE@VExemplar@MfScData@@@Z
MfPoTexture::MfPoTexture(MfScData::Exemplar) : MfPoBase(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Texture");
    MfScCiMethod *m;
    SetProperty(0, "Texture", MfScDataHandle::CreateData((MfScMethod)&MfPoTexture::PMTexture, "Texture::Texture", 3), 1);
    m = new MfScCiMethod("Texture");
    *ci + m;
    *m + new MfScMethodParameterInfo("String name", 0);
    *m + new MfScMethodParameterInfo("int type", 0);
    *m + new MfScMethodParameterInfo("int flags", 0);
    *ci + new MfScCiProperty("name - String");
    *ci + new MfScCiProperty("type - int");
    *ci + new MfScCiProperty("flags - int");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("name", 0);
    pttPropTokens->Add("type", 1);
    pttPropTokens->Add("flags", 2);
    MfScDataObject::AddClassInfo(ci);
}

// 0x1003b5c0 ??1MfPoTexture@@UAE@XZ
MfPoTexture::~MfPoTexture()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x1003b650 ?Initialize@MfPoTexture@@SAXXZ
void MfPoTexture::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoTexture(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1003b6e0 ?Shutdown@MfPoTexture@@SAXXZ
void MfPoTexture::Shutdown()
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

// 0x1003b740 ?GetProperty@MfPoTexture@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoTexture::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mTexture.m_name, 0);
            break;
        case 1:
            *value = MfScDataHandle::CreateData(mTexture.m_type);
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mTexture.m_flags);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x1003b7f0 ?SetProperty@MfPoTexture@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoTexture::SetProperty(MfScDataHandle *self, const MfString &name,
                                                             MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            if (!value->IsType(MfScDataHandle::HT_STRING))
                goto badType;
            mTexture.m_name = *(MfScObjString *)value->GetData();
            break;
        case 1:
            err = PEC_INVALID_PROPERTY_TYPE;
            if (value->IsType(MfScDataHandle::HT_INTEGER)) {
                int type = value->GetInt();
                if (type >= 0 && type <= 3) {
                    mTexture.m_type = type;
                    err = PEC_NO_ERROR;
                }
            }
            break;
        case 2:
            err = PEC_INVALID_PROPERTY_TYPE;
            if (value->IsType(MfScDataHandle::HT_INTEGER)) {
                mTexture.m_flags = value->GetInt();
                err = PEC_NO_ERROR;
            }
            break;
        }
    } else {
badType:
        err = PEC_INVALID_PROPERTY_TYPE;
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x1003b8e0 ?PMTexture@MfPoTexture@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoTexture::PMTexture(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (CheckNull(MfString("Texture.Texture()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_STRING) && args[1]->IsType(MfScDataHandle::HT_INTEGER)
        && args[2]->IsType(MfScDataHandle::HT_INTEGER)) {
        MfTexture texture;
        texture.m_name = *(MfScObjString *)args[0]->GetData();
        texture.m_type = args[1]->GetInt();
        texture.m_flags = args[2]->GetInt();
        result = MfScDataHandle::CreateData(new MfPoTexture(texture));
    } else {
        InvalidType(MfString("Texture"), MfString("Texture"), 0, nArgs, args,
                    &typeid(MfScObjString), &typeid(int), &typeid(int));
    }
    return result;
}

// 0x1003ba90 ?FilloutPropertyNameList@MfPoTexture@@EAEXPAVMfStringList@@@Z
void MfPoTexture::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x1003bab0 ?InitConstants@MfPoTexture@@SA?AVMfString@@XZ
MfString MfPoTexture::InitConstants()
{
    MfString s;
    s << "global " << "MF_TEXTURE_COLORMAP" << " = " << 0 << ";";
    s << "global " << "MF_TEXTURE_ENVMAP" << " = " << 1 << ";";
    s << "global " << "MF_TEXTURE_BUMPMAP" << " = " << 2 << ";";
    s << "global " << "MF_TEXTURE_NO_FLAGS" << " = " << 0 << ";";
    s << "global " << "MF_TEXTURE_FILTER" << " = " << 1 << ";";
    s << "global " << "MF_TEXTURE_MIPMAP" << " = " << 8 << ";";
    return s;
}
