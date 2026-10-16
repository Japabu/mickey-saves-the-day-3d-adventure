// MfFSMManager (RuntimeLib.dll 1007fdd0-10082260).
#include "MfFSMManager.h"
#include "MfFiniteStateMachine.h"
#include "MfFSMList.h"
#include "MfScDataHandle.h"
#include "MfScHandleDict.h"
#include "MfScVariableStack.h"
#include "MfMsgDispatcher.h"
#include "MfLiveActor.h"
#include "MfProtoActor.h"
#include "MfState.h"
#include "MfStringList.h"
#include "MfLogger.h"
#include "StateScriptFlags.h"
#include "MfStateCompiler.h"
#include "MfInterfaceRegistry.h"
#include "MfAiCompiler.h"
#include <windows.h>
#include <fstream.h>

class MfNullString;
extern __declspec(dllimport) MfNullString NULL_STRING;

// the script value of an event message for an actor (RuntimeLib 100699d0)
MfScDataHandle *MakeEventData(MfAiActor *actor, MfMsg &msg);
#include "MfWorld.h"

// the messages the manager handles (MfMsg IDs)
enum {
    MSG_RUN_BEHAVIOR = 1,           // the queued events and timers
    MSG_START_BEHAVIOR = 59         // start the enabled machines
};

MfFSMManager::MsgHandler MfFSMManager::aMethods[NUM_MSGS];
// 0x1007fdd0 _$E73 (original $E: the static initializer of gBhvrExtension, with 1007fde0)
// 0x1007fde0 _$E70
// 0x1007fe00 _$E72
// 0x1007fe10 _$E71
MfString gBhvrExtension(".bhvr");
int MfFSMManager::bMsgTableIsSet;

// 0x1007fe20 ?Hook23@MfFSMManager@@UAEHVMfString@@@Z
// 0x1007fe30 ?Hook29@MfFSMManager@@UAEXVMfString@@0@Z

// 0x1007feb0 ??0MfFSMManager@@QAE@VMfString@@PAVMfScDataHandle@@1PAVMfLiveActor@@@Z
MfFSMManager::MfFSMManager(MfString dir, MfScDataHandle *actorHandle, MfScDataHandle *spaceHandle, MfLiveActor *actor)
    : mEditMachine(0), mBhvrDir(dir), mActor(actor), mActive(0), m44(0), mEnabled(1), mStarting(0),
      mHandling(0), mEditing(0), mCopiedState(0), mNumFired(0), mLastLoaded(0), mRunning(0), m3ac(0)
{
    InitMsgTable();
    mBehaviors = new MfFSMList(1);
    mVariables = new MfScHandleDict;
    if (actorHandle)
        mVariables->Add("actor", actorHandle, MfScHandleDict::kAddRef);
    if (spaceHandle)
        mVariables->Add("space", spaceHandle, MfScHandleDict::kAddRef);
    mVarStack = new MfScVariableStack(mVariables, 1);
}

// 0x100800e0 ?ClearBehaviors@MfFSMManager@@UAEXXZ
void MfFSMManager::ClearBehaviors()
{
    mBehaviors->Clear();
}

// 0x100800f0 ?InitMsgTable@MfFSMManager@@MAEXXZ
void MfFSMManager::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfFSMManager::DefaultMsgHandler;
        bMsgTableIsSet = 1;
        aMethods[MSG_RUN_BEHAVIOR] = &MfFSMManager::HandleRun;
        aMethods[MSG_START_BEHAVIOR] = &MfFSMManager::HandleStart;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x10080140 ?HandleRun@MfFSMManager@@IAEXAAVMfMsg@@@Z
void MfFSMManager::HandleRun(MfMsg &msg)
{
    mRunning = 1;
    RunQueuedEvents();
    RunQueuedTimers();
    Hook15();
    mRunning = 0;
}

// 0x10080170 ?HandleStart@MfFSMManager@@IAEXAAVMfMsg@@@Z
void MfFSMManager::HandleStart(MfMsg &msg)
{
    m44 = 1;
    m3ac = 0;
    if (mEnabled && mStarting != 1) {
        mStarting = 1;
        MfDblListItem *pli;
        for (MfFiniteStateMachine *m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m;
             m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli)) {
            if (m->mb4 == 1)
                m->Start();
            if (!g_world->IsRunning())
                break;
        }
        mStarting = 0;
    }
}

