// MfState, MfStartProperty, MfStateBeginHistory (RuntimeLib.dll
// 10082c00-100846b0).
#include <iostream.h>
#include <windows.h>
#include "MfState.h"
#include "MfStateAction.h"
#include "MfStateDisplayPos.h"
#include "MfFiniteStateMachine.h"
#include "MfScDataHandle.h"
#include "MfScHandleDict.h"
#include "MfScThread.h"
#include "MfScVariableStack.h"
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"
#include "MfWorld.h"

// 0x10082c00 ??0MfState@@QAE@VMfString@@PAVMfStatePropertyList@@@Z
MfState::MfState(MfString name, MfStatePropertyList *properties)
    : mActiveChild(0), mHistoryStart(0), mInitialTransition(0),
      mStates(1), mTransitions(1), mExitPorts(1), mEntryPorts(1), mChoicePoints(1),
      mStartState(0), mBeginHistory(0), mEntryAction(0), mExitAction(0), mInitVar(0),
      mFunctionDef(0), mStateVariables(0), mFormatted(0), mb8(0), mbc(0), mVarStack(0),
      mVariables(0)
{
    mName = name;
    mProperties = properties;
    if (properties)
        properties->mOwnsItems = 0;
    mType = TYPE;
    mVariables = new MfScHandleDict;
}

// 0x10082d50 ?GetVariableStack@MfState@@UAEPAVMfScVariableStack@@XZ
MfScVariableStack *MfState::GetVariableStack()
{
    return mVarStack;
}

// 0x10082d80 ??1MfState@@UAE@XZ
MfState::~MfState()
{
    if (mBeginHistory)
        delete mBeginHistory;
    mBeginHistory = 0;
    if (mStartState)
        delete mStartState;
    mStartState = 0;
    if (mFunctionDef)
        delete mFunctionDef;
    mFunctionDef = 0;
    if (mInitVar)
        delete mInitVar;
    mInitVar = 0;
    if (mVarStack)
        delete mVarStack;
    mVarStack = 0;
    if (mVariables)
        delete mVariables;
    mVariables = 0;
    if (mStateVariables)
        delete mStateVariables;
    mStateVariables = 0;
    if (mEntryAction)
        delete mEntryAction;
    mEntryAction = 0;
    if (mExitAction)
        delete mExitAction;
    mExitAction = 0;
}

// 0x10082ee0 ?Clone@MfState@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfState::Clone()
{
    MfStatePropertyList *properties = new MfStatePropertyList(0);
    MfState *copy = new MfState(mName, properties);
    copy->SetProperties(mProperties);
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10082fc0 ?FormatForExecution@MfState@@UAEXPAVMfFiniteStateMachine@@@Z
void MfState::FormatForExecution(MfFiniteStateMachine *machine)
{
    MfDblListItem *pli;
    MfStateProperty *p;
    mMachine = machine;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        switch (p->mType) {
        case MfState::TYPE:
            if (mFormatted)
                break;
            p->SetParent(this);
            if (!mStates.Contains(p))
                mStates.AddProperty(p);
            break;
        case MfStateEntryAction::TYPE:
            mEntryAction = (MfStateEntryAction *)p;
            break;
        case MfStateExitAction::TYPE:
            mExitAction = (MfStateExitAction *)p;
            break;
        case 0x6d:
            p->SetParent(this);
            if (!mExitPorts.Contains(p))
                mExitPorts.AddProperty(p);
            break;
        case 0x6e:
            p->SetParent(this);
            if (!mEntryPorts.Contains(p))
                mEntryPorts.AddProperty(p);
            break;
        case 0x6f:
            p->SetParent(this);
            if (!mChoicePoints.Contains(p))
                mChoicePoints.AddProperty(p);
            break;
        case MfStateVariables::TYPE:
            mStateVariables = (MfStateVariables *)p;
            break;
        case MfStateTransition::TYPE:
            p->SetParent(this);
            ((MfStateTransition *)p)->SetSourceState(this);
            if (!mTransitions.Contains(p))
                mTransitions.AddProperty(p);
            break;
        case MfStateDisplayPos::TYPE:
            mDisplayPos = p;
            break;
        case MfStateFunctionDef::TYPE:
            mFunctionDef = (MfStateFunctionDef *)p;
            p->SetParent(this);
            break;
        case MfStartProperty::TYPE:
            mStartState = p;
            break;
        case MfStateBeginHistory::TYPE:
            mBeginHistory = p;
            if (mParent)
                ((MfState *)mParent)->SetHistoryStart(this);
            break;
        case MfStateInitVar::TYPE:
            p->SetParent(this);
            mInitVar = (MfStateInitVar *)p;
            break;
        case MfFSMCameraSettings::TYPE:
            mMachine->SetCameraSettings(p);
            break;
        case MfFSMPlayBack::TYPE:
            mMachine->mPlayBack = p;
            break;
        case 0x72:
            break;
        default:
            cout << "Warning: MfState::FormatForExecution() Unknown state property" << endl;
            break;
        }
    }
    mFormatted = 1;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        p->SetParent(this);
        p->FormatForExecution(mMachine);
    }
    MfStatePropertyList *list = &mTransitions;
    for (MfStateTransition *t = (MfStateTransition *)list->GetFirst(pli); t; t = (MfStateTransition *)list->GetNext(pli)) {
        if (t->mInitial) {
            mInitialTransition = t;
            return;
        }
    }
}

