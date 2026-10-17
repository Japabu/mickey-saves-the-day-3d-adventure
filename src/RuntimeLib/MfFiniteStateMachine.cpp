// MfFiniteStateMachine, MfFSMCameraSettings, MfFSMList
// (RuntimeLib.dll 1007dda0-1007fdd0).
#include <iostream.h>
// first: MfPoBase.h declares the script class members only before MfScData.h
#include "MfPoHFSM.h"
#include "MfFiniteStateMachine.h"
#include "MfFSMList.h"
#include "MfFSMManager.h"
#include "MfMsg.h"
#include "MfHookManager.h"
#include "MfLiveActor.h"
#include "MfScThread.h"
#include "MfScVirtualMachine.h"
#include "MfScVmEnv.h"
#include "MfWorld.h"
#include "MfStringList.h"
#include "MfScData.h"
#include "MfScDataHandle.h"
#include "MfScHandleDict.h"
#include "MfScVariableStack.h"
#include "MfState.h"
#include "MfStateDisplayPos.h"
#include "MfStateTransition.h"
#include "MfScPiccoloObjectFile.h"
#include "StateScriptFlags.h"
#include "MfLogger.h"
#include <windows.h>

// 'oSTr': a behaviour is about to start (kept out of MfHookManager.h: the
// struct there changes the inline expansion in MfViewEventHandler.cpp)
struct MfBehaviorStartHookData : public MfHookData {
    MfString behavior;                                      // 0x10
    int run;                                                // 0x14
    void *userData;                                         // 0x18
};

// 0x1007dda0 ??0MfFiniteStateMachine@@QAE@ABVMfString@@PAVMfStatePropertyList@@PAVMfState@@@Z
MfFiniteStateMachine::MfFiniteStateMachine(const MfString &file, MfStatePropertyList *properties, MfState *root)
    : mCurrent(0), mStart(0), mRoot(0), mPlayBack(0), mFiring(0), mExecuting(0), mFile(file),
      mProperties(0), mVarStack(0), mNumListeners(0), mListening(0), mac(0), mb0(0), mb4(1), mb8(0)
{
    if (mFile.ContainsSubString(gBhvrExtension, 0) < 0)
        mFile << gBhvrExtension;
    if (!root)
        mRoot = new MfState("Root", properties);
    else
        mRoot = (MfState *)root->Clone();
    MfScDataHandle *self = MfScDataHandle::CreateData(new MfPoHFSM(this));
    mHandles = new MfScHandleDict;
    mHandles->Add("hfsm", self, MfScHandleDict::kAddRef);
    self->Release();
    for (int i = 0; i < mNumListeners; i++)
        mListeners[i] = 0;
}

// 0x1007dfe0 ?ShowMessage@MfFiniteStateMachine@@UAEXHVMfString@@@Z
// 0x1007dff0 ??_GMfScHandleDict@@UAEPAXI@Z

// 0x1007e030 ??1MfFiniteStateMachine@@UAE@XZ
MfFiniteStateMachine::~MfFiniteStateMachine()
{
    ClearEvents();
    if (mVarStack)
        delete mVarStack;
    mVarStack = 0;
    if (mHandles)
        delete mHandles;
    mHandles = 0;
    if (mRoot)
        delete mRoot;
    mRoot = 0;
    if (mPlayBack)
        delete mPlayBack;
    mPlayBack = 0;
}