// 0x100820e0 ?GetName@MfFSMManager@@UAE?AVMfString@@XZ
MfString MfFSMManager::GetName()
{
    MfString name("MfFSMManagerOf");
    name << mActorName;
    return name;
}

// 0x100801f0 ?RunQueuedTimers@MfFSMManager@@UAEHXZ
int MfFSMManager::RunQueuedTimers()
{
    int ran = 0;
    MfDblListItem *pli;
    MfDblList *list = &mQueuedTimers;
    MfFSMLoadRequest *request;
    for (request = (MfFSMLoadRequest *)list->GetFirst(pli); request; request = (MfFSMLoadRequest *)list->GetNext(pli)) {
        ran = 1;
        LoadBehavior(request->name, 1, request->arg);
    }
    for (request = (MfFSMLoadRequest *)list->GetFirst(pli); request; request = (MfFSMLoadRequest *)list->GetNext(pli))
        delete request;
    list->Clear();
    return ran;
}

// 0x100802a0 ?RunQueuedEvents@MfFSMManager@@UAEHXZ
int MfFSMManager::RunQueuedEvents()
{
    int ran = 0;
    MfDblListItem *pli;
    MfDblList *list = &mQueuedEvents;
    for (void *machine = list->GetFirst(pli); machine; machine = list->GetNext(pli)) {
        ran = 1;
        ((MfFSMList *)mBehaviors)->Remove(machine, 1);
    }
    list->Clear();
    return ran;
}

// 0x100802f0 ?SetActorHandle@MfFSMManager@@QAEXPAVMfScDataHandle@@@Z
void MfFSMManager::SetActorHandle(MfScDataHandle *actor)
{
    if (actor)
        mVariables->Replace("actor", actor, MfScHandleDict::kAddRef);
}

// 0x10080360 ?SetSpaceHandle@MfFSMManager@@QAEXPAVMfScDataHandle@@@Z
void MfFSMManager::SetSpaceHandle(MfScDataHandle *space)
{
    if (space)
        mVariables->Replace("space", space, MfScHandleDict::kAddRef);
}

// 0x10080790 ?UpdateActive@MfFSMManager@@QAEXXZ
void MfFSMManager::UpdateActive()
{
    MfDblListItem *pli;
    mActive = 0;
    for (MfFiniteStateMachine *m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m;
         m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli)) {
        m->Initialize(mVarStack);
        if (m->NumStates() >= 1)
            mActive = 1;
    }
}

// 0x100807f0 ?Resume@MfFSMManager@@UAEXXZ
void MfFSMManager::Resume()
{
    m3ac = 1;
    g_msgDispatcher->Post(new MfMsg(MSG_START_BEHAVIOR, 0, 0, this, this));
}

// 0x100803d0 ??1MfFSMManager@@UAE@XZ
MfFSMManager::~MfFSMManager()
{
    Unregister();
    if (mVarStack)
        delete mVarStack;
    mVarStack = 0;
    if (mVariables)
        delete mVariables;
    mVariables = 0;
    ((MfFSMList *)mBehaviors)->mOwnsItems = 0;
    int originals = 0;
    MfDblListItem *pli;
    MfDblList *instances = mActor->Prototype()->Instances();
    for (MfActor *a = (MfActor *)instances->GetFirst(pli); a; a = (MfActor *)instances->GetNext(pli))
        if (!a->mTempCopy)
            originals++;
    MfFiniteStateMachine *m;
    if (originals == 1) {
        for (m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m; m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli))
            mActor->Prototype()->UnregCompiledBhvr(m->GetFile());
    }
    for (m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m; m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli))
        delete m;
    mBehaviors->Clear();
    if (mBehaviors)
        delete mBehaviors;
}

// 0x10080570 ?GetFile@MfFiniteStateMachine@@QAE?AVMfString@@XZ