// 0x100831e0 ?SortSubstates@MfState@@QAEXXZ
void MfState::SortSubstates()
{
    MfDblListItem *pli;
    MfStateProperty *p;
    mFormatted = 1;
    for (p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        if (p->mType == MfState::TYPE && !mStates.Contains(p)) {
            p->SetParent(this);
            mStates.AddProperty(p);
        }
    }
    MfStatePropertyList *list = &mStates;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        p->SetParent(this);
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        ((MfState *)p)->SortSubstates();
}

// 0x100832a0 ?GetEntryFunction@MfState@@QAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfState::GetEntryFunction()
{
    if (mEntryAction)
        return mEntryAction->mFunction;
    return 0;
}

// 0x100832c0 ?GetExitFunction@MfState@@QAEPAVMfScDataHandle@@XZ
MfScDataHandle *MfState::GetExitFunction()
{
    if (mExitAction)
        return mExitAction->mFunction;
    return 0;
}

// 0x100832e0 ?SetActiveChild@MfState@@QAEXPAV1@@Z
void MfState::SetActiveChild(MfState *child)
{
    mActiveChild = child;
}

// 0x100832f0 ?SetHistoryStart@MfState@@QAEXPAV1@@Z
void MfState::SetHistoryStart(MfState *child)
{
    mHistoryStart = child;
}

// 0x10083300 ?Activate@MfState@@QAEXXZ
void MfState::Activate()
{
    if (mParent) {
        ((MfState *)mParent)->SetActiveChild(this);
        ((MfState *)mParent)->Activate();
    }
}

