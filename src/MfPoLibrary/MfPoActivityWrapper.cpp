// MfPoActivityWrapper (MfPoLibrary.dll 0x10005580-0x10006280). The script
// class "ActivityWrapper" wraps an MfAiActivitySpec; its exemplar replaces
// the Actor methods SetActivity/AddActivity with versions that accept a
// wrapper (and calls the saved originals for anything else).
#include <windows.h>
#include <typeinfo.h>
#include "MfPoActivityWrapper.h"
#include "MfPoActor.h"
#include "MfAiActivity.h"
#include "MfAiActivitySpec.h"
#include "MfAiActor.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScMethodRoutine.h"
#include "MfScPropTokenTable.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"

// 0x100055c0 _$E29
// 0x100055d0 _$E26
// 0x100055f0 _$E28
// 0x10005600 _$E27
static MfString strClassName("ActivityWrapper");

MfPoActivityWrapper *MfPoActivityWrapper::poExemplar;

// 0x10005610 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
MfString MfPoActivityWrapper::GetClassName()
{
    return strClassName;
}

// 0x10005640 ??0MfPoActivityWrapper@@QAE@XZ
MfPoActivityWrapper::MfPoActivityWrapper()
    : MfScObject(1), paisSpec(0), iOptions(0), bGetActivity(0), bSet(0), pmSetActivity(0), pmAddActivity(0)
{
}

// 0x10005670 ??0MfPoActivityWrapper@@QAE@VExemplar@MfScData@@@Z
MfPoActivityWrapper::MfPoActivityWrapper(MfScData::Exemplar)
    : MfScObject(0), paisSpec(0), iOptions(0), bGetActivity(0), bSet(0), pmSetActivity(0), pmAddActivity(0)
{
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScDataObject *actorClass = 0;
    MfScDataHandle *actorHandle = 0;
    MfScDataHandle *method;
    MfScVmEnv::GetGlobalVariable("Actor", &actorHandle);
    if (actorHandle)
        actorClass = dynamic_cast<MfScDataObject *>(actorHandle->GetData());

    actorClass->GetProperty("SetActivity", &method);
    if (method)
        pmSetActivity = (method ? dynamic_cast<MfScMethodRoutine *>(method->GetData()) : 0)->GetMethod();
    actorClass->SetProperty(0, "SetActivity",
                            MfScDataHandle::CreateData(this, (MfScMethod)&MfPoActivityWrapper::EMSetActivity,
                                                       "Actor::SetActivity", 1), 1);

    actorClass->GetProperty("AddActivity", &method);
    if (method)
        pmAddActivity = (method ? dynamic_cast<MfScMethodRoutine *>(method->GetData()) : 0)->GetMethod();
    actorClass->SetProperty(0, "AddActivity",
                            MfScDataHandle::CreateData(this, (MfScMethod)&MfPoActivityWrapper::EMAddActivity,
                                                       "Actor::AddActivity", 1), 1);

    actorClass->SetProperty(0, "InstantiatePluginActivity",
                            MfScDataHandle::CreateData(this, (MfScMethod)&MfPoActivityWrapper::EMInstantiatePluginActivity,
                                                       "Actor::InstantiatePluginActivity", 1), 1);

    MfScClassInfo *ci = new MfScClassInfo("Actor");
    MfScCiMethod *m = new MfScCiMethod("InstantiatePluginActivity");
    *ci + m;
    *m + new MfScMethodParameterInfo("PluginActivity activity", 0);
    MfScDataObject::AddClassInfo(ci);

    if (actorHandle)
        actorHandle->Release();
}

// 0x10005b20 ??0MfPoActivityWrapper@@IAE@VExemplarChild@@@Z
MfPoActivityWrapper::MfPoActivityWrapper(ExemplarChild)
    : MfScObject(0), paisSpec(0), iOptions(0), bGetActivity(0), bSet(0), pmSetActivity(0), pmAddActivity(0)
{
}

// 0x10005b50 ??1MfPoActivityWrapper@@UAE@XZ
MfPoActivityWrapper::~MfPoActivityWrapper()
{
    if (paisSpec && !bSet)
        paisSpec->Release();
    if (poExemplar == this) {
        poExemplar = 0;
        MfScDataObject *actorClass = 0;
        MfScDataHandle *actorHandle = 0;
        MfScVmEnv::GetGlobalVariable("Actor", &actorHandle);
        if (actorHandle)
            actorClass = dynamic_cast<MfScDataObject *>(actorHandle->GetData());
        if (actorClass) {
            MfScDataHandle *h = MfScDataHandle::CreateData(pmSetActivity, "Actor::SetActivity", 1);
            actorClass->SetProperty(actorHandle, "SetActivity", h, 1);
            h = MfScDataHandle::CreateData(pmAddActivity, "Actor::AddActivity", 1);
            actorClass->SetProperty(actorHandle, "AddActivity", h, 1);
        }
        if (actorHandle)
            actorHandle->Release();
    }
}