// 0x10080870 ?GetScript@MfFSMManager@@QAE?AVMfString@@V2@@Z
MfString MfFSMManager::GetScript(MfString path)
{
    MfString *script = 0;
    MfFSMMutable *part = FindPart(path);
    MfString source;
    if (part)
        script = part->GetScript();
    if (script)
        source = *script;
    return source;
}

// EQUIV 0x10080930 ?FindPart@MfFSMManager@@QAEPAVMfFSMMutable@@VMfString@@@Z: reviewed: the second name pointer goes through ecx and then eax in the original and straight into eax here (registers only), and the unwind frame offsets differ by the larger frame; same loads, tests, calls and stores
MfFSMMutable *MfFSMManager::FindPart(MfString path)
{
    MfFSMMutable *found = 0;
    MfStringList names;
    MfFSMMutable::SplitPath(&names, path);
    MfDblListItem *pos;
    names.First(pos);
    MfString *file = names.Next(pos);
    if (names.GetCount() > 2) {
        MfDblListItem *pli;
        for (MfFiniteStateMachine *m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m;
             m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli)) {
            if (m->GetFile() == *file) {
                if (names.GetCount() > 1) {
                    found = m->Find(path, &names, pos, 1);
                    break;
                }
            }
        }
    }
    return found;
}

// 0x10080ae0 ?IsBehaviorLoaded@MfFSMManager@@QAEPAVMfFiniteStateMachine@@ABVMfString@@@Z
MfFiniteStateMachine *MfFSMManager::IsBehaviorLoaded(const MfString &name)
{
    MfDblListItem *pli;
    for (MfFiniteStateMachine *m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m;
         m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli))
        if (m->GetFile() == name)
            return m;
    return 0;
}

// 0x100805a0 ?HandleEvent@MfFSMManager@@UAEXAAVMfMsg@@@Z
void MfFSMManager::HandleEvent(MfMsg &msg)
{
    if (!m44 || !mEnabled || !m48 || !mActor->mLive)
        return;
    mHandling = 1;
    if (msg.mFrom) {
        if (g_msgDispatcher->Find(msg.mFromID))
            mLastSender = msg.mFrom->GetName();
        else
            mLastSender = NULL_STRING;
    } else
        mLastSender = NULL_STRING;
    mNumFired = 0;
    MfScDataHandle *outer = mVariables->Find("event");
    if (outer)
        outer->AddRef();
    MfScDataHandle *event = MakeEventData(mActor ? (MfAiActor *)mActor : 0, msg);
    mVariables->Replace("event", event, MfScHandleDict::kPassRef);
    MfDblListItem *pli;
    for (MfFiniteStateMachine *m = (MfFiniteStateMachine *)mBehaviors->GetFirst(pli); m;
         m = (MfFiniteStateMachine *)mBehaviors->GetNext(pli)) {
        if (m->mb4 == 1) {
            MfStateTransition *fired = m->HandleEvent(&msg);
            if (fired) {
                mFired[mNumFired] = fired;
                mNumFired++;
            }
            if (!mActor->mLive)
                break;
        }
    }
    mVariables->Replace("event", outer, MfScHandleDict::kPassRef);
    mHandling = 0;
}

// EQUIV 0x10080c10 ?ActivateBehavior@MfFSMManager@@UAEHVMfString@@@Z: reviewed: order of independent instructions only (the mb4 = 1 store and the vtable load for Start swap)
int MfFSMManager::ActivateBehavior(MfString name)
{
    int hasExtension = name.ContainsSubString(".BHVR", 0) > -1 || name.ContainsSubString(".bhvr", 0) > -1;
    if (!hasExtension)
        name << ".bhvr";
    MfDblListItem *pli;
    MfDblList *queued = &mQueuedTimers;
    for (MfFSMLoadRequest *request = (MfFSMLoadRequest *)queued->GetFirst(pli); request;
         request = (MfFSMLoadRequest *)queued->GetNext(pli)) {
        if (request->name == name) {
            request->arg = 1;
            return 0;
        }
    }
    MfFiniteStateMachine *machine = IsBehaviorLoaded(name);
    if (!machine) {
        LoadBehavior(name, 1, 1);
        machine = IsBehaviorLoaded(name);
    }
    if (machine && !machine->mb4) {
        machine->mb4 = 1;
        machine->Start();
    }
    return (int)machine;
}

