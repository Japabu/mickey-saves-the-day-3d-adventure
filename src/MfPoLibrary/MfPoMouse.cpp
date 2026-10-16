// MfPoMouse (MfPoLibrary.dll 0x10035500-0x100364d0; its inline members are
// emitted first by MfPoLibrary.cpp)
#include "MfPoMouse.h"
#include "MfPoActor.h"
#include "MfPoLibrary.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfAiMouse.h"
#include "MfAiWorld.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScPropTokenTable.h"
#include "MfScVmEnv.h"

// 0x1002ff80 ?GetPropTokenTable@MfPoMouse@@EAEPAVMfScPropTokenTable@@XZ
// 0x1002ff90 ??0MfPoMouse@@QAE@ABV0@@Z (implicit)
// 0x1002ffc0 ??_EMfPoMouse@@UAEPAXI@Z

// 0x10035500 _$E27
// 0x10035510 _$E24
// 0x10035530 _$E26
// 0x10035540 _$E25
static MfString strClassName("Mouse");
MfPoMouse *MfPoMouse::poExemplar;
MfScPropTokenTable *MfPoMouse::pttPropTokens;

// 0x10035550 ?GetClassNameA@MfPoMouse@@EAE?AVMfString@@XZ
MfString MfPoMouse::GetClassName()
{
    return strClassName;
}

// 0x10035580 ??0MfPoMouse@@QAE@XZ
MfPoMouse::MfPoMouse() : MfPoBase(1)
{
}

// 0x100355a0 ??0MfPoMouse@@AAE@VExemplar@MfScData@@@Z
MfPoMouse::MfPoMouse(MfScData::Exemplar) : MfPoBase(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("Mouse");
    MfScCiMethod *m;
    SetProperty(0, "SetMouseRoute", MfScDataHandle::CreateData((MfScMethod)&MfPoMouse::PMSetMouseRoute, "Mouse::SetMouseRoute", 2), 1);
    m = new MfScCiMethod("SetMouseRoute");
    *ci + m;
    *m + new MfScMethodParameterInfo("int mouseCode", 0);
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "ClearMouseRoute", MfScDataHandle::CreateData((MfScMethod)&MfPoMouse::PMClearMouseRoute, "Mouse::ClearMouseRoute", 2), 1);
    m = new MfScCiMethod("ClearMouseRoute");
    *ci + m;
    *m + new MfScMethodParameterInfo("int mouseCode", 0);
    *m + new MfScMethodParameterInfo("Actor actor", 0);
    SetProperty(0, "DisableToolMouseBehavior", MfScDataHandle::CreateData((MfScMethod)&MfPoMouse::PMDisableToolMouseBehavior, "Mouse::DisableToolMouseBehavior", 0), 1);
    m = new MfScCiMethod("DisableToolMouseBehavior");
    *ci + m;
    SetProperty(0, "EnableToolMouseBehavior", MfScDataHandle::CreateData((MfScMethod)&MfPoMouse::PMEnableToolMouseBehavior, "Mouse::EnableToolMouseBehavior", 0), 1);
    m = new MfScCiMethod("EnableToolMouseBehavior");
    *ci + m;
    *ci + new MfScCiProperty("lastPickedActor - Actor (read only)");
    *ci + new MfScCiProperty("lastPickedFloor - ActorNode (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("lastPickedActor", 0);
    pttPropTokens->Add("lastPickedFloor", 1);
}

// 0x10035e50 ??1MfPoMouse@@UAE@XZ
MfPoMouse::~MfPoMouse()
{
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x10035ee0 ?Initialize@MfPoMouse@@SAXXZ
void MfPoMouse::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoMouse(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x10035f70 ?Shutdown@MfPoMouse@@SAXXZ
void MfPoMouse::Shutdown()
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

// 0x10035fd0 ?GetProperty@MfPoMouse@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoMouse::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    switch (LookupPropertyToken(name, pttPropTokens)) {
    case 0: {
        MfAiActor *actor = g_paiMouse->GetLastPickedActor();
        if (actor)
            *value = actor->GetPiccoloHandle();
        else
            *value = 0;
        break;
    }
    case 1: {
        MfAiActorNode *floor = g_paiMouse->GetLastPickedFloor();
        if (floor)
            *value = floor->GetPiccoloHandle();
        else
            *value = 0;
        break;
    }
    default:
        if (this == poExemplar)
            err = MfScDataObject::GetProperty(name, value);
        else
            err = poExemplar->GetProperty(name, value);
        break;
    }
    return err;
}

// 0x10036080 ?SetProperty@MfPoMouse@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoMouse::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_NO_ERROR;
}

// 0x100360d0 ?PMSetMouseRoute@MfPoMouse@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMouse::PMSetMouseRoute(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Mouse.SetMouseRoute()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && actor) {
        MfAiActor *aiActor = actor->GetActorPtr();
        int mouseCode = args[0]->GetInt();
        if (!aiActor) {
            BadParameters(MfString("Mouse.ClearMouseRoute()   The actor is bad."));
            return 0;
        }
        g_paiMouse->SetMouseRoute(mouseCode, aiActor);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Mouse"), MfString("SetMouseRoute"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    return 0;
}

// 0x100361f0 ?PMClearMouseRoute@MfPoMouse@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMouse::PMClearMouseRoute(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (CheckNull(MfString("Mouse.ClearMouseRoute()"), nArgs, args, 0))
        return 0;
    MfPoActor *actor = args[1] ? dynamic_cast<MfPoActor *>(args[1]->GetData()) : 0;
    if (args[0]->IsType(MfScDataHandle::HT_INTEGER) && actor) {
        MfAiActor *aiActor = actor->GetActorPtr();
        int mouseCode = args[0]->GetInt();
        if (!aiActor) {
            BadParameters(MfString("Mouse.ClearMouseRoute()   The actor is bad."));
            return 0;
        }
        g_paiMouse->ClearMouseRoute(mouseCode, aiActor);
        return MfScDataHandle::CreateData(1);
    }
    InvalidType(MfString("Mouse"), MfString("ClearMouseRoute"), 0, nArgs, args, &typeid(int), &typeid(MfPoActor));
    return 0;
}

// 0x10036310 ?InitConstants@MfPoMouse@@SA?AVMfString@@XZ
MfString MfPoMouse::InitConstants()
{
    MfString s;
    s << "global " << "MF_MOUSE_EVENT_LBTN_DOWN" << " = " << 0 << ";";
    s << "global " << "MF_MOUSE_EVENT_LBTN_UP" << " = " << 1 << ";";
    s << "global " << "MF_MOUSE_EVENT_RBTN_DOWN" << " = " << 2 << ";";
    s << "global " << "MF_MOUSE_EVENT_RBTN_UP" << " = " << 3 << ";";
    s << "global " << "MF_MOUSE_EVENT_MOVE" << " = " << 4 << ";";
    return s;
}

// 0x10036470 ?FilloutPropertyNameList@MfPoMouse@@EAEXPAVMfStringList@@@Z
void MfPoMouse::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x10036490 ?PMEnableToolMouseBehavior@MfPoMouse@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMouse::PMEnableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiWorld->EnableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}

// 0x100364b0 ?PMDisableToolMouseBehavior@MfPoMouse@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoMouse::PMDisableToolMouseBehavior(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    g_paiWorld->DisableToolMouseBehavior();
    return MfScDataHandle::CreateData(1);
}
