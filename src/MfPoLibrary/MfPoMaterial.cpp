// MfPoMaterial (MfPoLibrary.dll 0x10032890-0x10035500; the inline members are
// emitted first by MfPoActorNode.cpp)
#include <typeinfo.h>
#include "MfPoMaterial.h"
#include "MfMaterial.h"
#include "MfPoColor.h"
#include "MfPoTexture.h"
#include "MfProperty.h"
#include "MfScObjString.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScPropertyExtList.h"
#include "MfScVmEnv.h"

// 0x10023300 ?GetPropTokenTable@MfPoMaterial@@EAEPAVMfScPropTokenTable@@XZ
// 0x10023310 ?GetPropertyExtensions@MfPoMaterial@@EAEPAVMfScPropertyExtList@@XZ
// 0x10023320 ??0MfPoMaterial@@QAE@ABV0@@Z (implicit)
// 0x10023370 ??_EMfPoMaterial@@UAEPAXI@Z

// 0x10032890 _$E32
// 0x100328a0 _$E29
// 0x100328c0 _$E31
// 0x100328d0 _$E30
static MfString strClassName("Material");
static bool CopyColorTo(MfScDataHandle *handle, const MfColor &color);

MfPoMaterial *MfPoMaterial::poExemplar;
MfScPropertyExtList *MfPoMaterial::pelPropExtensions;
MfScPropTokenTable *MfPoMaterial::pttPropTokens;

// 0x100328e0 ?GetClassNameA@MfPoMaterial@@EAE?AVMfString@@XZ
MfString MfPoMaterial::GetClassName()
{
    return strClassName;
}

// 0x10032910 ??0MfPoMaterial@@QAE@PAVMfMaterial@@H@Z
MfPoMaterial::MfPoMaterial(MfMaterial *material, int owner) : MfPoBase(1), mMaterial(material), bOwner(owner)
{
}

// 0x10032940 ??0MfPoMaterial@@AAE@VExemplar@MfScData@@@Z
MfPoMaterial::MfPoMaterial(MfScData::Exemplar) : MfPoBase(0), mMaterial(0), bOwner(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Material");
    MfScCiMethod *m;
    SetProperty(0, "GetSpecularColor", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMGetSpecularColor, "Material::GetSpecularColor", 1), 1);
    SetProperty(0, "GetColor", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMGetColor, "Material::GetColor", 1), 1);
    SetProperty(0, "SetSpecularColor", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMSetSpecularColor, "Material::SetSpecularColor", -1), 1);
    SetProperty(0, "SetColor", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMSetColor, "Material::SetColor", -1), 1);
    SetProperty(0, "GetSurface", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMGetSurface, "Material::GetSurface", 1), 1);
    SetProperty(0, "SetSurface", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMSetSurface, "Material::SetSurface", 3), 1);
    SetProperty(0, "CreateTextureCopy", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMCreateTextureCopy, "Material::CreateTextureCopy", 1), 1);
    m = new MfScCiMethod("CreateTextureCopy");
    *ci + m;
    *m + new MfScMethodParameterInfo("int textureIndex", 0);
    SetProperty(0, "SetTexture", MfScDataHandle::CreateData((MfScMethod)&MfPoMaterial::PMSetTexture, "Material::SetTexture", 2), 1);
    m = new MfScCiMethod("SetTexture");
    *ci + m;
    *m + new MfScMethodParameterInfo("int textureIndex", 0);
    *m + new MfScMethodParameterInfo("Texture texture", 0);
    *ci + new MfScCiProperty("shininess - float");
    *ci + new MfScCiProperty("opacity - float");
    *ci + new MfScCiProperty("textureMap - String");
    *ci + new MfScCiProperty("opacityMap - String");
    *ci + new MfScCiProperty("ambientColor - Color");
    *ci + new MfScCiProperty("diffuseColor - Color");
    *ci + new MfScCiProperty("emissiveColor - Color");
    *ci + new MfScCiProperty("specularColor - Color");
    *ci + new MfScCiProperty("flags - int");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("shininess", 0);
    pttPropTokens->Add("opacity", 1);
    pttPropTokens->Add("textureMap", 2);
    pttPropTokens->Add("opacityMap", 3);
    pttPropTokens->Add("ambientColor", 4);
    pttPropTokens->Add("diffuseColor", 5);
    pttPropTokens->Add("emissiveColor", 7);
    pttPropTokens->Add("specularColor", 6);
    pttPropTokens->Add("flags", 8);
    pelPropExtensions = new MfScPropertyExtList;
}