// EQUIV 0x1007e0f0 ?Initialize@MfFiniteStateMachine@@UAEXPAVMfScVariableStack@@@Z: reviewed: order of independent instructions only (the mStart = 0 store is scheduled before the GetFirst call setup instead of after)
void MfFiniteStateMachine::Initialize(MfScVariableStack *outer)
{
    MfDblListItem *pli;
    MfState *state;
    mac = 0;
    mRoot->SortSubstates();
    MfStatePropertyList *states = &mProperties;
    states->Clear();
    CollectStates(mRoot);
    MfUserHookData data;
    data.event = mFile;
    data.userData = mManager->mActor ? (MfAiActor *)mManager->mActor : 0;
    int chosen = 0;
    if (gHookManager->CallHooks('rSTr', &data) > 0)
        chosen = 1;
    for (state = (MfState *)states->GetFirst(pli); state; state = (MfState *)states->GetNext(pli))
        state->FormatForExecution(this);
    if (chosen) {
        for (state = (MfState *)states->GetFirst(pli); state; state = (MfState *)states->GetNext(pli)) {
            if (state->mStartState)
                state->SetStartState(0);
            if (state->GetPath() == data.data)
                state->SetStartState(1);
        }
    }
    mStart = 0;
    for (state = (MfState *)states->GetFirst(pli); state; state = (MfState *)states->GetNext(pli)) {
        if (state->mStartState) {
            mStart = state;
            break;
        }
    }
    UpdatePaths();
    if (mVarStack)
        delete mVarStack;
    mVarStack = new MfScVariableStack(*outer, mHandles, -1);
    mRoot->InitVariables(mVarStack);
}

// 0x1007e2e0 ??1MfUserHookData@@QAE@XZ
// 0x1007e330 ??_GMfScVariableStack@@UAEPAXI@Z

// 0x1007e3b0 ?Start@MfFiniteStateMachine@@UAEHXZ
int MfFiniteStateMachine::Start()
{
    MfScVirtualMachine vm;
    if (mb8 == 1)
        return 0;
    mb8 = 1;
    mNumListeners = 0;
    if (!mStart && !mac) {
        mac = 1;
        mb8 = 0;
        return 0;
    }
    if (mCurrent)
        mCurrent->mbc = 0;
    mCurrent = mStart;
    mRoot->ResetActiveChild();
    mRoot->RunInitVar(&vm);
    MfBehaviorStartHookData data;
    data.run = 1;
    data.behavior = mFile;
    data.userData = mManager->mActor ? (MfAiActor *)mManager->mActor : 0;
    int run = 1;
    if (gHookManager->CallHooks('oSTr', &data) > 0)
        run = data.run;
    if (mCurrent && g_world->IsRunning()) {
        mCurrent->mbc = 1;
        if (mCurrent->GetEntryFunction() && run) {
            mExecuting = mCurrent;
            MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
            MfScVariableStack *stack = mCurrent->GetVariableStack();
            MfScVariableStack *outer = env->mVarStack;
            env->mVarStack = stack;
            MfScDataHandle *result = vm.ExecuteFunction(mCurrent->GetEntryFunction(), 0);
            if (result)
                result->Release();
            env->mVarStack = outer;
            mExecuting = 0;
        }
    }
    mb8 = 0;
    return 1;
}

// 0x1007e350 ?NumStates@MfFiniteStateMachine@@QAEHXZ
int MfFiniteStateMachine::NumStates()
{
    return mProperties.GetCount() - 1;
}

// 0x1007e360 ?CollectStates@MfFiniteStateMachine@@QAEXPAVMfState@@@Z
void MfFiniteStateMachine::CollectStates(MfState *state)
{
    mProperties.AddProperty(state);
    MfDblListItem *pli;
    MfStatePropertyList *list = &state->mStates;
    for (MfState *s = (MfState *)list->GetFirst(pli); s; s = (MfState *)list->GetNext(pli))
        CollectStates(s);
}

// 0x1007e580 ?ClearListeners@MfFiniteStateMachine@@QAEXXZ
void MfFiniteStateMachine::ClearListeners()
{
    for (int i = 0; i < mNumListeners; i++) {
        if (mListeners[i]) {
            mListeners[i]->mListening = 0;
            mListeners[i] = 0;
        }
    }
    mNumListeners = 0;
}

