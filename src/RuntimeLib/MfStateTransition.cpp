// MfStateTransition (RuntimeLib.dll 10087f50-10089110).
#include <iostream.h>
#include "MfStateTransition.h"
#include "MfState.h"
#include "MfStateEntryPort.h"
#include "MfStateExitPort.h"
#include "MfStateChoicePoint.h"
#include "MfLogger.h"
#include "MfFiniteStateMachine.h"
#include "MfScDataHandle.h"
#include "MfScThread.h"
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"
#include "MfStringList.h"

// 0x10087f50 ??0MfStateTransition@@QAE@VMfString@@PAVMfTransPropList@@@Z
MfStateTransition::MfStateTransition(MfString name, MfTransPropList *properties)
    : mConnector(0), mDest(0), mExitBy(0), mEnterBy(0), mChoicePt(0), mTriggers(1),
      mTransType(0), mInitial(0), mAction(0), mSource(0), m64(0), m68(0), mListening(0), m70(0)
{
    mName = name;
    mType = TYPE;
    mTransProperties = properties;
}

// 0x10088000 ?Slot44@MfStateTransition@@UAEXVMfString@@@Z

// 0x10088030 ?Clone@MfStateTransition@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateTransition::Clone()
{
    MfTransPropList *properties = new MfTransPropList(0);
    MfStateTransition *copy = new MfStateTransition(mName, properties);
    copy->SetTransProperties(mTransProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10088100 ?SetTransProperties@MfStateTransition@@QAEXPAVMfTransPropList@@@Z
void MfStateTransition::SetTransProperties(MfTransPropList *list)
{
    MfDblListItem *pli;
    if (!mTransProperties)
        mTransProperties = new MfTransPropList(0);
    else
        mTransProperties->Clear();
    if (list) {
        for (MfFSMMutable *p = (MfFSMMutable *)list->GetFirst(pli); p; p = (MfFSMMutable *)list->GetNext(pli)) {
            MfFSMMutable *copy = p->Clone();
            mTransProperties->Append(copy);
        }
    }
}

// 0x100881c0 ??1MfStateTransition@@UAE@XZ
MfStateTransition::~MfStateTransition()
{
    if (mListening == 1 && mMachine)
        mMachine->RemoveListener(this);
    if (mInitial && mParent)
        ((MfState *)mParent)->SetInitialTransition(0);
    if (mDest)
        delete mDest;
    mDest = 0;
    if (mTransProperties)
        delete mTransProperties;
    mTransProperties = 0;
    if (mConnector)
        delete mConnector;
    mConnector = 0;
    if (mTransType)
        delete mTransType;
    mTransType = 0;
    if (mInitial)
        delete mInitial;
    mInitial = 0;
    if (mExitBy)
        delete mExitBy;
    mExitBy = 0;
    if (mEnterBy)
        delete mEnterBy;
    mEnterBy = 0;
    if (mAction)
        delete mAction;
    mAction = 0;
    if (mChoicePt)
        delete mChoicePt;
    mChoicePt = 0;
}

// 0x100882e0 ?SetParent@MfStateTransition@@UAEXPAVMfStateProperty@@@Z
void MfStateTransition::SetParent(MfStateProperty *parent)
{
    mParent = parent;
    mSource = parent;
}

// 0x100882f0 ?SetSourceState@MfStateTransition@@QAEXPAVMfStateProperty@@@Z
void MfStateTransition::SetSourceState(MfStateProperty *state)
{
    mParent = 0;
    mSource = state;
}

// EQUIV 0x10088310 ?FormatForExecution@MfStateTransition@@UAEXPAVMfFiniteStateMachine@@@Z: reviewed: order of independent instructions only (the part vtable load at offset 0 and the mTransition store at 0x18 of the same part swap)
void MfStateTransition::FormatForExecution(MfFiniteStateMachine *machine)
{
    MfDblListItem *pli;
    MfTransitionProperty *p;
    mMachine = machine;
    for (p = (MfTransitionProperty *)mTransProperties->GetFirst(pli); p; p = (MfTransitionProperty *)mTransProperties->GetNext(pli)) {
        switch (p->mType) {
        case MfTransTriggeredBy::TYPE:
            if (!mTriggers.Contains(p))
                (&mTriggers)->Append(p);
            break;
        case MfTransTrigAction::TYPE:
            mAction = (MfTransTrigAction *)p;
            break;
        case MfTransitionType::TYPE:
            mTransType = (MfTransitionType *)p;
            break;
        case MfTransDestState::TYPE:
            mDest = (MfTransDestState *)p;
            break;
        case MfVisualConnector::TYPE:
            mConnector = (MfVisualConnector *)p;
            break;
        case MfTransInit::TYPE:
            mInitial = (MfTransInit *)p;
            break;
        case MfTransExitBy::TYPE:
            mExitBy = (MfTransExitBy *)p;
            p->mTransition = this;
            break;
        case MfTransEnterBy::TYPE:
            mEnterBy = (MfTransEnterBy *)p;
            break;
        case MfTransChoicePt::TYPE:
            mChoicePt = (MfTransChoicePt *)p;
            break;
        default:
            cout << "MfStateTransition::FormatForExecution(): Unknown transition type:" << p->mType << endl;
            break;
        }
    }
    for (p = (MfTransitionProperty *)mTransProperties->GetFirst(pli); p; p = (MfTransitionProperty *)mTransProperties->GetNext(pli)) {
        p->mTransition = this;
        p->FormatForExecution(mMachine);
    }
    if (!mTransType)
        SetKind(MfTransitionType::JUMP);
}

// 0x10088440 ?GetActionFunction@MfStateTransition@@QAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfStateTransition::GetActionFunction()
{
    if (mAction)
        return mAction->mFunction;
    return 0;
}

// TODO 0x10088450 ?Take@MfStateTransition@@QAEPAVMfState@@PAVMfScVirtualMachine@@@Z: (92% similar) structure matches; ebx/ebp swapped for ExecuteFunction/GetThread as in MfStateExitPort::Exit, the result and outer stack slots swapped, and GetThread is kept in a register in the choice point branch
MfState *MfStateTransition::Take(MfScVirtualMachine *vm)
{
    MfState *entered = 0;
    MfScVmEnv *env;
    MfScVariableStack *stack;
    MfScVariableStack *outer;
    MfScDataHandle *result;
    if (mDest && mDest->mTarget) {
        if (mAction && mAction->mFunction) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        entered = mDest->mTarget->RunEntryAction(vm);
    } else if (mExitBy) {
        if (mParent && ((MfState *)mParent)->GetExitFunction()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(((MfState *)mParent)->GetExitFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        if (mAction && mAction->mFunction) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        if (mExitBy->mTarget)
            entered = ((MfStateExitPort *)mExitBy->mTarget)->Exit(vm);
    } else if (mEnterBy) {
        if (mParent && ((MfState *)mParent)->GetExitFunction()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(((MfState *)mParent)->GetExitFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        if (mAction && mAction->mFunction) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        if (mEnterBy->mTarget)
            entered = ((MfStateEntryPort *)mEnterBy->mTarget)->Enter(vm);
    } else if (mChoicePt) {
        if (mParent && ((MfState *)mParent)->GetExitFunction()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(((MfState *)mParent)->GetExitFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        MfStateTransition *next = 0;
        if (mAction && mAction->mFunction) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        MfStateChoicePoint *choicePoint = (MfStateChoicePoint *)mChoicePt->mTarget;
        if (choicePoint && (next = choicePoint->Choose(vm)) != 0)
            entered = next->Take(vm);
        if (!next) {
            MfString message("Dead end transition!");
            if (choicePoint) {
                message = "The choice point ";
                message << choicePoint->GetPath();
                message << " is not defining the transition to take!";
            }
            MfLogger::Log(2, "Conductor", "ExecuteLinks", message);
            mMachine->ShowMessage(1, message);
        }
    } else {
        MfString message("Dead end on a choice point for state transition: ");
        message << GetPath();
        MfLogger::Log(2, "Conductor", "ExecuteLinks", message);
        mMachine->ShowMessage(1, message);
    }
    if (entered)
        mListening = mMachine->AddListener(this);
    return entered;
}

// 0x10088930 ?Fire@MfStateTransition@@QAEPAVMfState@@PAVMfScVirtualMachine@@@Z
MfState *MfStateTransition::Fire(MfScVirtualMachine *vm)
{
    MfState *entered = 0;
    MfScVmEnv *env;
    MfScVariableStack *stack;
    MfScVariableStack *outer;
    MfScDataHandle *result;
    if (mDest && mDest->mTarget) {
        if (mAction && mAction->mFunction) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        entered = mDest->mTarget->EnterInitial(vm);
    } else if (mEnterBy) {
        if (mAction && mAction->GetScript()) {
            env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            stack = mSource->GetVariableStack();
            outer = env->mVarStack;
            env->mVarStack = stack;
            result = vm->ExecuteFunction(mAction->mFunction, 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
        }
        if (mEnterBy->mTarget)
            entered = ((MfStateEntryPort *)mEnterBy->mTarget)->EnterState(vm);
    }
    return entered;
}

// 0x10088a30 ?GetKind@MfStateTransition@@QAEHXZ
int MfStateTransition::GetKind()
{
    if (!mTransType) {
        cout << "WARNING: no jump type was specified for this transition." << endl;
        return 0;
    }
    return mTransType->mKind;
}

// 0x10088a60 ?SetKind@MfStateTransition@@QAEXH@Z
void MfStateTransition::SetKind(int kind)
{
    MfString name;
    if (mTransType) {
        if (kind == mTransType->mKind)
            return;
        mMachine->PartDeleted(mTransType);
        mTransProperties->Remove(mTransType, 1);
        if (mTransType)
            delete mTransType;
        mTransType = 0;
    }
    if (kind == MfTransitionType::JUMP && (!GetTarget() || mSource != GetTarget()))
        name = "jump";
    else if (kind == MfTransitionType::SELF_INTERNAL && (!GetTarget() || mSource == GetTarget()))
        name = "self_internal";
    else
        name = "self_external";
    mTransType = new MfTransitionType(name);
    mTransProperties->Append(mTransType);
    mTransType->FormatForExecution(mMachine);
    mMachine->PartAdded(mTransType);
}

// 0x10088ba0 ?GetTarget@MfStateTransition@@QAEPAVMfStateProperty@@XZ
MfStateProperty *MfStateTransition::GetTarget()
{
    if (mExitBy)
        return mExitBy->mTarget;
    if (mEnterBy)
        return mEnterBy->mTarget;
    if (mChoicePt)
        return mChoicePt->mTarget;
    if (mDest)
        return mDest->mTarget;
    return 0;
}

// 0x10088bd0 ?Write@MfStateTransition@@UAEXAAVostream@@AAHH@Z
void MfStateTransition::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    indent++;
    os << tabs;
    os << "transition " << mName << endl;
    os << tabs;
    os << "{" << endl;
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->Write(os, indent, mode);
    os << tabs;
    os << "}" << endl;
    indent--;
}

// 0x10088d00 ?SetPath@MfStateTransition@@UAEXVMfString@@@Z
void MfStateTransition::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->SetPath(mPath);
}

// 0x10088db0 ?Compile@MfStateTransition@@UAEXPAVMfScPiccoloObjectFile@@PAVMfPcErrorCallBack@@AAH@Z
void MfStateTransition::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->Compile(objectFile, errors, failures);
}

// 0x10088e00 ?Link@MfStateTransition@@UAEXXZ
void MfStateTransition::Link()
{
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->Link();
}

// 0x10088e30 ?Load@MfStateTransition@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfStateTransition::Load(MfScPiccoloObjectFile *objectFile)
{
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->Load(objectFile);
}

// 0x10088e70 ?Unload@MfStateTransition@@UAEXXZ
void MfStateTransition::Unload()
{
    MfDblListItem *pli;
    for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
        p->Unload();
}

// 0x10088ea0 ?Evaluate@MfStateTransition@@QAEHPAVMfTransTriggeredBy@@@Z
int MfStateTransition::Evaluate(MfTransTriggeredBy *trigger)
{
    MfScVirtualMachine vm;
    MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
    MfScVariableStack *stack = mSource->GetVariableStack();
    MfScVariableStack *outer = env->mVarStack;
    env->mVarStack = stack;
    MfScDataHandle *result = vm.ExecuteFunction(trigger->mCondition, 0);
    env->mVarStack = outer;
    int value = 0;
    if (result) {
        if (result->IsType(0x10))
            value = result->mLong;
        result->Release();
    }
    return value;
}

// 0x10088f50 ?GetVariableStack@MfStateTransition@@UAEPAVMfScVariableStack@@XZ
MfScVariableStack *MfStateTransition::GetVariableStack()
{
    return mSource->GetVariableStack();
}

// 0x10088f60 ?Find@MfStateTransition@@UAEPAVMfFSMMutable@@VMfString@@PAVMfStringList@@PAVMfDblListItem@@H@Z
MfFSMMutable *MfStateTransition::Find(MfString path, MfStringList *names, MfDblListItem *pos, int depth)
{
    MfString *name = 0;
    if (pos && (pos = pos->GetNext()) != 0)
        name = (MfString *)pos->GetData();
    depth++;
    if (depth < names->GetCount() - 1) {
        MfDblListItem *pli;
        for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli)) {
            MfFSMMutable *found = p->Find(path, names, pos, depth);
            if (found)
                return found;
        }
        return 0;
    }
    if (*name == GetName() && path == mPath)
        return this;
    return 0;
}

// 0x10089060 ?CollectAll@MfStateTransition@@UAEXPAVMfDblList@@@Z
void MfStateTransition::CollectAll(MfDblList *list)
{
    list->Add(this);
    if (mTransProperties) {
        MfDblListItem *pli;
        for (MfFSMMutable *p = (MfFSMMutable *)mTransProperties->GetFirst(pli); p; p = (MfFSMMutable *)mTransProperties->GetNext(pli))
            p->CollectAll(list);
    }
}