// 0x10080db0 ?DeactivateBehavior@MfFSMManager@@QAEXVMfString@@@Z
void MfFSMManager::DeactivateBehavior(MfString name)
{
    int hasExtension = name.ContainsSubString(".BHVR", 0) > -1 || name.ContainsSubString(".bhvr", 0) > -1;
    if (!hasExtension)
        name << ".bhvr";
    MfFiniteStateMachine *machine = IsBehaviorLoaded(name);
    if (machine && machine->mb4 == 1)
        machine->mb4 = 0;
}

// 0x10080b80 ?AddMachine@MfFSMManager@@QAEXPAVMfFiniteStateMachine@@@Z
void MfFSMManager::AddMachine(MfFiniteStateMachine *machine)
{
    if (!machine)
        return;
    if (!mEditing) {
        machine->mManager = this;
        MfFSMList *list = (MfFSMList *)mBehaviors;
        if (list->mTrackChanges == 1)
            list->mChanged = 1;
        list->Append(machine);
        mLastLoaded = machine;
    } else {
        mCopiedState = 0;
        machine->mManager = this;
        machine->Initialize(mVarStack);
        machine->Initialize(mVarStack);
        if (machine->mRoot) {
            mCopiedState = machine->mRoot->FirstSubstate();
            mCopiedState->mMachine = mEditMachine;
            mCopiedState = mCopiedState->Clone();
            delete machine;
        }
    }
}

// 0x100815c0 ?ActorName@MfFSMManager@@QAE?AVMfString@@XZ

// 0x10081ab0 ?LinkIfShared@MfFSMManager@@QAEHVMfString@@@Z
int MfFSMManager::LinkIfShared(MfString name)
{
    if (!mActor)
        return 0;
    MfDblList *instances = mActor->Prototype()->Instances();
    MfDblListItem *pos;
    for (MfActor *actor = (MfActor *)instances->GetFirst(pos); actor;
         actor = (MfActor *)instances->GetNext(pos)) {
        if (actor != mActor && !actor->mTempCopy) {
            LinkBehavior(name);
            return 1;
        }
    }
    return 0;
}

// 0x10081b80 ?ReinitInInstances@MfFSMManager@@QAEXVMfString@@H@Z
void MfFSMManager::ReinitInInstances(MfString name, int all)
{
    if (!mActor)
        return;
    MfDblList *instances = mActor->Prototype()->Instances();
    MfDblListItem *pos;
    for (MfLiveActor *actor = (MfLiveActor *)instances->GetFirst(pos); actor;
         actor = (MfLiveActor *)instances->GetNext(pos)) {
        if (actor != mActor && !actor->mTempCopy) {
            MfFSMManager *manager = actor->GetHFSMManager();
            if (manager)
                manager->ReinitBehavior(name, all);
        }
    }
}

// 0x10081c70 ?ReinitBehavior@MfFSMManager@@QAEXVMfString@@H@Z
void MfFSMManager::ReinitBehavior(MfString name, int all)
{
    MfDblListItem *pos;
    for (MfFiniteStateMachine *machine = (MfFiniteStateMachine *)mBehaviors->GetFirst(pos); machine;
         machine = (MfFiniteStateMachine *)mBehaviors->GetNext(pos)) {
        if (all || machine->GetFile() == name) {
            machine->Link();
            machine->Initialize(mVarStack);
            break;
        }
    }
}

// 0x10081d40 ?LinkBehavior@MfFSMManager@@QAEXVMfString@@@Z
void MfFSMManager::LinkBehavior(MfString name)
{
    MfDblListItem *pos;
    for (MfFiniteStateMachine *machine = (MfFiniteStateMachine *)mBehaviors->GetFirst(pos); machine;
         machine = (MfFiniteStateMachine *)mBehaviors->GetNext(pos)) {
        if (machine->GetFile() == name)
            machine->Link();
    }
}