// 0x1007e5c0 ?RemoveListener@MfFiniteStateMachine@@UAEXPAVMfStateTransition@@@Z
void MfFiniteStateMachine::RemoveListener(MfStateTransition *listener)
{
    for (int i = 0; i < mNumListeners; i++)
        if (mListeners[i] == listener)
            mListeners[i] = 0;
}

// 0x1007e600 ?AddListener@MfFiniteStateMachine@@UAEHPAVMfStateTransition@@@Z
int MfFiniteStateMachine::AddListener(MfStateTransition *listener)
{
    if (mListening && mNumListeners < MAX_LISTENERS) {
        mListeners[mNumListeners] = listener;
        mNumListeners++;
        return 1;
    }
    return 0;
}

// EQUIV 0x1007e640 ?HandleEvent@MfFiniteStateMachine@@QAEPAVMfStateTransition@@PAVMfMsg@@@Z: reviewed: register choice only: the original keeps 0 in edi and reloads this from its slot for every mFiring store, VC6 here keeps this in edi (reloading it after the inner loop reuses edi) and 0 in ebx; same loads, compares, calls and stores in the same order
MfStateTransition *MfFiniteStateMachine::HandleEvent(MfMsg *msg)
{
    MfStateTransition *fired = 0;
    int triggered = 0;
    MfState *state = mCurrent;
    if (!state) {
        Start();
        state = mCurrent;
        if (!state)
            return 0;
    }
    if (mListening)
        ClearListeners();
    do {
        MfDblListItem *pli;
        MfStatePropertyList *transitions = &state->mTransitions;
        for (MfStateTransition *t = (MfStateTransition *)transitions->GetFirst(pli); t;
             t = (MfStateTransition *)transitions->GetNext(pli)) {
            mFiring = t;
            MfDblListItem *tpli;
            MfTransPropList *triggers = &t->mTriggers;
            for (MfTransTriggeredBy *trigger = (MfTransTriggeredBy *)triggers->GetFirst(tpli); trigger;
                 trigger = (MfTransTriggeredBy *)triggers->GetNext(tpli)) {
                if (trigger->mEvent.type == msg->mID && trigger->mEvent.sub == msg->m38) {
                    triggered = 1;
                    if (!trigger->mCondition || t->Evaluate(trigger) == 1) {
                        FireTransition(t);
                        fired = t;
                        goto done;
                    }
                    triggered = 0;
                }
            }
        }
done:
        mFiring = 0;
        if (triggered)
            break;
        state = (MfState *)state->mParent;
    } while (state);
    mFiring = 0;
    return fired;
}

// 0x1007e760 ?SetCurrentState@MfFiniteStateMachine@@UAEPAVMfState@@PAV2@@Z
MfState *MfFiniteStateMachine::SetCurrentState(MfState *state)
{
    MfState *previous = mCurrent;
    previous->Activate();
    previous->mbc = 0;
    mCurrent = state;
    state->mbc = 1;
    return previous;
}

