// MfPoColor (MfPoLibrary.dll 0x10029880-0x1002a980)
#include "MfPoColor.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x10029880 ?GetPropTokenTable@MfPoColor@@EAEPAVMfScPropTokenTable@@XZ
// 0x10029890 ??0MfPoColor@@QAE@ABV0@@Z (implicit)
// 0x10029910 ??4MfPoColor@@QAEAAV0@ABV0@@Z (implicit)
// 0x10029950 ??_EMfPoColor@@UAEPAXI@Z

// 0x100299a0 _$E24
// 0x100299b0 _$E21
// 0x100299d0 _$E23
// 0x100299e0 _$E22
static MfString strClassName("Color");
MfPoColor *MfPoColor::poExemplar;
MfScPropTokenTable *MfPoColor::pttPropTokens;

// a number argument clamped to a colour component
static inline float ColorComponent(MfScDataHandle *value)
{
    float c = value->GetNumber();
    if (c < 0.0f)
        c = 0.0f;
    if (c > 1.0f)
        c = 1.0f;
    return c;
}

// 0x100299f0 ?GetClassNameA@MfPoColor@@EAE?AVMfString@@XZ
MfString MfPoColor::GetClassName()
{
    return strClassName;
}

// 0x10029a20 ??0MfPoColor@@QAE@VMfColor@@@Z
MfPoColor::MfPoColor(MfColor color) : MfPoBase(1)
{
    mColor = color;
}

// 0x10029aa0 ??0MfPoColor@@AAE@VExemplar@MfScData@@@Z
MfPoColor::MfPoColor(MfScData::Exemplar) : MfPoBase(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Color");
    MfScCiMethod *m;
    SetProperty(0, "Color", MfScDataHandle::CreateData((MfScMethod)&MfPoColor::PMColor, "Color::Color", 4), 1);
    m = new MfScCiMethod("Color");
    *ci + m;
    *m + new MfScMethodParameterInfo("float red", 0);
    *m + new MfScMethodParameterInfo("float green", 0);
    *m + new MfScMethodParameterInfo("float blue", 0);
    *m + new MfScMethodParameterInfo("float alpha", 0);
    *ci + new MfScCiProperty("red - float");
    *ci + new MfScCiProperty("green - float");
    *ci + new MfScCiProperty("blue - float");
    *ci + new MfScCiProperty("alpha - float");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("red", 0);
    pttPropTokens->Add("green", 1);
    pttPropTokens->Add("blue", 2);
    pttPropTokens->Add("alpha", 3);
    MfScDataObject::AddClassInfo(ci);
}

// 0x1002a2f0 ??1MfPoColor@@UAE@XZ
MfPoColor::~MfPoColor()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x1002a380 ?Initialize@MfPoColor@@SAXXZ
void MfPoColor::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoColor(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1002a410 ?Shutdown@MfPoColor@@SAXXZ
void MfPoColor::Shutdown()
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

// 0x1002a470 ?GetProperty@MfPoColor@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoColor::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mColor.m_r);
            break;
        case 1:
            *value = MfScDataHandle::CreateData(mColor.m_g);
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mColor.m_b);
            break;
        case 3:
            *value = MfScDataHandle::CreateData(mColor.m_a);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x1002a550 ?SetProperty@MfPoColor@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoColor::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            if (!value->IsType(0x30))
                goto badType;
            mColor.m_r = ColorComponent(value);
            break;
        case 1:
            if (!value->IsType(0x30))
                goto badType;
            mColor.m_g = ColorComponent(value);
            break;
        case 2:
            if (!value->IsType(0x30))
                goto badType;
            mColor.m_b = ColorComponent(value);
            break;
        case 3:
            if (!value->IsType(0x30))
                goto badType;
            mColor.m_a = ColorComponent(value);
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

// 0x1002a700 ?PMColor@MfPoColor@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoColor::PMColor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfScDataHandle *result = 0;
    if (CheckNull(MfString("Color.Color()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x30) && args[1]->IsType(0x30) && args[2]->IsType(0x30) && args[3]->IsType(0x30)) {
        MfColor color;
        color.m_r = ColorComponent(args[0]);
        color.m_g = ColorComponent(args[1]);
        color.m_b = ColorComponent(args[2]);
        color.m_a = ColorComponent(args[3]);
        result = MfScDataHandle::CreateData(new MfPoColor(color));
    } else {
        InvalidType(MfString("Color"), MfString("Color"), 0, nArgs, args,
                    &typeid(float), &typeid(float), &typeid(float), &typeid(float));
    }
    return result;
}

// 0x1002a960 ?FilloutPropertyNameList@MfPoColor@@EAEXPAVMfStringList@@@Z
void MfPoColor::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x100300b0 ?Color@MfPoColor@@QAEAAVMfColor@@XZ (inline; the first copy is MfPoLibrary.cpp's)
