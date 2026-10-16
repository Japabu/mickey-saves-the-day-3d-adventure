// MfPoEvent and MfPmhUser (MfPoLibrary.dll 0x1002a980-0x1002bdc0; the inline
// members are emitted first by MfPoActor.cpp)
#include "MfPoEvent.h"
#include "MfPmhUser.h"
#include "MfAiActor.h"
#include "MfScObjString.h"
#include "MfPoActor.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScHandleDict.h"
#include "MfScPropTokenTable.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include "MfStringList.h"


// 0x1002a980 _$E28
// 0x1002a990 _$E25
// 0x1002a9b0 _$E27
// 0x1002a9c0 _$E26
static MfString strClassName("Event");
MfPoEvent *MfPoEvent::poExemplar;
MfScPropTokenTable *MfPoEvent::pttPropTokens;
MfScPropTokenTable *MfPmhUser::pttPropTokens;

// 0x1002a9d0 ??0MfPoEvent@@IAE@VExemplar@MfScData@@@Z
MfPoEvent::MfPoEvent(MfScData::Exemplar) : MfScObject(0), fsEventType("invalid_type"), pmhHandler(0)
{
    uiFlags = (uiFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    SetProperty(0, "Event", MfScDataHandle::CreateData((MfScMethod)&MfPoEvent::PMEvent, "Event::Event", 1), 1);
    SetProperty(0, "Copy", MfScDataHandle::CreateData((MfScMethod)&MfPoEvent::PMCopy, "Event::Copy", 0), 1);
    SetProperty(0, "Test", MfScDataHandle::CreateData((MfScMethod)&MfPoEvent::PMTest, "Event::Test", 1), 1);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("type", 0);
    pttPropTokens->Add("sender", 1);
    pttPropTokens->Add("id", 2);

    MfScClassInfo *ci = new MfScClassInfo("Event");
    *ci + new MfScCiMethod("Copy");
    *ci + new MfScCiProperty("type");
    *ci + new MfScCiProperty("sender");
    *ci + new MfScCiProperty("id");
    MfScDataObject::AddClassInfo(ci);
}

// 0x1002b030 ??0MfPoEvent@@QAE@ABVMfString@@PAVMfPiccoloMessageHandler@@@Z
MfPoEvent::MfPoEvent(const MfString &type, MfPiccoloMessageHandler *handler)
    : MfScObject(0), fsEventType(type), pmhHandler(handler)
{
}

// 0x1002b0a0 ??0MfPoEvent@@QAE@ABVMfString@@PAVMfAiActor@@@Z
MfPoEvent::MfPoEvent(const MfString &type, MfAiActor *receiver) : MfScObject(0), fsEventType(type)
{
    pmhHandler = new MfPmhUser(receiver);
}

// 0x1002b130 ??1MfPoEvent@@UAE@XZ
MfPoEvent::~MfPoEvent()
{
    if (pmhHandler)
        delete pmhHandler;
    if (this == poExemplar) {
        delete pttPropTokens;
        pttPropTokens = 0;
        if (this == poExemplar)
            poExemplar = 0;
    }
}

// 0x1002b1d0 ?Initialize@MfPoEvent@@SAXXZ
void MfPoEvent::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoEvent(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1002b260 ?Shutdown@MfPoEvent@@SAXXZ
void MfPoEvent::Shutdown()
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

// 0x1002b2c0 ?InitConstants@MfPoEvent@@SA?AVMfString@@XZ
MfString MfPoEvent::InitConstants()
{
    MfString s;
    return s;
}

// 0x1002b330 ?GetProperty@MfPoEvent@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoEvent::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = PEC_NO_ERROR;
    *value = 0;
    if (this == poExemplar) {
        err = MfScDataObject::GetProperty(name, value);
    } else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:
            *value = MfScDataHandle::CreateData(fsEventType, 0);
            break;
        case 1:
            if (pmhHandler) {
                MfAiActor *sender = pmhHandler->Sender();
                if (!sender)
                    sender = pmhHandler->Receiver()->GetLastSender();
                if (sender)
                    *value = sender->GetPiccoloHandle();
            }
            break;
        case 2:
            if (pmhHandler)
                *value = MfScDataHandle::CreateData(pmhHandler->Receiver()->GetLastEventID());
            break;
        default:
            if (pmhHandler) {
                err = (MfPropertyErrorCode)pmhHandler->GetProperty(name, value);
                if (err != PEC_NO_ERROR)
                    return poExemplar->GetProperty(name, value);
            } else {
                err = poExemplar->GetProperty(name, value);
            }
            break;
        }
    }
    return err;
}

