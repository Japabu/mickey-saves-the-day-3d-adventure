// MfPoHFSM (RuntimeLib.dll 0x1006c270-0x1006cd30): the Piccolo object of a
// behaviour state machine. Its properties name the state whose script runs
// and the transition being fired; SetChoiceTransition is what the script of a
// choice point calls.
#include "MfPoHFSM.h"
#include "MfFiniteStateMachine.h"
#include "MfState.h"
#include "MfStateTransition.h"
#include "MfScClassInfo.h"
#include "MfScDataHandle.h"
#include "MfScObjString.h"
#include "MfScPropTokenTable.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"
#include <typeinfo.h>

// 0x1006c270 _$E21
// 0x1006c280 _$E18
// 0x1006c2a0 _$E20
// 0x1006c2b0 _$E19
static MfString strClassName("HFSM");
MfPoHFSM *MfPoHFSM::poExemplar;
MfScPropTokenTable *MfPoHFSM::pttPropTokens;

// 0x1006c2c0 ?GetClassNameA@MfPoHFSM@@EAE?AVMfString@@XZ (folded)
MfString MfPoHFSM::GetClassName()
{
    return strClassName;
}

// 0x1006c2f0 ??0MfPoHFSM@@QAE@PAVMfFiniteStateMachine@@@Z
MfPoHFSM::MfPoHFSM(MfFiniteStateMachine *machine) : MfPoBase(1)
{
    mMachine = machine;
}

// 0x1006c320 ?GetPropTokenTable@MfPoHFSM@@EAEPAVMfScPropTokenTable@@XZ (folded)
// 0x1006c330 ??_GMfPoHFSM@@UAEPAXI@Z (folded)

// 0x1006c350 ??0MfPoHFSM@@AAE@VExemplar@MfScData@@@Z
MfPoHFSM::MfPoHFSM(MfScData::Exemplar) : MfPoBase(0), mMachine(0)
{
    mFlags = (mFlags & ~0x400) | 0x200;
    Name() = strClassName + MfString("<class>");
    poExemplar = this;

    MfScClassInfo *ci = new MfScClassInfo("HFSM");
    MfScCiMethod *m;
    SetProperty(0, "SetChoiceTransition", MfScDataHandle::CreateData((MfScMethod)&MfPoHFSM::PMSetChoiceTransition, "HFSM::SetChoiceTransition", 1), 1);
    m = new MfScCiMethod("SetChoiceTransition");
    *ci + m;
    *m + new MfScMethodParameterInfo("String transitionName", 0);
    *ci + new MfScCiProperty("state - String (read only)");
    *ci + new MfScCiProperty("transition - String (read only)");
    MfScDataObject::AddClassInfo(ci);

    pttPropTokens = new MfScPropTokenTable;
    pttPropTokens->Add("state", 0);
    pttPropTokens->Add("transition", 1);
}

// 0x1006c850 ??1MfSortedListItem@@UAE@XZ
// 0x1006c860 ??_GMfSortedListItem@@UAEPAXI@Z
// 0x1006c880 ??_GMfScCiProperty@@UAEPAXI@Z
// 0x1006c8a0 ??_GMfScClassInfo@@UAEPAXI@Z
// 0x1006c8c0 ??_GMfScCiMethod@@UAEPAXI@Z
// 0x1006c8e0 ??_GMfScMethodParameterInfo@@UAEPAXI@Z
// (local copies for the dllimport classes of the class documentation)

// 0x1006c900 ??1MfPoHFSM@@UAE@XZ
MfPoHFSM::~MfPoHFSM()
{
    if (this == poExemplar)
        poExemplar = 0;
}

// 0x1006c920 ?Initialize@MfPoHFSM@@SAXXZ
void MfPoHFSM::Initialize()
{
    if (poExemplar == 0) {
        MfScData::Exemplar exemplar;
        poExemplar = new MfPoHFSM(exemplar);
        MfScDataHandle *handle = MfScDataHandle::CreateData(poExemplar);
        handle->SetReadOnly();
        MfScVmEnv::SetGlobalVariable(strClassName, handle, 1, 0);
    }
}