// 0x1007e790 ?FireTransition@MfFiniteStateMachine@@QAEXPAVMfStateTransition@@@Z
void MfFiniteStateMachine::FireTransition(MfStateTransition *transition)
{
    MfScVirtualMachine vm;
    ClearEvents();
    if (transition->mDest && transition->mDest->mTarget) {
        MfState *target = CollectActions(mCurrent, transition);
        if (target) {
            MfDblListItem *pli;
            for (MfFSMEvent *e = (MfFSMEvent *)mEvents.GetFirst(pli); e; e = (MfFSMEvent *)mEvents.GetNext(pli)) {
                mExecuting = e->mState;
                MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
                MfScVariableStack *outer = env->mVarStack;
                env->mVarStack = e->mStack;
                MfScDataHandle *result = vm.ExecuteFunction(e->mFunction, 0);
                if (result)
                    result->Release();
                env->mVarStack = outer;
                if (!g_world->IsRunning())
                    break;
            }
            if (!transition->GetKind())
                SetCurrentState(target);
        } else {
            MfString message("This transition is not terminated: ");
            message << transition->GetPath();
            MfLogger::Log(2, mFile, "HFSM", message);
        }
    } else {
        MfState *state = mCurrent;
        if (state != transition->mParent) {
            for (;;) {
                mExecuting = state;
                if (state->GetExitFunction()) {
                    MfScVmEnv *env = ((MfScThread *)MfThread::GetThread())->mVmEnv;
                    MfScVariableStack *stack = state->GetVariableStack();
                    MfScVariableStack *outer = env->mVarStack;
                    env->mVarStack = stack;
                    MfScDataHandle *result = vm.ExecuteFunction(state->GetExitFunction(), 0);
                    if (result)
                        result->Release();
                    env->mVarStack = outer;
                    if (!g_world->IsRunning())
                        break;
                }
                if (state == transition->mParent)
                    break;
                state = (MfState *)state->mParent;
            }
        }
        if (mCurrent)
            mCurrent->Activate();
        MfState *target = transition->Take(&vm);
        if (target)
            SetCurrentState(target);
        else {
            MfString message("This transition is not terminated: ");
            message << transition->GetPath();
            MfLogger::Log(2, mFile, "HFSM", message);
        }
    }
}

// the paths CollectActions finds from a state and to the target up to the root
static MfStateProperty *sFromPath[50];
static MfStateProperty *sToPath[50];
// the internal transitions taken (only counted)
static int sInternalTaken;

// TODO 0x1007ea90 ?CollectActions@MfFiniteStateMachine@@QAEPAVMfState@@PAV2@PAVMfStateTransition@@@Z: same size and shape; the original keeps this in ebx and a zero register in ebp (later i), ours this in ebp and no zero register
MfState *MfFiniteStateMachine::CollectActions(MfState *state, MfStateTransition *transition)
{
    MfState *target = transition->mDest->mTarget;
    int numFrom = 0;
    int numTo = 0;
    int i, j, k;
    MfFSMEvent *e;
    ClearEvents();
    if (state == mRoot || target == mRoot || !target)
        return 0;
    if (state != target && state->GetAncestors(sFromPath, 0, numFrom, 50) == 1
        && target->GetAncestors(sToPath, 0, numTo, 50) == 1) {
        // the first common ancestor: sFromPath[i] == sToPath[j]
        for (i = 0; i < numFrom; i++) {
            for (j = 0; j < numTo; j++)
                if (sFromPath[i] == sToPath[j])
                    break;
            if (j < numTo)
                break;
        }
    }
    if (mListening == 1) {
        AddListener(transition);
        transition->mListening = 1;
    }
    if (transition->GetKind() == 0) {
        // the exit actions of the states left, the transition's action, the
        // entry actions of the states entered
        for (k = 0; k < i && k < numFrom; k++) {
            MfState *s = (MfState *)sFromPath[k];
            if (s->GetExitFunction()) {
                e = new MfFSMEvent(s->GetExitFunction(), s->GetVariableStack(), s);
                e->mName = s->GetName();
                mEvents.Add(e);
            }
        }
        if (transition->GetActionFunction()) {
            e = new MfFSMEvent(transition->GetActionFunction(), transition->GetVariableStack(), 0);
            e->mName = transition->GetName();
            mEvents.Add(e);
        }
        for (k = 0; k < j && k < numTo; k++) {
            MfState *s = (MfState *)sToPath[k];
            if (s->GetEntryFunction()) {
                e = new MfFSMEvent(s->GetEntryFunction(), s->GetVariableStack(), s);
                e->mName = s->GetName();
                mEvents.Add(e);
            }
        }
    } else if (transition->GetKind() == 2) {
        // a self transition: leaves and enters the state again
        if (state->GetExitFunction()) {
            e = new MfFSMEvent(state->GetExitFunction(), state->GetVariableStack(), state);
            e->mName = state->GetName();
            mEvents.Add(e);
        }
        if (transition->GetActionFunction()) {
            e = new MfFSMEvent(transition->GetActionFunction(), transition->GetVariableStack(), 0);
            e->mName = transition->GetName();
            mEvents.Add(e);
        }
        if (state->GetEntryFunction()) {
            e = new MfFSMEvent(state->GetEntryFunction(), state->GetVariableStack(), state);
            e->mName = state->GetName();
            mEvents.Add(e);
        }
    } else if (transition->GetKind() == 1) {
        // an internal transition: only its action
        if (transition->GetActionFunction()) {
            e = new MfFSMEvent(transition->GetActionFunction(), transition->GetVariableStack(), 0);
            e->mName = transition->GetName();
            sInternalTaken++;
            mEvents.Add(e);
        }
    }
    return target;
}

