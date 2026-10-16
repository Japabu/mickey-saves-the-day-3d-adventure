// MfStateEntryPort (RuntimeLib.dll 10085ba0-10086280).
#include <iostream.h>
#include "MfStateEntryPort.h"
#include "MfState.h"
#include "MfStateAction.h"
#include "MfStateDisplayPos.h"
#include "MfFiniteStateMachine.h"
#include "MfScDataHandle.h"
#include "MfScThread.h"
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"

// 0x10085ba0 ??0MfStateEntryPort@@QAE@ABVMfString@@HPAVMfStatePropertyList@@@Z
MfStateEntryPort::MfStateEntryPort(const MfString &name, int slot, MfStatePropertyList *properties)
    : mTransition(0), mAction(0), mSide(1000)
{
    mName = name;
    mProperties = properties;
    mType = TYPE;
    SetSlot(slot);
}

// 0x10085c20 ?GetVariableStack@MfStateEntryPort@@UAEPAVMfScVariableStack@@XZ
MfScVariableStack *MfStateEntryPort::GetVariableStack()
{
    return mParent->GetVariableStack();
}

// 0x10085c50 ??1MfStateEntryPort@@UAE@XZ
MfStateEntryPort::~MfStateEntryPort()
{
    if (mTransition)
        delete mTransition;
    mTransition = 0;
    if (mAction)
        delete mAction;
    mAction = 0;
}

// 0x10085cd0 ?Clone@MfStateEntryPort@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateEntryPort::Clone()
{
    MfStatePropertyList *properties = new MfStatePropertyList(0);
    MfStateEntryPort *copy = new MfStateEntryPort(mName, mSlot, properties);
    copy->SetProperties(mProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10085d90 ?FormatForExecution@MfStateEntryPort@@UAEXPAVMfFiniteStateMachine@@@Z
void MfStateEntryPort::FormatForExecution(MfFiniteStateMachine *machine)
{
    MfDblListItem *pli;
    MfStateProperty *p;
    mMachine = machine;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        switch (p->mType) {
        case MfStateDisplayPos::TYPE:
            mDisplayPos = p;
            break;
        case MfStateTransition::TYPE:
            mTransition = (MfStateTransition *)p;
            mTransition->SetSourceState(this);
            break;
        case MfStateEntryAction::TYPE:
            mAction = (MfStateEntryAction *)p;
            mAction->mOwner = this;
            break;
        default:
            cout << "MfStateEntryPort::FormatForExecution(): Unknown transition type:" << p->mType << endl;
            break;
        }
    }
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->FormatForExecution(mMachine);
}

// EQUIV 0x10085e50 ?Enter@MfStateEntryPort@@QAEPAVMfState@@PAVMfScVirtualMachine@@@Z: reviewed: register and stack slot choice only (the original keeps ExecuteFunction in ebp and spills the outer variable stack, ours keeps GetThread in ebp); same calls, arguments and order
MfState *MfStateEntryPort::Enter(MfScVirtualMachine *vm)
{
    MfScVmEnv *env;
    MfScVariableStack *stack;
    MfScVariableStack *outer;
    MfScDataHandle *result;
    if (mAction && mAction->mFunction) {
        env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        stack = mParent->GetVariableStack();
        outer = env->mVarStack;
        env->mVarStack = stack;
        result = vm->ExecuteFunction(mAction->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
    }
    if (mTransition) {
        if (((MfState *)mParent)->GetEntryFunction()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mParent->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(((MfState *)mParent)->GetEntryFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        return mTransition->Take(vm);
    }
    MfState *state = (MfState *)mParent;
    if (state->mActiveChild)
        return state->Enter(vm);
    MfState *entered = state->EnterInitial(vm);
    if (!entered)
        entered = (MfState *)mParent;
    return entered;
}

// 0x10085f40 ?EnterState@MfStateEntryPort@@QAEPAVMfState@@PAVMfScVirtualMachine@@@Z
MfState *MfStateEntryPort::EnterState(MfScVirtualMachine *vm)
{
    if (((MfState *)mParent)->GetEntryFunction()) {
        MfScDataHandle *result = vm->ExecuteFunction(((MfState *)mParent)->GetEntryFunction(), 0);
        if (result)
            result->Release();
    }
    MfState *state = (MfState *)mParent;
    MfState *entered;
    if (state->mInitialTransition)
        entered = state->mInitialTransition->Fire(vm);
    else
        entered = state;
    return entered;
}

// 0x10085fa0 ?Write@MfStateEntryPort@@UAEXAAVostream@@AAHH@Z
void MfStateEntryPort::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    indent++;
    os << tabs;
    os << "entry_port " << mName << " " << mSide << endl;
    os << tabs;
    os << "{" << endl;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Write(os, indent, mode);
    os << tabs;
    os << "}" << endl;
    indent--;
}

// 0x100860f0 ?SetSlot@MfStateEntryPort@@QAEXH@Z
void MfStateEntryPort::SetSlot(int slot)
{
    mSlot = slot;
    if (slot > 999) {
        mSide = slot;
        return;
    }
    if (slot >= 32) {
        cout << "WARNING: MfStateEntryPort: My slot number is too high" << endl;
        mSlot = 31;
    }
    if (mSlot < 8)
        mSide = 1001;
    if (mSlot >= 8 && mSlot < 16)
        mSide = 1002;
    if (mSlot >= 16 && mSlot < 24)
        mSide = 1003;
    if (mSlot >= 24 && mSlot < 32)
        mSide = 1004;
}

// 0x10086180 ?SetPath@MfStateEntryPort@@UAEXVMfString@@@Z
void MfStateEntryPort::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->SetPath(mPath);
}

// 0x10086230 ?Compile@MfStateEntryPort@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z
void MfStateEntryPort::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Compile(objectFile, errors, failures);
}

// the slots that pass a call on to all properties (the image keeps the
// copies of MfState and MfStateChoicePoint)
void MfStateEntryPort::Load(MfScPiccoloObjectFile *objectFile)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Load(objectFile);
}

void MfStateEntryPort::Link()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Link();
}

void MfStateEntryPort::Unload()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Unload();
}
