// MfPoLight (MfPoLibrary.dll 0x10030690-0x10031650; the inline members are
// emitted first by MfPoActorNode.cpp)
#include "MfPoLight.h"
#include "MfPoColor.h"
#include "MfAiActorNode.h"
#include "MfLight.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x10030690 _$E31
// 0x100306a0 _$E28
// 0x100306c0 _$E30
// 0x100306d0 _$E29
static MfString strClassName("LightNode");
MfPoLight *MfPoLight::poExemplar;
MfScPropTokenTable *MfPoLight::pttPropTokens;

// 0x100306e0 ?GetClassNameA@MfPoLight@@EAE?AVMfString@@XZ
MfString MfPoLight::GetClassName()
{
    return strClassName;
}

// 0x10030710 ??0MfPoLight@@QAE@PAVMfAiActorNode@@PAVMfLight@@@Z
MfPoLight::MfPoLight(MfAiActorNode *node, MfLight *light) : MfPoActorNode(node), mLight(light), mNode(node)
{
}

// 0x10030740 ??0MfPoLight@@AAE@VExemplar@MfScData@@@Z
MfPoLight::MfPoLight(MfScData::Exemplar) : MfPoActorNode(EXEMPLAR_CHILD_INSTANCE), mLight(0), mNode(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("LightNode");
    MfScCiMethod *m;
    *ci + new MfScCiProperty("type - int");
    *ci + new MfScCiProperty("ambientColor - Color");
    *ci + new MfScCiProperty("diffuseColor - Color");
    *ci + new MfScCiProperty("specularColor - Color");
    *ci + new MfScCiProperty("spotAngle - float");
    *ci + new MfScCiProperty("enabled - boolean");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("type", 0);
    pttPropTokens->Add("ambientColor", 1);
    pttPropTokens->Add("diffuseColor", 2);
    pttPropTokens->Add("specularColor", 3);
    pttPropTokens->Add("spotAngle", 4);
    pttPropTokens->Add("enabled", 5);
    MfScDataObject::AddClassInfo(ci);
}

// 0x10030ed0 ??1MfPoLight@@UAE@XZ
MfPoLight::~MfPoLight()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10030f50 ?Initialize@MfPoLight@@SAXXZ
void MfPoLight::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoLight(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10030fe0 ?Shutdown@MfPoLight@@SAXXZ
void MfPoLight::Shutdown()
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

// 0x10031040 ?GetProperty@MfPoLight@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoLight::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        if (mNode == (MfAiActorNode *)-1)
            return PEC_INVALID_HANDLE;
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(mLight->m_type);
            break;
        case 1:
            *value = MfScDataHandle::CreateData(new MfPoColor(mLight->m_ambient));
            break;
        case 2:
            *value = MfScDataHandle::CreateData(new MfPoColor(mLight->m_diffuse));
            break;
        case 3:
            *value = MfScDataHandle::CreateData(new MfPoColor(mLight->m_specular));
            break;
        case 4:
            *value = MfScDataHandle::CreateData(mLight->m_spotAngle);
            break;
        case 5:
            *value = MfScDataHandle::CreateData((int)mLight->m_enabled);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
            if (err == PEC_NO_SUCH_PROPERTY)
                err = MfPoActorNode::GetProperty(name, value);
            break;
        }
    }
    return err;
}

// 0x10031230 ?SetProperty@MfPoLight@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoLight::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    bool passedOn = false;
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        if (mNode == (MfAiActorNode *)-1)
            return PEC_INVALID_HANDLE;
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            err = PEC_INVALID_PROPERTY_TYPE;
            if (value->IsType(MfScDataHandle::HT_INTEGER)) {
                int type = value->GetInt();
                if (type >= 0 && type <= 3) {
                    mLight->m_type = type;
                    mNode->SetupLight(mLight->m_enabled);
                    err = PEC_NO_ERROR;
                }
            }
            break;
        case 1: {
            if (!value->IsType(0x200))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mLight->m_ambient = color->Color();
            mNode->SetupLight(mLight->m_enabled);
            break;
        }
        case 2: {
            if (!value->IsType(0x200))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mLight->m_diffuse = color->Color();
            mNode->SetupLight(mLight->m_enabled);
            break;
        }
        case 3: {
            if (!value->IsType(0x200))
                goto badType;
            MfPoColor *color = dynamic_cast<MfPoColor *>(value->GetData());
            if (!color)
                goto badType;
            mLight->m_specular = color->Color();
            mNode->SetupLight(mLight->m_enabled);
            break;
        }
        case 4:
            if (!value->IsType(MfScDataHandle::HT_NUMBER))
                goto badType;
            {
                float angle = value->GetNumber();
                if (angle < 0.0f)
                    angle = 0.0f;
                mLight->m_spotAngle = angle;
            }
            mNode->SetupLight(mLight->m_enabled);
            break;
        case 5:
            if (!value->IsType(MfScDataHandle::HT_INTEGER))
                goto badType;
            mLight->m_enabled = value->GetInt() != 0;
            mNode->SetupLight(mLight->m_enabled);
            break;
        default:
            err = MfPoActorNode::SetProperty(self, name, value, release);
            passedOn = true;
            break;
        }
    } else {
badType:
        err = PEC_INVALID_PROPERTY_TYPE;
    }
    if (!passedOn && release && value)
        value->Release();
    return err;
}

// 0x10031500 ?FilloutPropertyNameList@MfPoLight@@EAEXPAVMfStringList@@@Z
void MfPoLight::FilloutPropertyNameList(MfStringList *list)
{
    MfPoActorNode::FilloutPropertyNameList(list);
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x10031520 ?InitConstants@MfPoLight@@SA?AVMfString@@XZ
MfString MfPoLight::InitConstants()
{
    MfString s;
    s << "global " << "MF_LIGHT_POINT" << " = " << 0 << ";";
    s << "global " << "MF_LIGHT_PARALLEL" << " = " << 1 << ";";
    s << "global " << "MF_LIGHT_SPOT" << " = " << 2 << ";";
    s << "global " << "MF_LIGHT_CUSTOM" << " = " << 3 << ";";
    return s;
}