// 0x1007ea40 ?ClearEvents@MfFiniteStateMachine@@QAEXXZ
void MfFiniteStateMachine::ClearEvents()
{
    MfDblListItem *pli;
    MfDblList *list = &mEvents;
    for (MfFSMEvent *e = (MfFSMEvent *)list->GetFirst(pli); e; e = (MfFSMEvent *)list->GetNext(pli))
        delete e;
    list->Clear();
}

// 0x1007efd0 ?WriteObjectFile@MfFiniteStateMachine@@QAEHPAVMfPcErrorCallBack@@PAHVMfString@@@Z
int MfFiniteStateMachine::WriteObjectFile(MfPcErrorCallBack *errors, int *error, MfString file)
{
    int failures = 0;
    MfScPiccoloObjectFile objectFile(file, 1, 1, 0);
    if (!objectFile.mStatus) {
        *error = objectFile.WriteClose();
        return failures;
    }
    mRoot->Compile(&objectFile, errors, failures);
    *error = objectFile.WriteClose();
    MfString text = objectFile.ECToString();
    if (*error)
        mManager->ReportSaveError(text, file);
    return failures;
}

// 0x1007f0f0 ?Unload@MfFiniteStateMachine@@QAEXXZ
void MfFiniteStateMachine::Unload()
{
    if (mRoot)
        mRoot->Unload();
}

