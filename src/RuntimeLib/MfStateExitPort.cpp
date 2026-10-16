// MfStateExitPort (RuntimeLib.dll 100868b0-10086f00).
#include <iostream.h>
#include "MfStateExitPort.h"
#include "MfState.h"
#include "MfStateAction.h"
#include "MfStateDisplayPos.h"
#include "MfFiniteStateMachine.h"
#include "MfScDataHandle.h"
#include "MfScVirtualMachine.h"
#include "MfScThread.h"
#include "MfScVmEnv.h"

// 0x100868b0 ??0MfStateExitPort@@QAE@VMfString@@HPAVMfStatePropertyList@@@Z
MfStateExitPort::MfStateExitPort(MfString name, int slot, MfStatePropertyList *properties)
    : mTransition(0), mAction(0), mSide(1000)
{
    mName = name;
    mProperties = properties;
    mType = TYPE;
    SetSlot(slot);
}

// 0x10086970 ??1MfStateExitPort@@UAE@XZ
MfStateExitPort::~MfStateExitPort()
{
    if (mTransition)
        delete mTransition;
    mTransition = 0;
    if (mAction)
        delete mAction;
    mAction = 0;
}

// 0x100869f0 ?Clone@MfStateExitPort@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateExitPort::Clone()
{
    MfStatePropertyList *properties = new MfStatePropertyList(0);
    MfStateExitPort *copy = new MfStateExitPort(mName, mSlot, properties);
    copy->SetProperties(mProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10086ad0 ?FormatForExecution@MfStateExitPort@@UAEXPAVMfFiniteStateMachine@@@Z
void MfStateExitPort::FormatForExecution(MfFiniteStateMachine *machine)
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
            cout << "MfStateExitPort::FormatForExecution(): Unknown transition type:" << p->mType << endl;
            break;
        }
    }
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->FormatForExecution(mMachine);
}

// EQUIV 0x10086b90 ?Exit@MfStateExitPort@@QAEPAVMfState@@PAVMfScVirtualMachine@@@Z: reviewed: register choice only, every instruction and stack slot agrees (ebx and ebp swapped: the original keeps ExecuteFunction in ebx and GetThread in ebp)
MfState *MfStateExitPort::Exit(MfScVirtualMachine *vm)
{
    MfState *next = 0;
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
        if (((MfState *)mParent)->GetExitFunction()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mParent->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(((MfState *)mParent)->GetExitFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        next = mTransition->Take(vm);
    }
    return next;
}

// 0x10086c70 ?Write@MfStateExitPort@@UAEXAAVostream@@AAHH@Z
void MfStateExitPort::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    indent++;
    os << tabs;
    os << "exit_port " << mName << " " << mSide << endl;
    os << tabs;
    os << "{" << endl;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Write(os, indent, mode);
    os << tabs;
    os << "}" << endl;
    indent--;
}

// 0x10086dc0 ?SetPath@MfStateExitPort@@UAEXVMfString@@@Z
void MfStateExitPort::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->SetPath(mPath);
}

// 0x10086e70 ?SetSlot@MfStateExitPort@@QAEXH@Z
void MfStateExitPort::SetSlot(int slot)
{
    mSlot = slot;
    if (slot > 999) {
        mSide = slot;
        return;
    }
    if (slot >= 32) {
        cout << "WARNING: MfFSMMutStateExitPort: My slot number is too high" << endl;
        mSlot = 31;
    }
    if (mSlot < 8)
        mSide = 1001;
    else if (mSlot < 16)
        mSide = 1002;
    else if (mSlot < 24)
        mSide = 1003;
    else if (mSlot < 32)
        mSide = 1004;
}

// the slots that pass a call on to all properties (the image keeps the
// copies of MfState, MfStateChoicePoint and MfStateEntryPort)
MfScVariableStack *MfStateExitPort::GetVariableStack()
{
    return mParent->GetVariableStack();
}

void MfStateExitPort::Load(MfScPiccoloObjectFile *objectFile)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Load(objectFile);
}

void MfStateExitPort::Link()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Link();
}

void MfStateExitPort::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Compile(objectFile, errors, failures);
}

void MfStateExitPort::Unload()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Unload();
}