// 0x10083320 ?RunEntryAction@MfState@@QAEPAV1@PAVMfScVirtualMachine@@@Z
MfState *MfState::RunEntryAction(MfScVirtualMachine *vm)
{
    if (mEntryAction && mEntryAction->mFunction) {
        mMachine->mExecuting = this;
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        MfScDataHandle *result = vm->ExecuteFunction(mEntryAction->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
        mMachine->mExecuting = 0;
    }
    return this;
}

// 0x10083390 ?GetAncestors@MfState@@QAEHPAPAVMfStateProperty@@PAV1@AAHH@Z
int MfState::GetAncestors(MfStateProperty **path, MfState *stop, int &count, int max)
{
    MfStateProperty *state = this;
    count = 0;
    while (1) {
        path[count] = state;
        state = state->mParent;
        count++;
        if (state == mMachine->mRoot) {
            path[count] = mMachine->mRoot;
            count++;
            return 1;
        }
        if (state == stop) {
            path[count] = stop;
            count++;
            return 1;
        }
        if (count == max)
            return 0;
    }
}

// 0x10083400 ?Enter@MfState@@QAEPAV1@PAVMfScVirtualMachine@@@Z
MfState *MfState::Enter(MfScVirtualMachine *vm)
{
    MfState *entered = mHistoryStart ? mHistoryStart : mActiveChild;
    if (mEntryAction && mEntryAction->mFunction) {
        mMachine->mExecuting = this;
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        MfScDataHandle *result = vm->ExecuteFunction(mEntryAction->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
        mMachine->mExecuting = 0;
    }
    if (mHistoryStart || mActiveChild) {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        if (mHistoryStart)
            entered = mHistoryStart->Enter(vm);
        else
            entered = mActiveChild->Enter(vm);
        env->mVarStack = outer;
        if (!entered) {
            if (!mHistoryStart)
                entered = mActiveChild;
            else
                entered = mHistoryStart;
        }
    }
    return entered;
}

// 0x100834e0 ?EnterInitial@MfState@@QAEPAV1@PAVMfScVirtualMachine@@@Z
MfState *MfState::EnterInitial(MfScVirtualMachine *vm)
{
    MfState *entered = 0;
    if (mEntryAction && mEntryAction->mFunction) {
        mMachine->mExecuting = this;
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        MfScDataHandle *result = vm->ExecuteFunction(mEntryAction->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
        mMachine->mExecuting = 0;
    }
    if (mInitialTransition) {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        entered = mInitialTransition->Fire(vm);
        env->mVarStack = outer;
        if (!entered)
            entered = this;
    } else if (mHistoryStart) {
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        if (mHistoryStart)
            entered = mHistoryStart->EnterInitial(vm);
        env->mVarStack = outer;
        if (!entered)
            entered = mHistoryStart;
    } else
        entered = this;
    return entered;
}

// 0x100835e0 ?SetStartState@MfState@@QAEXH@Z
void MfState::SetStartState(int start)
{
    if (!start) {
        if (mStartState)
            RemovePart(mStartState, 0);
    } else if (!mStartState)
        AddPart(new MfStartProperty);
}

// 0x10083690 ?Clone@MfStartProperty@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStartProperty::Clone()
{
    MfStartProperty *copy = new MfStartProperty;
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x10083740 ??1MfStartProperty@@UAE@XZ

// 0x10083750 ?Clone@MfStateBeginHistory@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfStateBeginHistory::Clone()
{
    MfStateBeginHistory *copy = new MfStateBeginHistory;
    copy->SetFullPath(mPath);
    copy->mMachine = mMachine;
    return copy;
}

// 0x100837e0 ?CanDelete@MfState@@UAEHXZ
int MfState::CanDelete()
{
    return mMachine->mRoot != this;
}

// 0x10083800 ?Write@MfState@@UAEXAAVostream@@AAHH@Z
void MfState::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mMachine->mRoot != this) {
        for (int i = 0; i < indent; i++)
            tabs << '\t';
        indent++;
        os << tabs;
        os << "state " << mName << endl;
        os << tabs;
        os << "{" << endl;
    }
    if (mStartState)
        os << tabs << "\tstart_state" << endl;
    if (mBeginHistory)
        os << tabs << "\tbegin_hist" << endl;
    if (this == mMachine->mRoot && mode == 1) {
        MfStateProperty *playBack = mMachine->mPlayBack;
        if (!playBack)
            playBack = new MfFSMPlayBack(1);
        playBack->Write(os, indent, 1);
    }
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Write(os, indent, mode);
    if (mMachine->mRoot != this) {
        os << tabs;
        os << "}" << endl;
        indent--;
    }
}

// 0x100839e0 ?FindState@MfState@@QAEPAV1@VMfString@@@Z
MfState *MfState::FindState(MfString name)
{
    MfDblListItem *pli;
    MfStatePropertyList *list = &mStates;
    for (MfStateProperty *p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name)
            return (MfState *)p;
    return 0;
}

// 0x10083aa0 ?FindExitPort@MfState@@QAEPAVMfStateProperty@@VMfString@@@Z
MfStateProperty *MfState::FindExitPort(MfString name)
{
    MfDblListItem *pli;
    MfStatePropertyList *list = &mExitPorts;
    for (MfStateProperty *p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name)
            return p;
    return 0;
}

// 0x10083b60 ?FindEntryPort@MfState@@QAEPAVMfStateProperty@@VMfString@@@Z
MfStateProperty *MfState::FindEntryPort(MfString name)
{
    MfDblListItem *pli;
    MfStatePropertyList *list = &mEntryPorts;
    for (MfStateProperty *p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name)
            return p;
    return 0;
}

// 0x10083c20 ?FindChoicePoint@MfState@@QAEPAVMfStateProperty@@VMfString@@@Z
MfStateProperty *MfState::FindChoicePoint(MfString name)
{
    MfDblListItem *pli;
    MfStatePropertyList *list = &mChoicePoints;
    for (MfStateProperty *p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name)
            return p;
    return 0;
}

// 0x10083ce0 ?CountNamed@MfState@@UAEHVMfString@@PAVMfFSMMutable@@@Z
int MfState::CountNamed(MfString name, MfFSMMutable *except)
{
    MfDblListItem *pli;
    MfStateProperty *p;
    int count = 0;
    MfStatePropertyList *list = &mStates;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name && p != except)
            count++;
    list = &mTransitions;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name && p != except)
            count++;
    list = &mExitPorts;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name && p != except)
            count++;
    list = &mEntryPorts;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name && p != except)
            count++;
    list = &mChoicePoints;
    for (p = (MfStateProperty *)list->GetFirst(pli); p; p = (MfStateProperty *)list->GetNext(pli))
        if (p->GetName() == name && p != except)
            count++;
    return count;
}