static int bShutdown;

// 0x1006c9b0 ?Shutdown@MfPoHFSM@@SAXXZ (folded)
void MfPoHFSM::Shutdown()
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

// 0x1006ca10 ?GetProperty@MfPoHFSM@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@ABVMfString@@PAPAVMfScDataHandle@@@Z
MfScDataObject::MfPropertyErrorCode MfPoHFSM::GetProperty(const MfString &name, MfScDataHandle **value)
{
    MfPropertyErrorCode err = (MfPropertyErrorCode)0;
    *value = 0;
    if (this == poExemplar)
        err = MfScDataObject::GetProperty(name, value);
    else {
        switch (LookupPropertyToken(name, pttPropTokens)) {
        case 0:     // state
            if (mMachine->mExecuting)
                *value = MfScDataHandle::CreateData(mMachine->mExecuting->GetName(), 0);
            break;
        case 1:     // transition
            if (mMachine->mFiring)
                *value = MfScDataHandle::CreateData(mMachine->mFiring->GetName(), 0);
            break;
        default:
            err = poExemplar->GetProperty(name, value);
        }
    }
    return err;
}

// 0x1006caa0 ?SetProperty@MfPoHFSM@@UAE?AW4MfPropertyErrorCode@MfScDataObject@@PAVMfScDataHandle@@ABVMfString@@0H@Z
MfScDataObject::MfPropertyErrorCode MfPoHFSM::SetProperty(MfScDataHandle *self, const MfString &name,
                                                          MfScDataHandle *value, int release)
{
    if (this == poExemplar)
        return MfScDataObject::SetProperty(self, name, value, release);
    if (release && value)
        value->Release();
    return (MfPropertyErrorCode)0;
}

// 0x1006caf0 ?PMSetChoiceTransition@MfPoHFSM@@AAEPAVMfScDataHandle@@PAV2@HPAPAV2@@Z
// SetChoiceTransition(name): the choice point takes the transition of that
// name; the script's alternative return is set to 0.
MfScDataHandle *MfPoHFSM::PMSetChoiceTransition(MfScDataHandle *self, int nArgs, MfScDataHandle **args)
{
    if (InvalidObject(mMachine, MfString("HFSM"), MfString("SetChoiceTransition")))
        return 0;
    if (CheckNull(MfString("HFSM.SetChoiceTransition()"), nArgs, args, 0))
        return 0;
    if (args[0]->IsType(0x100)) {
        MfScObjString *transition = (MfScObjString *)args[0]->GetData();
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        env->mAltReturnValue = 0;
        env->mAltReturnSet = 1;
        if (mMachine->SetChoice(*(MfString *)transition))
            return MfScDataHandle::CreateData(1);
        return MfScDataHandle::CreateData(0);
    }
    InvalidType(MfString("HFSM"), MfString("SetChoiceTransition"), 0, nArgs, args, &typeid(MfScObjString));
    return 0;
}

// 0x1006cc40 ?SetChoice@MfFiniteStateMachine@@QAEHVMfString@@@Z
int MfFiniteStateMachine::SetChoice(MfString choice)
{
    mChoice = choice;
    return 1;
}

// 0x1006cca0 ?FilloutPropertyNameList@MfPoHFSM@@EAEXPAVMfStringList@@@Z (folded)
void MfPoHFSM::FilloutPropertyNameList(MfStringList *list)
{
    MfScDataObject::FilloutPropertyNameList(list, pttPropTokens);
}

// 0x1006ccc0 ??_EMfScCiProperty@@W3AEPAXI@Z
// 0x1006ccd0 ??_EMfScCiMethod@@W3AEPAXI@Z
// 0x1006cce0 ??_EMfScClassInfo@@W7AEPAXI@Z