// 0x10033960 ??1MfPoMaterial@@UAE@XZ
MfPoMaterial::~MfPoMaterial()
{
    if (mMaterial && bOwner)
        delete mMaterial;
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar) {
            delete pelPropExtensions;
            pelPropExtensions = 0;
            if (this == poExemplar)
                poExemplar = 0;
        }
    }
}

// 0x10033a10 ?Initialize@MfPoMaterial@@SAXXZ
void MfPoMaterial::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoMaterial(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10033aa0 ?Shutdown@MfPoMaterial@@SAXXZ
void MfPoMaterial::Shutdown()
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

// 0x10033b00 ?GetProperty@MfPoMaterial@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoMaterial::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (GetPropertyExt(name, value))
        return PEC_NO_ERROR;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mMaterial->m_shininess);
            break;
        case 1:
            *value = MfScDataHandle::CreateData(mMaterial->m_diffuse.m_a);
            break;
        case 2:
            *value = MfScDataHandle::CreateData(mMaterial->m_textures[0].m_name, 0);
            break;
        case 3: {
            MfProperty *mask = mMaterial->m_properties.Find("OpacityMask");
            MfString maskName;
            if (mask && mask->ConvertToString(maskName))
                *value = MfScDataHandle::CreateData(maskName, 0);
            else
                *value = MfScDataHandle::CreateData(NULL_STRING, 0);
            break;
        }
        case 4:
            *value = MfScDataHandle::CreateData(new MfPoColor(mMaterial->m_ambient));
            break;
        case 5:
            *value = MfScDataHandle::CreateData(new MfPoColor(mMaterial->m_diffuse));
            break;
        case 6:
            *value = MfScDataHandle::CreateData(new MfPoColor(mMaterial->m_specular));
            break;
        case 7:
            *value = MfScDataHandle::CreateData(new MfPoColor(mMaterial->m_emissive));
            break;
        case 8:
            *value = MfScDataHandle::CreateData(mMaterial->m_shading);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
        }
    }
    return err;
}

// 0x10033e10 ?SetProperty@MfPoMaterial@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoMaterial::SetProperty(MfScDataHandle *self, const MfString &name,
                                                              MfScDataHandle *value, int release)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (SetPropertyExt(self, name, value, release))
        return PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            if (!value->IsType(MfScDataHandle::HT_NUMBER))
                goto badType;
            mMaterial->m_shininess = value->GetNumber();
            break;
        case 1: {
            if (!value->IsType(MfScDataHandle::HT_NUMBER))
                goto badType;
            float opacity = value->GetNumber();
            if (opacity < 0.0f)
                opacity = 0.0f;
            if (opacity > 1.0f)
                opacity = 1.0f;
            mMaterial->m_diffuse.m_a = mMaterial->m_ambient.m_a = opacity;
            break;
        }
        case 2:
            if (!value->IsType(MfScDataHandle::HT_STRING))
                goto badType;
        {
            MfScObjString *texture = (MfScObjString *)value->GetData();
            mMaterial->m_numTextures = 1;
            mMaterial->m_textures[0].m_name = *texture;
        }
            break;
        case 3:
            if (!value->IsType(MfScDataHandle::HT_STRING))
                goto badType;
            mMaterial->SetOpacityMask(*(MfScObjString *)value->GetData());
            break;
        case 4: {
            if (!value->IsType(MfScDataHandle::HT_OBJECT))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mMaterial->m_ambient = color->Color();
            break;
        }
        case 5: {
            if (!value->IsType(MfScDataHandle::HT_OBJECT))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mMaterial->m_diffuse = color->Color();
            break;
        }
        case 6: {
            if (!value->IsType(MfScDataHandle::HT_OBJECT))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mMaterial->m_specular = color->Color();
            break;
        }
        case 7: {
            if (!value->IsType(MfScDataHandle::HT_OBJECT))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mMaterial->m_emissive = color->Color();
            break;
        }
        case 8:
            err = PEC_INVALID_PROPERTY_TYPE;
            if (value->IsType(MfScDataHandle::HT_INTEGER)) {
                err = PEC_NO_ERROR;
                mMaterial->m_shading = value->GetInt();
            }
            break;
        default:
            goto badType;
        }
    } else {
badType:
        err = PEC_INVALID_PROPERTY_TYPE;
    }
    if (release && value)
        value->Release();
    return err;
}