// 0x10083eb0 ?SetInitialTransition@MfState@@QAEXPAVMfStateTransition@@@Z
void MfState::SetInitialTransition(MfStateTransition *transition)
{
    if (mInitialTransition == transition)
        return;
    if (mInitialTransition && transition)
        MessageBox(0, "It doesn't make sense to have more than ONE initial transition!", "State Error",
                   MB_ICONEXCLAMATION);
    else
        mInitialTransition = transition;
    if (mInitialTransition)
        return;
    MfDblListItem *pli;
    MfStatePropertyList *list = &mTransitions;
    for (MfStateTransition *t = (MfStateTransition *)list->GetFirst(pli); t; t = (MfStateTransition *)list->GetNext(pli)) {
        if (t->mInitial) {
            mInitialTransition = t;
            return;
        }
    }
}

// 0x10083f20 ?SetPath@MfState@@UAEXVMfString@@@Z
void MfState::SetPath(MfString parent)
{
    mPath = parent;
    mPath << '\\';
    mPath << mName;
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->SetPath(mPath);
}

// 0x10083fd0 ?InitVariables@MfState@@QAEXPAVMfScVariableStack@@@Z
void MfState::InitVariables(MfScVariableStack *outer)
{
    if (mVarStack)
        delete mVarStack;
    mVarStack = new MfScVariableStack(*outer, mVariables, -1);
    if (mStateVariables)
        mStateVariables->DefineVariables(mVariables);
    if (mFunctionDef)
        mFunctionDef->DefineFunctions(mVariables);
    MfDblListItem *pli;
    MfStatePropertyList *list = &mStates;
    for (MfState *s = (MfState *)list->GetFirst(pli); s; s = (MfState *)list->GetNext(pli))
        s->InitVariables(mVarStack);
}

// 0x100840c0 ?RunInitVar@MfState@@QAEXPAVMfScVirtualMachine@@@Z
void MfState::RunInitVar(MfScVirtualMachine *vm)
{
    if (mInitVar && mInitVar->mFunction && g_world->IsRunning()) {
        mMachine->mExecuting = this;
        MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
        MfScVariableStack *outer = env->mVarStack;
        env->mVarStack = mVarStack;
        MfScDataHandle *result = vm->ExecuteFunction(mInitVar->mFunction, 0);
        if (result)
            result->Release();
        env->mVarStack = outer;
        mMachine->mExecuting = 0;
        if (!g_world->IsRunning())
            return;
    }
    MfDblListItem *pli;
    MfStatePropertyList *list = &mStates;
    for (MfState *s = (MfState *)list->GetFirst(pli); s; s = (MfState *)list->GetNext(pli)) {
        s->RunInitVar(vm);
        if (!g_world->IsRunning())
            return;
    }
}

// 0x100841a0 ?Load@MfState@@UAEXPAVMfScPiccoloObjectFile@@@Z
void MfState::Load(MfScPiccoloObjectFile *objectFile)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Load(objectFile);
}

// 0x100841e0 ?RemoveStructure@MfState@@QAEXXZ
void MfState::RemoveStructure()
{
    MfDblListItem *pli;
    MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli);
    while (p) {
        if (p->mType != MfState::TYPE && p->mType != MfStateTransition::TYPE && p->mType != 0x6d
            && p->mType != 0x6e && p->mType != 0x6f)
            p = (MfStateProperty *)mProperties->GetNext(pli);
        else {
            UnlinkPart(p, 0);
            mProperties->Remove(pli, p, 0);
            p = (MfStateProperty *)mProperties->GetCurrent(pli);
        }
    }
}

// 0x10084260 ?RemovePart@MfState@@UAEHPAVMfStateProperty@@H@Z
int MfState::RemovePart(MfStateProperty *part, int keep)
{
    mProperties->Remove(part, keep);
    return UnlinkPart(part, keep);
}