// 0x10081e00 ?UnloadInInstances@MfFSMManager@@QAEXVMfString@@@Z
void MfFSMManager::UnloadInInstances(MfString name)
{
    if (!mActor)
        return;
    MfDblList *instances = mActor->Prototype()->Instances();
    MfDblListItem *pos;
    for (MfLiveActor *actor = (MfLiveActor *)instances->GetFirst(pos); actor;
         actor = (MfLiveActor *)instances->GetNext(pos)) {
        if (actor != mActor && !actor->mTempCopy) {
            MfFSMManager *manager = actor->GetHFSMManager();
            if (manager)
                manager->UnloadBehavior(name);
        }
    }
}

// 0x10081ee0 ?UnloadBehavior@MfFSMManager@@QAEXVMfString@@@Z
void MfFSMManager::UnloadBehavior(MfString name)
{
    MfDblListItem *pos;
    for (MfFiniteStateMachine *machine = (MfFiniteStateMachine *)mBehaviors->GetFirst(pos); machine;
         machine = (MfFiniteStateMachine *)mBehaviors->GetNext(pos)) {
        if (machine->GetFile() == name) {
            mActor->Prototype()->UnregCompiledBhvr(machine->GetFile());
            machine->Unload();
            break;
        }
    }
}

// 0x10081fc0 ?ReportSaveError@MfFSMManager@@QAEXABVMfString@@0@Z
void MfFSMManager::ReportSaveError(const MfString &errorCode, const MfString &objectFile)
{
    MfString message("Error compiling behavior Piccolo code\n");
    message << "For the actor '" << ActorName() << "' the output of the Piccolo\ncould not be saved.\n"
            << "The object file is " << objectFile << "\n"
            << "and the error code is:" << errorCode;
    MfLogger::Log(2, "Conductor", "Compiler", message);
}

// 0x10082170 ?GetCurStateName@MfFSMManager@@QAEHVMfString@@AAV2@1@Z
int MfFSMManager::GetCurStateName(MfString machineName, MfString &path, MfString &name)
{
    MfDblListItem *pos;
    for (MfFiniteStateMachine *machine = (MfFiniteStateMachine *)mBehaviors->GetFirst(pos); machine;
         machine = (MfFiniteStateMachine *)mBehaviors->GetNext(pos)) {
        if (machine->GetFile() == machineName) {
            MfState *current = machine->mCurrent;
            if (current) {
                path = current->GetPath();
                name = current->GetName();
                return 1;
            }
        }
    }
    return 0;
}

// 0x100818d0 ?CompileBehaviors@MfFSMManager@@UAEXHH@Z
void MfFSMManager::CompileBehaviors(int force, int useCompiled)
{
    MfDblListItem *pos;
    for (MfFiniteStateMachine *machine = (MfFiniteStateMachine *)mBehaviors->GetFirst(pos); machine;
         machine = (MfFiniteStateMachine *)mBehaviors->GetNext(pos)) {
        int shared = 0;
        if (useCompiled == 1 && mActor->Prototype()->IsBhvrCompiled(machine->GetFile()) == 1)
            shared = LinkIfShared(machine->GetFile());
        if (!shared && (machine->mb0 == 1 || force == 1)) {
            if (StateScriptFlags::iFlags & 1) {
                machine->Initialize(mVarStack);
                int error;
                if (machine->Compile(0, &error) > 0) {
                    UnloadInInstances(machine->GetFile());
                    continue;
                }
                if (error) {
                    MfString code(error);
                    ReportSaveError(code, machine->GetFile());
                }
            } else {
                machine->LoadCompiled();
            }
            mActor->Prototype()->RegCompiledBhvr(machine->GetFile());
            UnloadInInstances(machine->GetFile());
            ReinitInInstances(machine->GetFile(), 0);
        }
        machine->Initialize(mVarStack);
    }
}

