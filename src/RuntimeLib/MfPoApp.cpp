// MfPoApp (RuntimeLib.dll 0x1007bcd0-0x1007c870): the Piccolo class object
// "App". Its methods call the application's hooks: 'WURL' (open a URL in a
// browser window), 'SdAP' (shut the application down) and
// MF_HOOK_USER_EVENT.
#include "MfPoApp.h"
#include "MfHookManager.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScVmEnv.h"
#include "enums.h"
#include <typeinfo.h>

// the data of the 'WURL' hook (its destructor is emitted at 1007c640)
struct MfURLHookData : public MfHookData {
    MfString url;                                           // 0x10
    MfString target;                                        // 0x14
};

// 0x1007bcd0 _$E20
// 0x1007bce0 _$E17
// 0x1007bd00 _$E19
// 0x1007bd10 _$E18
static MfString strClassName("App");
MfPoApp *MfPoApp::poExemplar;

// 0x1007bd20 ?GetClassNameA@MfPoApp@@EAE?AVMfString@@XZ (folded)
MfString MfPoApp::GetClassName()
{
    return strClassName;
}

// 0x1007bd50 ??_GMfPoApp@@UAEPAXI@Z (folded)

// 0x1007bd70 ??0MfPoApp@@AAE@VExemplar@MfScData@@@Z
MfPoApp::MfPoApp(MfScData::Exemplar) : MfPoBase(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("App");
    MfScCiMethod *m;
    SetProperty(0, "OpenURL", MfScDataHandle::CreateData((MfScMethod)&MfPoApp::PMOpenURL, "App::OpenURL", 2), 1);
    m = new MfScCiMethod("OpenURL");
    *ci + m;
    *m + new MfScMethodParameterInfo("String url", 0);
    *m + new MfScMethodParameterInfo("String target", 0);
    SetProperty(0, "Shutdown", MfScDataHandle::CreateData((MfScMethod)&MfPoApp::PMShutdown, "App::Shutdown", 0), 1);
    m = new MfScCiMethod("Shutdown");
    *ci + m;
    SetProperty(0, "UserEvent", MfScDataHandle::CreateData((MfScMethod)&MfPoApp::PMUserEvent, "App::UserEvent", 2), 1);
    m = new MfScCiMethod("UserEvent");
    *ci + m;
    *m + new MfScMethodParameterInfo("String event", 0);
    *m + new MfScMethodParameterInfo("String data", 0);
    MfScDataObject::AddClassInfo(ci);
}

// 0x1007c330 ??1MfPoApp@@UAE@XZ
MfPoApp::~MfPoApp()
{
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x1007c350 ?Initialize@MfPoApp@@SAXXZ
void MfPoApp::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoApp(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1007c3e0 ?Shutdown@MfPoApp@@SAXXZ (folded)
void MfPoApp::Shutdown()
{
    if (bShutdown == 0) {
        MfScDataHandle *handle;
        bShutdown = 1;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->mFlags &= ~8;
            MfScVmEnv::ClearGlobalVariable(strClassName, 0);
            handle->Release();
        }
    }
}

// 0x1007c440 ?GetProperty@MfPoApp@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoApp::GetProperty(const MfString &name, MfScDataHandle **value)
{
    *value = 0;
    if (this == poExemplar)
        return MfScDataObject::GetProperty(name, value);
    return poExemplar->GetProperty(name, value);
}

// 0x1007c480 ?SetProperty@MfPoApp@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoApp::SetProperty(MfScDataHandle *self, const MfString &name,
                                                         MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return (MfPropertyErrorCode)0;
}

// 0x1007c4d0 ?PMOpenURL@MfPoApp@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// OpenURL(url, target)
MfScDataHandle *MfPoApp::PMOpenURL(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("App.OpenURL()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x100) && args[1]->IsType(0x100)) {
            MfScObjString *url = (MfScObjString *)args[0]->GetData();
            MfScObjString *target = (MfScObjString *)args[1]->GetData();
            MfURLHookData data;
            data.url = *(MfString *)url;
            data.target = *(MfString *)target;
            gHookManager->CallHooks('WURL', &data);
        } else
            InvalidType(MfString("App"), MfString("OpenURL"), 0, nArgs, args, &typeid(MfScObjString));
    }
    return 0;
}

// 0x1007c640 ??1MfURLHookData@@QAE@XZ (compiler-generated)

// 0x1007c690 ?PMShutdown@MfPoApp@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoApp::PMShutdown(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    gHookManager->CallHooks('SdAP', 0);
    return 0;
}

// 0x1007c6b0 ?PMUserEvent@MfPoApp@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// UserEvent(event, data): MF_HOOK_USER_EVENT
MfScDataHandle *MfPoApp::PMUserEvent(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (!CheckNull(MfString("App.UserEvent()"), nArgs, args, 0)) {
        if (args[0]->IsType(0x100) && args[1]->IsType(0x100)) {
            MfScObjString *event = (MfScObjString *)args[0]->GetData();
            MfScObjString *eventData = (MfScObjString *)args[1]->GetData();
            MfUserHookData data;
            data.event = *(MfString *)event;
            data.data = *(MfString *)eventData;
            gHookManager->CallHooks(MF_HOOK_USER_EVENT, &data);
        } else
            InvalidType(MfString("App"), MfString("UserEvent"), 0, nArgs, args, &typeid(MfScObjString));
    }
    return 0;
}

// 0x1007c820 ??1MfUserHookData@@QAE@XZ (compiler-generated)
