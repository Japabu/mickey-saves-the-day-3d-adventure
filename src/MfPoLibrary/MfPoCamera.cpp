// MfPoCamera (MfPoLibrary.dll 0x10028bb0-0x10029880; the inline members are
// emitted first by MfPoActorNode.cpp)
#include "MfPoCamera.h"
#include "MfAiActorNode.h"
#include "MfCamera.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x10028bb0 _$E27
// 0x10028bc0 _$E24
// 0x10028be0 _$E26
// 0x10028bf0 _$E25
static MfString strClassName("CameraNode");
MfPoCamera *MfPoCamera::poExemplar;
MfScPropTokenTable *MfPoCamera::pttPropTokens;

// 0x10028c00 ?GetClassNameA@MfPoCamera@@EAE?AVMfString@@XZ
MfString MfPoCamera::GetClassName()
{
    return strClassName;
}

// 0x10028c30 ??0MfPoCamera@@QAE@PAVMfAiActorNode@@PAVMfCamera@@@Z
MfPoCamera::MfPoCamera(MfAiActorNode *node, MfCamera *camera) : MfPoActorNode(node), mCamera(camera), mNode(node)
{
}

// 0x10028c60 ??0MfPoCamera@@AAE@VExemplar@MfScData@@@Z
MfPoCamera::MfPoCamera(MfScData::Exemplar) : MfPoActorNode(EXEMPLAR_CHILD_INSTANCE), mCamera(0), mNode(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("CameraNode");
    *ci + new MfScCiProperty("cameraName - String (read only)");
    *ci + new MfScCiProperty("projMode - int");
    *ci + new MfScCiProperty("fov - float");
    *ci + new MfScCiProperty("nearClip - float");
    *ci + new MfScCiProperty("farClip - float");

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("cameraName", 0);
    pttPropTokens->Add("projMode", 1);
    pttPropTokens->Add("fov", 2);
    pttPropTokens->Add("nearClip", 3);
    pttPropTokens->Add("farClip", 4);

    MfScDataObject::AddClassInfo(ci);
}

// 0x100292f0 ??1MfPoCamera@@UAE@XZ
MfPoCamera::~MfPoCamera()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10029370 ?Initialize@MfPoCamera@@SAXXZ
void MfPoCamera::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoCamera(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10029400 ?Shutdown@MfPoCamera@@SAXXZ
void MfPoCamera::Shutdown()
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

// 0x10029460 ?GetProperty@MfPoCamera@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoCamera::GetProperty(const MfString &name, MfScDataHandle **value)
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
        *value = MfScDataHandle::CreateData(mCamera->m_name, 0);
        break;
    case 1:
        *value = MfScDataHandle::CreateData(mCamera->m_projMode);
        break;
    case 2:
        *value = MfScDataHandle::CreateData(mCamera->m_fov);
        break;
    case 3:
        *value = MfScDataHandle::CreateData(mCamera->m_nearClip);
        break;
    case 4:
        *value = MfScDataHandle::CreateData(mCamera->m_farClip);
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

// 0x10029590 ?SetProperty@MfPoCamera@@EAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoCamera::SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int release)
{
    bool passedOn = false;
    MfPropertyErrorCode err = PEC_NO_ERROR;
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (value) {
        if (mNode == (MfAiActorNode *)-1)
            return PEC_INVALID_HANDLE;
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 1:
            err = PEC_INVALID_PROPERTY_TYPE;
            if (value->IsType(MfScDataHandle::HT_INTEGER)) {
                int mode = value->GetInt();
                if (mode == 0 || mode == 1) {
                    mCamera->m_projMode = mode;
                    mNode->SyncAllViews(4);
                    err = PEC_NO_ERROR;
                }
            }
            break;
        case 2:
            if (value->IsType(MfScDataHandle::HT_NUMBER)) {
                mCamera->m_fov = value->GetNumber();
                if (mCamera->m_fov < 0.0f)
                    mCamera->m_fov = -mCamera->m_fov;
                mNode->SyncAllViews(1);
            } else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 3:
            if (value->IsType(MfScDataHandle::HT_NUMBER)) {
                mCamera->m_nearClip = value->GetNumber();
                if (mCamera->m_nearClip > mCamera->m_farClip)
                    mCamera->m_nearClip = mCamera->m_farClip;
                mNode->SyncAllViews(2);
            } else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        case 4:
            if (value->IsType(MfScDataHandle::HT_NUMBER)) {
                mCamera->m_farClip = value->GetNumber();
                if (mCamera->m_nearClip > mCamera->m_farClip)
                    mCamera->m_farClip = mCamera->m_nearClip;
                mNode->SyncAllViews(2);
            } else
                err = PEC_INVALID_PROPERTY_TYPE;
            break;
        default:
            err = MfPoActorNode::SetProperty(self, name, value, release);
            passedOn = true;
            break;
        }
    } else
        err = PEC_INVALID_PROPERTY_TYPE;
    if (!passedOn && release && value)
        value->Release();
    return err;
}

// 0x10029780 ?FilloutPropertyNameList@MfPoCamera@@EAEXPAVMfStringList@@@Z
void MfPoCamera::FilloutPropertyNameList(MfStringList *list)
{
    MfPoActorNode::FilloutPropertyNameList(list);
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x100297a0 ?InitConstants@MfPoCamera@@SA?AVMfString@@XZ
MfString MfPoCamera::InitConstants()
{
    MfString s;
    s << "global " << "MF_CAMERA_PERSPECTIVE" << " = " << 0 << ";";
    s << "global " << "MF_CAMERA_ORTHOGRAPHIC" << " = " << 1 << ";";
    return s;
}