// 0x100815f0 ?QueueUnload@MfFSMManager@@QAEXVMfString@@@Z
void MfFSMManager::QueueUnload(MfString name)
{
    int hasExtension = name.ContainsSubString(".BHVR", 0) > -1 || name.ContainsSubString(".bhvr", 0) > -1;
    if (!hasExtension)
        name << ".bhvr";
    MfFiniteStateMachine *machine = IsBehaviorLoaded(name);
    if (!machine) {
        MfString message("This script is already unloaded: ");
        message << name;
        MfLogger::Log(1, "Conductor", "UnloadHFSM", message);
    } else if (mBehaviors->GetCount() < 2) {
        MfString message("You cannot unload ");
        message << name + MfString(" while ") + mActorName + MfString(" has only one loaded behavior.");
        MfLogger::Log(1, "Conductor", "UnloadHFSM", message);
    } else {
        mQueuedEvents.Add(machine);
        g_msgDispatcher->Post(new MfMsg(MSG_RUN_BEHAVIOR, 0, 0, this, this));
    }
}

// 0x10081560 ??0MfFSMLoadRequest@@QAE@VMfString@@H@Z

// TODO 0x10080eb0 ?LoadBehavior@MfFSMManager@@UAEHVMfString@@HH@Z: same code shape, but the original keeps the ifstream's virtual base offset in a register across is_open/eof (ours keeps the vbtable slot address), which shifts the register choice of the whole function
int MfFSMManager::LoadBehavior(MfString name, int compile, int activate)
{
    MfString path(mBhvrDir);
    MfString script;
    ifstream file;
    mLastLoaded = 0;
    int hasExtension = name.ContainsSubString(".BHVR", 0) > -1 || name.ContainsSubString(".bhvr", 0) > -1;
    if (!hasExtension)
        name << ".bhvr";
    MfFiniteStateMachine *machine = IsBehaviorLoaded(name);
    if (machine)
        return (int)machine;
    if ((g_world->IsRunning() && !mRunning) || mHandling == 1 || mStarting == 1) {
        // not now: load it on the next run
        mQueuedTimers.Add(new MfFSMLoadRequest(name, activate));
        g_msgDispatcher->Post(new MfMsg(MSG_RUN_BEHAVIOR, 0, 0, this, this));
        return 0;
    }
    path << "\\" << name;
    HANDLE handle = CreateFile((const char *)path, 0, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        MfString message("The actor ");
        message << ActorName();
        message << " was unable to load the ";
        message << name;
        message << " file.";
        MfLogger::Log(1, "Conductor", "Loading", message);
        return 0;
    }
    CloseHandle(handle);
    file.open((char *)path, ios::in);
    if (file.is_open()) {
        while (!file.eof()) {
            char line[256];
            file.getline(line, 255);
            script << line;
            script << '\n';
        }
        MfString owner;
        owner = mActorClass;
        gStateCompiler->Compile(owner, script, mBhvrDir, path, 0, 0, this);
    }
    file.close();
    MfAiCompiler *compiler;
    if (MfInterfaceRegistry::GetInterface("PiccoloCompilerI", (MfAi **)&compiler)) {
        if ((StateScriptFlags::iFlags & 1) || compile == 1) {
            if (mLastLoaded) {
                int error;
                mLastLoaded->Initialize(mVarStack);
                mLastLoaded->Initialize(mVarStack);
                mLastLoaded->Compile(0, &error);
                mLastLoaded->Initialize(mVarStack);
            }
        }
        compiler->Release();
    }
    mLastLoaded->Initialize(mVarStack);
    mActive = 1;
    mLastLoaded->Initialize(mVarStack);
    if (activate == 1) {
        if (!mActor->mGoingLive) {
            int live = 0;
            if (g_world->IsRunning() && mActor->mControlMode != 1) {
                live = 1;
                mActor->SetControlMode(1);
            }
            mLastLoaded->mb4 = 1;
            if (!live && !mLastLoaded->Start())
                mLastLoaded->mb4 = live;
        }
    } else
        mLastLoaded->mb4 = 0;
    return (int)mLastLoaded;
}

// Not claimed: in the original this member is folded with an identical function
// (10080ad0, claimed in MfActorLib as MfProtoSkill::SetInterpolate); the port needs
// its own definition.
void MfFSMManager::Enable(int on)
{
    m48 = on;
}