// 0x1002b440 ?SetProperty@MfPoEvent@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoEvent::SetProperty(MfScDataHandle *self, const MfString &name,
                                                           MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (pmhHandler)
        return (MfPropertyErrorCode)pmhHandler->SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return PEC_NO_SUCH_PROPERTY;
}

// 0x1002b4b0 ?FilloutPropertyNameList@MfPoEvent@@EAEXPAVMfStringList@@@Z
void MfPoEvent::FilloutPropertyNameList(MfStringList *list)
{
    if (pmhHandler)
        pmhHandler->FilloutPropertyNameList(list);
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x1002b4e0 ?GetInstanceProperties@MfPoEvent@@UAEXPAVMfStringList@@H@Z
void MfPoEvent::GetInstanceProperties(MfStringList *list, int sort)
{
    if (pmhHandler)
        pmhHandler->GetInstanceProperties(list);
    if (sort)
        list->Sort(1);
}

// 0x1002b510 ?GetClassNameA@MfPoEvent@@MAE?AVMfString@@XZ
MfString MfPoEvent::GetClassName()
{
    if (pmhHandler)
        return pmhHandler->GetClassName();
    return strClassName;
}

// EQUIV 0x1002b550 ?PMEvent@MfPoEvent@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the VM environment is loaded into eax before the pushes and moved to ecx (as PMCopy); same calls and arguments
MfScDataHandle *MfPoEvent::PMEvent(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (args[0]) {
        if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
            MfScDataHandle *actorHandle;
            bool missing = MfScThread::GetThread()->GetVmEnv()->GetVariable("actor", &actorHandle) != 0;
            if (missing) {
                MfScThread::GetThread()->GetVmEnv()->ThrowException(23, "The \"actor\" variable doesn't exist.");
            } else {
                MfPoActor *actor = actorHandle ? dynamic_cast<MfPoActor *>(actorHandle->GetData()) : 0;
                MfScObjString *type = (MfScObjString *)args[0]->GetData();
                MfPmhUser *handler = new MfPmhUser(actor->GetActorPtr());
                MfScDataHandle *result = MfScDataHandle::CreateData(new MfPoEvent(*type, handler));
                actorHandle->Release();
                return result;
            }
        } else {
            MfScThread::GetThread()->GetVmEnv()->ThrowException(11, "Event constructor parameter must be a string.");
        }
    } else {
        MfScThread::GetThread()->GetVmEnv()->ThrowException(6, "Event string cannot be nil.");
    }
    return 0;
}

// EQUIV 0x1002b780 ?PMCopy@MfPoEvent@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the VM environment is loaded into eax before the pushes and moved to ecx; same call and arguments
MfScDataHandle *MfPoEvent::PMCopy(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (this == poExemplar) {
        MfScThread::GetThread()->GetVmEnv()->ThrowException(23, "Cannot call copy on the Event class object");
        return 0;
    }
    MfScDataObject *copy = new MfScDataObject(0);
    MfScDataHandle *result = MfScDataHandle::CreateData(copy);
    MfStringList names;
    FilloutPropertyNameList(&names);
    GetInstanceProperties(&names, 0);
    if (names.GetCount() > 0) {
        MfDblListItem *pli;
        for (MfString *name = names.First(pli); name; name = names.Next(pli)) {
            MfScDataHandle *value;
            if (GetProperty(*name, &value) == PEC_NO_ERROR)
                copy->SetProperty(0, *name, value, 1);
        }
    }
    return result;
}