// 0x10034140 ?PMGetSpecularColor@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMGetSpecularColor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("GetSpecularColor");
    static MfString fullName("Material.GetSpecularColor()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(className, methodName, 0, nArgs, args, &typeid(MfScDataObject));
    } else if (!CopyColorTo(args[0], mMaterial->m_specular)) {
        BadParameters(fullName + MfString("Could not initialize color data."));
    }
    return MfScDataHandle::CreateData(0);
}
// 0x100344f0 _$E37 (the destructor of a static string above)
// 0x10034500 _$E36 (the destructor of a static string above)
// 0x10034510 _$E35 (the destructor of a static string above)

// 0x10034350 ?CopyColorTo@@YA_NPAVMfScDataHandle@@ABVMfColor@@@Z (no name in the binary)
// sets the components of the colour object a handle points to
static bool CopyColorTo(MfScDataHandle *handle, const MfColor &color)
{
    MfScDataObject *object = (MfScDataObject *)handle->GetData();
    object->Lock();
    int err = object->SetProperty(handle, "red", MfScDataHandle::CreateData(color.m_r), 1);
    if (!err) {
        err = object->SetProperty(handle, "green", MfScDataHandle::CreateData(color.m_g), 1);
        if (!err) {
            err = object->SetProperty(handle, "blue", MfScDataHandle::CreateData(color.m_b), 1);
            if (!err)
                err = object->SetProperty(handle, "alpha", MfScDataHandle::CreateData(color.m_a), 1);
        }
    }
    object->Unlock();
    return err == 0;
}

// 0x10034520 ?PMGetColor@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMGetColor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("GetColor");
    static MfString fullName("Material.GetColor()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(className, methodName, 0, nArgs, args, &typeid(MfScDataObject));
    } else if (!CopyColorTo(args[0], mMaterial->m_diffuse)) {
        BadParameters(fullName + MfString("Could not initialize color data."));
    }
    return MfScDataHandle::CreateData(0);
}
// 0x10034730 _$E41 (the destructor of a static string above)
// 0x10034740 _$E40 (the destructor of a static string above)
// 0x10034750 _$E39 (the destructor of a static string above)

// 0x10034760 ?PMSetSpecularColor@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMSetSpecularColor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("SetSpecularColor");
    static MfString fullName("Material.SetSpecularColor()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs < 3 || nArgs > 4)
        return MfScDataHandle::CreateData(0);
    for (int i = 0; i < nArgs; i++) {
        if (!args[i]->IsType(MfScDataHandle::HT_NUMBER)) {
            InvalidType(className, methodName, 0, nArgs, args, &typeid(float));
            return MfScDataHandle::CreateData(0);
        }
    }
    mMaterial->m_specular.m_r = args[0]->GetNumber();
    mMaterial->m_specular.m_g = args[1]->GetNumber();
    mMaterial->m_specular.m_b = args[2]->GetNumber();
    if (nArgs == 4)
        mMaterial->m_specular.m_a = args[3]->GetNumber();
    return MfScDataHandle::CreateData(1);
}
// 0x10034970 _$E45 (the destructor of a static string above)
// 0x10034980 _$E44 (the destructor of a static string above)
// 0x10034990 _$E43 (the destructor of a static string above)

// 0x100349a0 ?PMSetColor@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMSetColor(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("SetColor");
    static MfString fullName("Material.SetColor()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    if (nArgs < 3 || nArgs > 4)
        return MfScDataHandle::CreateData(0);
    for (int i = 0; i < nArgs; i++) {
        if (!args[i]->IsType(MfScDataHandle::HT_NUMBER)) {
            InvalidType(className, methodName, 0, nArgs, args, &typeid(float));
            return MfScDataHandle::CreateData(0);
        }
    }
    mMaterial->m_diffuse.m_r = args[0]->GetNumber();
    mMaterial->m_ambient.m_r = mMaterial->m_diffuse.m_r;
    mMaterial->m_diffuse.m_g = args[1]->GetNumber();
    mMaterial->m_ambient.m_g = mMaterial->m_diffuse.m_g;
    mMaterial->m_diffuse.m_b = args[2]->GetNumber();
    mMaterial->m_ambient.m_b = mMaterial->m_diffuse.m_b;
    if (nArgs == 4) {
        mMaterial->m_diffuse.m_a = args[3]->GetNumber();
        mMaterial->m_ambient.m_a = mMaterial->m_diffuse.m_a;
    }
    return MfScDataHandle::CreateData(1);
}
// 0x10034bd0 _$E49 (the destructor of a static string above)
// 0x10034be0 _$E48 (the destructor of a static string above)
// 0x10034bf0 _$E47 (the destructor of a static string above)