// 0x10084290 ?UnlinkPart@MfState@@QAEHPAVMfStateProperty@@H@Z
int MfState::UnlinkPart(MfStateProperty *part, int keep)
{
    mMachine->PartDeleted(part);
    switch (part->mType) {
    case MfState::TYPE:
        mStates.Remove(part, keep);
        break;
    case MfStateEntryAction::TYPE:
        if (mEntryAction)
            delete mEntryAction;
        mEntryAction = 0;
        break;
    case MfStateInitVar::TYPE:
        if (mInitVar)
            delete mInitVar;
        mInitVar = 0;
        break;
    case MfStateExitAction::TYPE:
        if (mExitAction)
            delete mExitAction;
        mExitAction = 0;
        break;
    case MfStateTransition::TYPE:
        mTransitions.Remove(part, keep);
        break;
    case MfStateDisplayPos::TYPE:
        if (mDisplayPos)
            delete mDisplayPos;
        mDisplayPos = 0;
        break;
    case MfStateFunctionDef::TYPE:
        if (mFunctionDef)
            delete mFunctionDef;
        mFunctionDef = 0;
        break;
    case MfStartProperty::TYPE:
        mStartState = 0;
        break;
    case MfStateBeginHistory::TYPE:
        mBeginHistory = 0;
        break;
    case 0x6d:
        mExitPorts.Remove(part, keep);
        break;
    case 0x6e:
        mEntryPorts.Remove(part, keep);
        break;
    case 0x6f:
        mChoicePoints.Remove(part, keep);
        break;
    }
    return 1;
}

// 0x10084440 ?AddPart@MfState@@UAEHPAVMfStateProperty@@@Z
int MfState::AddPart(MfStateProperty *part)
{
    part->SetParent(this);
    mProperties->AddProperty(part);
    switch (part->mType) {
    case MfState::TYPE:
        if (part != mMachine->mRoot)
            mStates.AddProperty(part);
        break;
    case MfStateEntryAction::TYPE:
        mEntryAction = (MfStateEntryAction *)part;
        break;
    case MfStateInitVar::TYPE:
        mInitVar = (MfStateInitVar *)part;
        break;
    case MfStateExitAction::TYPE:
        mExitAction = (MfStateExitAction *)part;
        break;
    case MfStateVariables::TYPE:
        mStateVariables = (MfStateVariables *)part;
        break;
    case MfStateTransition::TYPE:
        mTransitions.AddProperty(part);
        part->mMachine = mMachine;
        mMachine->PartAdded(part);
        break;
    case MfStateDisplayPos::TYPE:
        mDisplayPos = part;
        break;
    case MfStateFunctionDef::TYPE:
        mFunctionDef = (MfStateFunctionDef *)part;
        break;
    case MfStartProperty::TYPE:
        mStartState = part;
        break;
    case MfStateBeginHistory::TYPE:
        mBeginHistory = part;
        break;
    case 0x6d:
        mExitPorts.AddProperty(part);
        break;
    case 0x6e:
        mEntryPorts.AddProperty(part);
        break;
    case 0x6f:
        mChoicePoints.AddProperty(part);
        break;
    }
    part->SetPath(mPath);
    mMachine->PartAdded(part);
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        p->SetParent(this);
        p->FormatForExecution(mMachine);
    }
    return 1;
}

// 0x10084590 ?Slot17@MfState@@UAEXH@Z
void MfState::Slot17(int a)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli)) {
        if (p->mType == MfStateTransition::TYPE)
            ((MfStateTransition *)p)->Slot52(0, a);
        else
            p->Slot17(a);
    }
}

// 0x100845f0 ?FirstSubstate@MfState@@QAEPAV1@XZ
MfState *MfState::FirstSubstate()
{
    MfDblListItem *pli;
    MfStatePropertyList *list = &mStates;
    return (MfState *)list->GetFirst(pli);
}

// 0x10084610 ?ResetActiveChild@MfState@@QAEPAV1@XZ
MfState *MfState::ResetActiveChild()
{
    MfDblListItem *pli;
    mActiveChild = 0;
    return (MfState *)(&mStates)->GetFirst(pli);
}

// the slots that pass a call on to all properties (the copies of
// MfStateChoicePoint's and MfStateEntryPort's are the ones in the image)
void MfState::Link()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Link();
}

void MfState::Compile(MfScPiccoloObjectFile *objectFile, MfPcErrorCallBack *errors, int &failures)
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Compile(objectFile, errors, failures);
}

void MfState::Unload()
{
    MfDblListItem *pli;
    for (MfStateProperty *p = (MfStateProperty *)mProperties->GetFirst(pli); p; p = (MfStateProperty *)mProperties->GetNext(pli))
        p->Unload();
}