// 0x10005d30 ?Initialize@MfPoActivityWrapper@@SAXXZ
void MfPoActivityWrapper::Initialize()
{
    if (!poExemplar) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoActivityWrapper(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

// 0x10005dc0 ?Shutdown@MfPoActivityWrapper@@SAXXZ
void MfPoActivityWrapper::Shutdown()
{
    if (poExemplar) {
        MfScDataHandle *handle;
        MfScVmEnv::GetGlobalVariable(strClassName, &handle);
        if (handle) {
            handle->SetReadWrite();
            MfScVmEnv::SetGlobalVariable(strClassName, 0, 1, 0);
            handle->Release();
        }
    }
}

// 0x10005e20 ?ExceptionMsg@MfPoActivityWrapper@@IAEXVMfString@@@Z
void MfPoActivityWrapper::ExceptionMsg(MfString msg)
{
    MfScVmEnv *env = MfScThread::GetThread()->GetVmEnv();
    MfString text((const char *)msg);
    env->ThrowException(12, text);
}

// 0x10005eb0 ?SetSpecObject@MfPoActivityWrapper@@IAEXPAVMfAiActivitySpec@@H@Z
void MfPoActivityWrapper::SetSpecObject(MfAiActivitySpec *spec, int shared)
{
    if (paisSpec && !bSet)
        paisSpec->Release();
    paisSpec = spec;
    if (spec && !shared)
        spec->AddRef();
}

// 0x10005ef0 ?EMSetActivity@MfPoActivityWrapper@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivityWrapper::EMSetActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfPoActivityWrapper *wrapper = args[0] ? dynamic_cast<MfPoActivityWrapper *>(args[0]->GetData()) : 0;
    if (!wrapper) {
        MfScData *actor = self ? dynamic_cast<MfScData *>(self->GetData()) : 0;
        return (actor->*pmSetActivity)(self, nArgs, args);
    }
    if (wrapper->bSet) {
        ExceptionMsg("You can not set an activity twice!!");
        return MfScDataHandle::CreateData(0);
    }
    wrapper->bSet = 1;
    MfAiActor *actor = (self ? dynamic_cast<MfPoActor *>(self->GetData()) : 0)->GetActorPtr();
    if (!wrapper->paisSpec)
        return MfScDataHandle::CreateData(0);
    MfAiActivity *activity = actor->CreatePluginActivity(wrapper->paisSpec, wrapper->iOptions, 1);
    actor->SetActivity(activity);
    return activity->GetPiccoloHandle();
}

// 0x10006040 ?EMAddActivity@MfPoActivityWrapper@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivityWrapper::EMAddActivity(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    MfPoActivityWrapper *wrapper = args[0] ? dynamic_cast<MfPoActivityWrapper *>(args[0]->GetData()) : 0;
    if (!wrapper) {
        MfScData *actor = self ? dynamic_cast<MfScData *>(self->GetData()) : 0;
        return (actor->*pmAddActivity)(self, nArgs, args);
    }
    if (wrapper->bSet) {
        ExceptionMsg("You can not add an activity twice!!");
        return MfScDataHandle::CreateData(0);
    }
    wrapper->bSet = 1;
    MfAiActor *actor = (self ? dynamic_cast<MfPoActor *>(self->GetData()) : 0)->GetActorPtr();
    if (!wrapper->paisSpec)
        return MfScDataHandle::CreateData(0);
    MfAiActivity *activity = actor->CreatePluginActivity(wrapper->paisSpec, wrapper->iOptions, 1);
    actor->AddActivity(activity);
    return activity->GetPiccoloHandle();
}

// 0x10006190 ?EMInstantiatePluginActivity@MfPoActivityWrapper@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
MfScDataHandle *MfPoActivityWrapper::EMInstantiatePluginActivity(MfScDataHandle *self, int nArgs,
                                                                 MfScDataHandle **args)
{
    MfPoActivityWrapper *wrapper = args[0] ? dynamic_cast<MfPoActivityWrapper *>(args[0]->GetData()) : 0;
    if (!wrapper) {
        ExceptionMsg("You can only instantiate plugin activities!!");
        return MfScDataHandle::CreateData(0);
    }
    if (wrapper->bSet) {
        ExceptionMsg("You can not instantiate an plugin activity twice!!");
        return MfScDataHandle::CreateData(0);
    }
    wrapper->bSet = 1;
    MfAiActor *actor = (self ? dynamic_cast<MfPoActor *>(self->GetData()) : 0)->GetActorPtr();
    if (!wrapper->paisSpec)
        return MfScDataHandle::CreateData(0);
    return actor->CreatePluginActivity(wrapper->paisSpec, wrapper->iOptions, 1)->GetPiccoloHandle();
}

// Also matched by this code:
// plugins/psccontentinfo.dll 0x10004fc0 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// plugins/psccontentinfo.dll 0x10006500 ??_EMfScClassInfo@@W7AEPAXI@Z
// plugins/pscstdlib.dll 0x10001290 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// plugins/pscstdlib.dll 0x10003f60 ??_EMfScClassInfo@@W7AEPAXI@Z
// plugins/pscstdlib.dll 0x10003ff0 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// plugins/pscstdlib.dll 0x10004f00 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// plugins/pscstdlib.dll 0x100057a0 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// plugins/pscstdlib.dll 0x10007860 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x1006c2c0 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x1006cce0 ??_EMfScClassInfo@@W7AEPAXI@Z
// runtimelib.dll 0x1006cd80 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x1006f7c0 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x10070e50 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x10071b70 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x10073910 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x10078e80 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
// runtimelib.dll 0x1007bd20 ?GetClassNameA@MfPoActivityWrapper@@MAE?AVMfString@@XZ