// TODO 0x1007f100 ?Compile@MfFiniteStateMachine@@UAEHPAVMfPcErrorCallBack@@PAH@Z: as LoadCompiled
int MfFiniteStateMachine::Compile(MfPcErrorCallBack *errors, int *error)
{
    int failures = 0;
    UpdatePaths();
    MfString objectPath = mManager->BhvrDir();
    MfString name = mFile;
    MfString behaviorPath = mManager->BhvrDir();
    behaviorPath << "\\" << mFile;
    if (name.GetCharacter(name.Length() - 5) == '.')
        name.Truncate(name.Length() - 5);
    objectPath << "\\" << name;
    objectPath << ".pobj";
    HANDLE object = CreateFile((const char *)objectPath, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    HANDLE behavior = CreateFile((const char *)behaviorPath, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (object != INVALID_HANDLE_VALUE) {
        FILETIME objectTime;
        GetFileTime(object, 0, 0, &objectTime);
        CloseHandle(object);
        if (behavior != INVALID_HANDLE_VALUE) {
            FILETIME behaviorTime;
            GetFileTime(behavior, 0, 0, &behaviorTime);
            CloseHandle(behavior);
            // the object file is used when it is not older than the behaviour (2 seconds slack)
            if (!(StateScriptFlags::iFlags & 1)
                && (mPlayBack || objectTime.dwHighDateTime > behaviorTime.dwHighDateTime
                    || objectTime.dwHighDateTime == behaviorTime.dwHighDateTime
                       && objectTime.dwLowDateTime + 20000000 > behaviorTime.dwLowDateTime)) {
                MfScPiccoloObjectFile objectFile(objectPath, 0, 1, 0);
                if (!objectFile.mError) {
                    mRoot->Load(&objectFile);
                    *error = objectFile.ReadClose();
                }
            } else
                failures = WriteObjectFile(errors, error, objectPath);
        } else {
            CloseHandle(behavior);
            MfScPiccoloObjectFile objectFile(objectPath, 0, 1, 0);
            if (!objectFile.mError) {
                mRoot->Load(&objectFile);
                *error = objectFile.ReadClose();
            }
        }
    } else {
        CloseHandle(object);
        CloseHandle(behavior);
        failures = WriteObjectFile(errors, error, objectPath);
    }
    if (failures > 0) {
        mb0 = 1;
        Unload();
        DeleteFile((char *)objectPath);
    } else
        mb0 = 0;
    return failures;
}

// TODO 0x1007f4d0 ?LoadCompiled@MfFiniteStateMachine@@QAEXXZ: the original expands MfString::Truncate inline (copy-on-write, calling the imported MfStringRef copy constructor, then MfStringRef::Truncate); see NOTES.md
void MfFiniteStateMachine::LoadCompiled()
{
    UpdatePaths();
    MfString objectPath = mManager->BhvrDir();
    MfString name = mFile;
    MfString behaviorPath = mManager->BhvrDir();
    behaviorPath << "\\" << mFile;
    if (name.GetCharacter(name.Length() - 5) == '.')
        name.Truncate(name.Length() - 5);
    objectPath << "\\" << name;
    objectPath << ".pobj";
    HANDLE object = CreateFile((const char *)objectPath, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    HANDLE behavior = CreateFile((const char *)behaviorPath, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (object != INVALID_HANDLE_VALUE) {
        CloseHandle(object);
        if (behavior != INVALID_HANDLE_VALUE) {
            CloseHandle(behavior);
            MfScPiccoloObjectFile objectFile(objectPath, 0, 1, 0);
            if (!objectFile.mError) {
                mRoot->Load(&objectFile);
                objectFile.ReadClose();
            }
        }
    }
}

// 0x1007f4a0 ?BhvrDir@MfFSMManager@@QAE?AVMfString@@XZ

// 0x1007f740 ?UpdatePaths@MfFiniteStateMachine@@QAEXXZ
void MfFiniteStateMachine::UpdatePaths()
{
    MfString path;
    MfString actorClass = mManager->ActorClass();
    MfString drive;
    MfString dir;
    MfString name;
    MfString ext;
    actorClass.SplitPath(&drive, &dir, &name, &ext);
    path << '\\';
    path << name;
    path << '\\';
    path << mFile;
    mRoot->SetPath(path);
}

// 0x1007f880 ?ActorClass@MfFSMManager@@QAE?AVMfString@@XZ

// 0x1007f8b0 ?Link@MfFiniteStateMachine@@QAEXXZ
void MfFiniteStateMachine::Link()
{
    mRoot->Link();
}

// 0x1007f8c0 ?Find@MfFiniteStateMachine@@QAEPAVMfFSMMutable@@VMfString@@PAVMfStringList@@PAVMfDblListItem@@H@Z
MfFSMMutable *MfFiniteStateMachine::Find(MfString path, MfStringList *names, MfDblListItem *pos, int depth)
{
    return mRoot->Find(path, names, pos, depth);
}

// 0x1007f940 ?RemoveStructure@MfFiniteStateMachine@@UAEXXZ
void MfFiniteStateMachine::RemoveStructure()
{
    mRoot->RemoveStructure();
}

// 0x1007f950 ?IsPlayingBack@MfFiniteStateMachine@@QAEHXZ
int MfFiniteStateMachine::IsPlayingBack()
{
    if (mPlayBack && ((MfFSMPlayBack *)mPlayBack)->mPlayback == 1)
        return 1;
    return 0;
}

// 0x1007f970 ??0MfFSMCameraSettings@@QAE@NNNN@Z
MfFSMCameraSettings::MfFSMCameraSettings(double a, double b, double c, double d)
    : mA(a), mB(b), mC(c), mD(d)
{
    mType = TYPE;
}

// Inline members this object emits:
// 0x1007f9d0 ?CountNamed@MfFSMMutable@@UAEHVMfString@@PAV1@@Z
// 0x1007f9f0 ?GetPath@MfFSMMutable@@UAEAAVMfString@@XZ
// 0x1007fa00 ?GetName@MfFSMMutable@@UAEAAVMfString@@XZ
// 0x1007fa40 ?Slot30@MfStateProperty@@UAEXHHHH@Z
// 0x1007fa50 ?Slot33@MfStateProperty@@UAEXHHHHHHHHHHHHHH@Z
// TODO 0x1007fa60 ?Slot38@MfStateProperty@@UAE?AUMfStatePropertyRect@@H@Z: the original also clears a dead local ([esp]) besides the four fields of the result

// 0x1007faa0 ??1MfFSMCameraSettings@@UAE@XZ
MfFSMCameraSettings::~MfFSMCameraSettings()
{
}

// 0x1007fab0 ?Clone@MfFSMCameraSettings@@UAEPAVMfFSMMutable@@XZ
MfFSMMutable *MfFSMCameraSettings::Clone()
{
    MfFSMCameraSettings *copy = new MfFSMCameraSettings(mA, mB, mC, mD);
    copy->mMachine = mMachine;
    return copy;
}

// 0x1007fb30 ?Write@MfFSMCameraSettings@@UAEXAAVostream@@AAHH@Z
void MfFSMCameraSettings::Write(ostream &os, int &indent, int mode)
{
    MfString tabs;
    if (mode == 1)
        return;
    for (int i = 0; i < indent; i++)
        tabs << '\t';
    os << tabs;
    os << "camera_setting{" << mA << " " << mB << " " << mC << " " << mD << "}" << endl;
}

// 0x1007fc60 ??0MfFSMList@@QAE@H@Z
MfFSMList::MfFSMList(int ownsItems)
    : mOwnsItems(ownsItems), mTrackChanges(0)
{
}

// 0x1007fc80 ?Delete@MfFSMList@@UAEPAXAAPAVMfDblListItem@@@Z
void *MfFSMList::Delete(MfDblListItem *&pli)
{
    if (mTrackChanges == 1)
        mChanged = 1;
    return MfDblList::Delete(pli);
}

// 0x1007fcc0 ??1MfFSMList@@UAE@XZ

// 0x1007fd10 ?Clear@MfFSMList@@UAEXXZ
void MfFSMList::Clear()
{
    if (mTrackChanges == 1)
        mChanged = 1;
    if (mOwnsItems == 1) {
        MfDblListItem *pli;
        for (MfFSMMutable *p = (MfFSMMutable *)GetFirst(pli); p; p = (MfFSMMutable *)GetNext(pli))
            delete p;
    }
    MfDblList::Clear();
}

// EQUIV 0x1007fd60 ?Remove@MfFSMList@@QAEPAXPAXH@Z: reviewed: when Find fails the original returns the null item pointer reloaded from its stack slot (its address goes to Delete later), VC6 here returns the constant 0; the value is the same, everything else is identical
void *MfFSMList::Remove(void *item, int keep)
{
    if (mTrackChanges == 1)
        mChanged = 1;
    MfDblListItem *pli = MfDblList::Find(item);
    if (!pli)
        return pli;
    void *removed = MfDblList::Delete(pli);
    if (mOwnsItems == 1 && removed == item && !keep) {
        if (removed)
            delete (MfFSMMutable *)removed;
        return 0;
    }
    return removed;
}

// Not claimed: in the original this member is folded with an identical function
// (100649d0, ??1MfMsgTypeName); the port needs its own definition.
MfFSMEvent::~MfFSMEvent()
{
}