// EQUIV 0x1002b960 ?PMTest@MfPoEvent@@IAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z: reviewed: the VM environment is loaded into eax before the pushes and moved to ecx (as PMEvent), and the receiver actor is loaded once into ebx before the null test of the string instead of in both branches; same calls and arguments
MfScDataHandle *MfPoEvent::PMTest(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (args[0]) {
        if (args[0]->IsType(MfScDataHandle::HT_STRING)) {
            MfScDataHandle *actorHandle;
            bool missing = MfScThread::GetThread()->GetVmEnv()->GetVariable("actor", &actorHandle) != 0;
            if (missing) {
                MfScThread::GetThread()->GetVmEnv()->ThrowException(23, "The \"actor\" variable doesn't exist.");
            } else {
                MfPoActor *actor = actorHandle ? dynamic_cast<MfPoActor *>(actorHandle->GetData()) : 0;
                MfScObjString *type = (MfScObjString *)args[0]->GetData();
                MfScDataHandle *result = MfScDataHandle::CreateData(new MfPoEvent(*type->String(), actor->GetActorPtr()));
                actor->GetActorPtr()->SendEvent(actor->GetActorPtr(), result, 0);
                actorHandle->Release();
                return result;
            }
        } else {
            MfScThread::GetThread()->GetVmEnv()->ThrowException(11, "Event constructor parameter must be a string.");
        }
    } else {
        MfScThread::GetThread()->GetVmEnv()->ThrowException(6, "Event string cannot be nil.");
    }
    return 0;
}

// 0x1002b940 ??_GMfScDataObject@@UAEPAXI@Z

// 0x1002bb80 ?GetClassNameA@MfPmhUser@@EAE?AVMfString@@XZ
MfString MfPmhUser::GetClassName()
{
    return MfString("UserEvent");
}

// 0x1002bbb0 ?FilloutPropertyNameList@MfPmhUser@@EAEXPAVMfStringList@@@Z
void MfPmhUser::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x1002bbd0 ??0MfPmhUser@@QAE@PAVMfAiActor@@@Z
MfPmhUser::MfPmhUser(MfAiActor *receiver) : MfPiccoloMessageHandler(receiver), paiSender(0)
{
    pdictProperties = new MfScHandleDict;
    if (!pttPropTokens)
        pttPropTokens = new MfScPropTokenTable;
}

// 0x1002bca0 ??_GMfScHandleDict@@UAEPAXI@Z

// 0x1002bcc0 ??1MfPmhUser@@UAE@XZ
MfPmhUser::~MfPmhUser()
{
    delete pdictProperties;
}

// 0x1002bd10 ?GetProperty@MfPmhUser@@UAEHABVMfString@@PAPAVMfScDataHandle@@@Z
int MfPmhUser::GetProperty(const MfString &name, MfScDataHandle **value)
{
    *value = 0;
    if (!pdictProperties)
        return MfScDataObject::PEC_NO_PROPERTY_TABLE;
    MfScDataHandle *handle = pdictProperties->Find(name);
    if (!handle)
        return MfScDataObject::PEC_NO_SUCH_PROPERTY;
    handle->AddRef();
    *value = handle;
    return MfScDataObject::PEC_NO_ERROR;
}

// 0x1002bd50 ?SetProperty@MfPmhUser@@UAEHPAVMfScDataHandle@@ABVMfString@@0H@Z
int MfPmhUser::SetProperty(MfScDataHandle *self, const MfString &name, MfScDataHandle *value, int release)
{
    if (!pdictProperties) {
        if (release == 1 && value)
            value->Release();
        return MfScDataObject::PEC_NO_PROPERTY_TABLE;
    }
    pdictProperties->Replace(name, value, release ? MfScHandleDict::kPassRef : MfScHandleDict::kAddRef);
    return MfScDataObject::PEC_NO_ERROR;
}

// 0x1002bda0 ?GetInstanceProperties@MfPmhUser@@UAEXPAVMfStringList@@@Z
void MfPmhUser::GetInstanceProperties(MfStringList *list)
{
    MfScDataObject::CopyHandleDictNamesToList(pdictProperties, list);
}