// 0x10034c00 ?PMGetSurface@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMGetSurface(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("GetSurface");
    static MfString fullName("Material.GetSurface()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    MfScData *data = args[0]->GetData();
    if (!data || typeid(*data) != typeid(MfScDataObject)) {
        InvalidType(className, methodName, 0, nArgs, args, &typeid(MfScDataObject));
    } else {
        MfScDataObject *surface = (MfScDataObject *)args[0]->GetData();
        surface->Lock();
        int err = surface->SetProperty(args[0], "ambient", MfScDataHandle::CreateData(1.0f), 1);
        if (!err) {
            err = surface->SetProperty(args[0], "diffuse", MfScDataHandle::CreateData(1.0f), 1);
            if (!err)
                err = surface->SetProperty(args[0], "specular", MfScDataHandle::CreateData(1.0f), 1);
        }
        surface->Unlock();
        if (err)
            BadParameters(fullName + MfString("Could not initialize surface data."));
    }
    return MfScDataHandle::CreateData(0);
}
// 0x10034f20 _$E53 (the destructor of a static string above)
// 0x10034f30 _$E52 (the destructor of a static string above)
// 0x10034f40 _$E51 (the destructor of a static string above)

// 0x10034f50 ?PMSetSurface@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// (the messages name GetSurface)
MfScDataHandle *MfPoMaterial::PMSetSurface(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    static MfString className("Material");
    static MfString methodName("GetSurface");
    static MfString fullName("Material.GetSurface()");
    if (InvalidObject(mMaterial, className, methodName))
        return MfScDataHandle::CreateData(0);
    if (CheckNull(fullName, nArgs, args, 0))
        return MfScDataHandle::CreateData(0);
    for (int i = 0; i < nArgs; i++) {
        if (!args[i]->IsType(MfScDataHandle::HT_NUMBER)) {
            InvalidType(className, methodName, 0, nArgs, args, &typeid(float));
            return MfScDataHandle::CreateData(0);
        }
    }
    return MfScDataHandle::CreateData(1);
}
// 0x100350e0 _$E57 (the destructor of a static string above)
// 0x100350f0 _$E56 (the destructor of a static string above)
// 0x10035100 _$E55 (the destructor of a static string above)

// 0x10035110 ?PMCreateTextureCopy@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMCreateTextureCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Material.CreateTextureCopy()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER)) {
        int index = args[0]->GetInt();
        if (index >= 0 && index <= 7)
            return MfScDataHandle::CreateData(new MfPoTexture(mMaterial->m_textures[index]));
        BadParameters(MfString("Material.CreateTextureCopy()  The texture index must be between 0 and 7."));
    } else {
        InvalidType(MfString("Material"), MfString("CreateTextureCopy"), 0, nArgs, args, &typeid(int));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x10035270 ?PMSetTexture@MfPoMaterial@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMaterial::PMSetTexture(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Material.SetTexture()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && args[1]->IsType(MfScDataHandle::HT_OBJECT)) {
        int index = args[0]->GetInt();
        MfPoTexture *texture = args[1] ? dynamic_cast<MfPoTexture *>(args[1]->GetData()) : 0;
        if (index >= 0 && index <= 7) {
            mMaterial->m_textures[index] = texture->Texture();
            mMaterial->m_numTextures = index + 1;
            return MfScDataHandle::CreateData(1);
        }
        BadParameters(MfString("Material.SetTexture()  The texture index must be between 0 and 7."));
    } else {
        InvalidType(MfString("Material"), MfString("SetTexture"), 0, nArgs, args,
                    &typeid(int), &typeid(MfPoTexture));
    }
    return MfScDataHandle::CreateData(0);
}

// 0x100353b0 ?FilloutPropertyNameList@MfPoMaterial@@EAEXPAVMfStringList@@@Z
void MfPoMaterial::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x100353d0 ?InitConstants@MfPoMaterial@@SA?AVMfString@@XZ
MfString MfPoMaterial::InitConstants()
{
    MfString s;
    s << "global " << "MF_MAT_NO_FLAGS" << " = " << 0 << ";";
    s << "global " << "MF_MAT_TWO_SIDED" << " = " << 1 << ";";
    s << "global " << "MF_MAT_SMOOTH_SHADED" << " = " << 2 << ";";
    s << "global " << "MF_MAT_LIT" << " = " << 4 << ";";
    return s;
}
