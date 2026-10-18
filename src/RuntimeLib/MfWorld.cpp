// MfWorld (RuntimeLib.dll 10098e50-1009bb00), and the inline members emitted
// in the first object file:
// 0x10002000 ?CreateUniqueID@MfWorld@@UAEHXZ (= ?CreateUniqueNumber@MfWorld@@SAHXZ)
// 0x10002010 ?ActorExists@MfWorld@@QAEHPAVMfActor@@@Z
// 0x10002030 ?Type@MfWorld@@UAEHXZ
// 0x10002040 ?CreateSpace@MfWorld@@UAEPAVMfAiSpace@@XZ
// 0x10002060 ?SaveSpace@MfWorld@@UAEHPBVMfAiSpace@@@Z
// 0x10002090 ?SetRenderable@MfWorld@@UAEXH@Z
// 0x100020a0 ?StockMouseEnabled@MfWorld@@QAEHXZ
// 0x100020b0 ?CatchMsg@MfWorld@@UAEXAAVMfMsg@@@Z
// 0x100020d0 ?RegWorkerThreadAccess@MfWorld@@QAEXXZ
// 0x100020e0 ?UnRegWorkerThreadAccess@MfWorld@@QAEXXZ
#include "MfWorld.h"
#include <string.h>
#include "MfActor.h"
#include "MfApiActor.h"
#include "MfAiAppWorld.h"
#include "MfDataPath.h"
#include "MfHookManager.h"
#include "MfLiveActor.h"
#include "MfProtoActor.h"
#include "MfProtoDB.h"
#include "MfRenderer.h"
#include "MfMsgData.h"
#include "MfMsgDispatcher.h"
#include "MfProperty.h"
#include "MfScDebugger.h"
#include "MfSoundManager.h"
#include "MfJoystick.h"
#include "MfKeyboard.h"
#include "MfMouse.h"
#include "MfInterfaceRegistry.h"
#include "MfStateCompiler.h"
#include "MfAiContent.h"
#include "MfLogger.h"
#include "MfFileProgress.h"
#include "MfFSMManager.h"
#include "MfFSMMutable.h"
#include "MfStringList.h"
#include "MfTransform3.h"

// the world (MfWorld's constructor sets it, the destructor clears it)
MfWorld *g_world;
extern __declspec(dllimport) MfAiWorld *g_paiWorld;
extern __declspec(dllimport) MfAiAppWorld *g_paiAppWorld;

// 'UpLi': a space is about to go live or continue; a hook can clear update
struct MfSpaceUpdateHookData : public MfHookData {
    MfAiSpace *pSpace;                                      // 0x10
    int update;                                             // 0x14
    int resuming;                                           // 0x18
};

MfWorld::MsgHandler MfWorld::aMethods[NUM_MSGS];
int MfWorld::bMsgTableIsSet;

// 0x10098e50 ??0MfWorld@@QAE@PAVMfAiAppWorld@@@Z
// Registers the world with the dispatcher, makes the behaviour compiler,
// publishes the world as the MfAiWorld and tells the hooks.
MfWorld::MfWorld(MfAiAppWorld *app)
    : mWorkerThreads(0), mClock(0), mAccumPauseTime(0), mPauseTime(0), mApp(app), mStockMouseEnabled(1),
      m74(1), mRefCount(1)
{
    InitMsgTable();
    g_msgDispatcher->Register(this);
    mRunning = 0;
    mRenderable = 1;
    if (!gStateCompiler)
        new MfStateCompiler;
    MfInterfaceRegistry::RemoveInterface(MfString("MfAiWorld"));
    g_world = this;
    g_paiWorld = this;
    g_paiAppWorld = app;
    MfInterfaceRegistry::AddInterface((MfAiWorld *)this);
    MfRuntimeHookData data;
    data.value = 1;
    gHookManager->CallHooks('WRtR', &data);
}

// 0x10098fd0 ??_EMfWorld@@UAEPAXI@Z

// TODO 0x10099030 ??1MfWorld@@UAE@XZ: (77% similar) the original destroys mSpaceList without resetting its
// vptr (matches byte for byte when MfSpaceList has no user-declared destructor, but then
// MfLiveActor::NewParentAfterUngrasp differs); registers ebp/edi swap (70% similar)
MfWorld::~MfWorld()
{
    Unregister();
    _WaitForWorkerThreadsToFinish();
    GoAuthor(0);
    g_msgDispatcher->Unregister(this);
    EnterCriticalSection(&mCritSect);
    MfDblListItem *pli;
    for (MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pli); space; space = (MfSpace *)mSpaceList.GetNext(pli))
        space->Release();
    mSpaceList.Clear();
    LeaveCriticalSection(&mCritSect);
    MfDblList actors;
    actors.Copy(mActors);
    for (MfLiveActor *actor = (MfLiveActor *)actors.GetFirst(pli); actor; actor = (MfLiveActor *)actors.GetNext(pli)) {
        actor->CleanupActivity(0);
        actor->Release();
    }
    mActors.Clear();
    MfProtoDB::RemoveAllPrototypes();
    if (g_world == this) {
        g_world = 0;
        g_paiWorld = 0;
        if (gStateCompiler)
            delete gStateCompiler;
        gStateCompiler = 0;
    }
}

// 0x100991e0 ?_WaitForWorkerThreadsToFinish@MfWorld@@IAEXXZ
void MfWorld::_WaitForWorkerThreadsToFinish()
{
    while (mWorkerThreads)
        Sleep(100);
}

// 0x10099200 ?InitMsgTable@MfWorld@@MAEXXZ
void MfWorld::InitMsgTable()
{
    if (!bMsgTableIsSet) {
        bMsgTableIsSet = 1;
        for (int i = 0; i < NUM_MSGS; i++)
            aMethods[i] = &MfWorld::DefaultMsgHandler;
        aMethods[MSG_DESTROY_ACTOR] = &MfWorld::HandleDestroyActorRequest;
        aMethods[MSG_DESTROY_SPACE] = &MfWorld::HandleDestroySpaceRequest;
        aMethods[MSG_TELEPORT_ACTOR] = &MfWorld::HandleTeleportActorRequest;
    }
    mMsgTable = aMethods;
    Register();  // the original ends in a jump to MfRootObj::Register (0x10069340)
}

// 0x10099270 ?ProcessMessages@MfWorld@@UAEXXZ
void MfWorld::ProcessMessages()
{
    g_msgDispatcher->ProcessMessages();
}

// EQUIV 0x10099280 ?UpdateAllSpaces@MfWorld@@UAEHK@Z: reviewed: register choice (this and the result swap ebx/edi) and prologue scheduling of independent pushes and loads only
int MfWorld::UpdateAllSpaces(unsigned long clock)
{
    MfWorldHookData data;
    data.pWorld = this;
    data.clock = clock;
    gHookManager->CallHooks(MF_HOOK_UPDATE_WORLD, &data);
    mClock = clock - mAccumPauseTime;
    if (!mRunning)
        return 0;
    int moved = 0;
    MfDblList *spaces = &mSpaceList;
    MfDblListItem *pos;
    MfSpace *space = (MfSpace *)spaces->GetFirst(pos);
    while (space) {
        if (space->LiveUpdate(mClock) || moved)
            moved = 1;
        space = (MfSpace *)spaces->GetNext(pos);
    }
    if (MfLiveActor::plaPendingOptionTarget)
        return 1;
    return moved;
}

// 0x10099320 ?NewSpace@MfWorld@@QAEPAVMfSpace@@XZ
MfSpace *MfWorld::NewSpace()
{
    MfSpace *space = new MfSpace(this);
    space->RenderSpace() = mRenderable;
    space->CreateRenderNodes();
    EnterCriticalSection(&mCritSect);
    mSpaceList.Add(space);
    LeaveCriticalSection(&mCritSect);
    if (mRenderable)
        MfRenderer::Instance()->RenderAllViews();
    return space;
}

// 0x100994f0 ?HandleDestroySpaceRequest@MfWorld@@QAEXAAVMfMsg@@@Z
void MfWorld::HandleDestroySpaceRequest(MfMsg &msg)
{
    MfSpace *space = ((MfDeleteSpaceReqData *)msg.mData)->mSpace;
    if (!space)
        return;
    int missing = 0;
    EnterCriticalSection(&mCritSect);
    if (mSpaceList.Contains(space))
        mSpaceList.Remove(space, 0);
    else
        missing = 1;
    LeaveCriticalSection(&mCritSect);
    if (missing)
        return;
    MfSpaceHookData data;
    data.pSpace = space;
    gHookManager->CallHooks('WDSp', &data);
    space->Release();
}

// 0x10099400 ?RequestDestroySpace@MfWorld@@UAEXPAVMfSpace@@@Z
void MfWorld::RequestDestroySpace(MfSpace *space)
{
    MfSpaceHookData data;
    data.pSpace = space;
    if (gHookManager->CallHooks('WDSR', &data) > 0)
        return;
    MfDeleteSpaceReqData *request = new MfDeleteSpaceReqData(space);
    MfMsg *msg = new MfMsg(MSG_DESTROY_SPACE, this, this);
    msg->SetData(request);
    g_msgDispatcher->Post(msg);
}

// 0x100993c0 ?CloseSpace@MfWorld@@UAEHPBVMfAiSpace@@@Z
int MfWorld::CloseSpace(const MfAiSpace *space)
{
    RequestDestroySpace((MfSpace *)space);
    return 1;
}

// EQUIV 0x100995b0 ?LoadSpace@MfWorld@@UAEPAVMfAiSpace@@ABVMfString@@J@Z: reviewed: this and the zero register swap ebp/edi; where ours pushes or stores the zero register the original uses a register known to hold 0 there (the null space for the first SetProgress, the flag on its zero path) or an immediate 0; the space-constructor arguments come from the same values (0x34, 0, 0, this - 0x14, 0) and the error path returns 0 either way; same calls and stores
// Opens the space file (or finds it open), tells the hooks and posts the
// world's space-loaded message.
MfAiSpace *MfWorld::LoadSpace(const MfString &file, long flags)
{
    MfSpaceHookData data;
    data.name = file;
    data.rid = 0;
    data.hid = 0;
    data.pSpace = 0;
    if (gHookManager->CallHooks('WLSR', &data) > 0)
        return 0;
    MfString drive, dir, ext, name;
    file.SplitPath(&drive, &dir, &name, &ext);
    MfString path = ConstructSpaceFilePath(name, 0);
    MfSpace *space = mSpaceList.Contains(path);
    if (!space) {
        MfFileProgress::SetProgress(0, name);
        MfFileProgress::SetProgress(3, NULL_STRING);
        space = new MfSpace(g_world);
        space->mRenderSpace = 0;
        if (flags & 4)
            space->mLoadAllSkills = 0;
        MfFileProgress::SetProgress(0, file);
        int error = space->Load(path);
        MfFileProgress::SetProgress(2, NULL_STRING);
        if (error) {
            space->Release();
            space = 0;
            data.status = (MfRequestStatus)error;
        } else {
            EnterCriticalSection(&mCritSect);
            mSpaceList.Add(space);
            LeaveCriticalSection(&mCritSect);
            space->mRenderSpace = mRenderable;
            space->CreateRenderNodes();
            space->mRoot->syncChildTransforms(0);
            space->EnableStaticEnv(1);
            space->MakeSnapshotEnv(0);
            data.status = (MfRequestStatus)error;
        }
    } else
        data.status = MF_PREVIOUSLY_LOADED;
    data.pSpace = space;
    if (space)
        data.name = space->mSpaceFileName;
    else
        data.name = NULL_STRING;
    gHookManager->CallHooks('WLSp', &data);
    if (data.status == MF_SUCCESS) {
        MfMsg *msg = new MfMsg(52, 0, 0, this, 0);
        msg->mToSelf = 1;
        g_msgDispatcher->Post(msg);
    }
    MfRenderer::Instance()->RenderAllViews();
    return space;
}

// 0x10099900 ?RegisterActorPrototype@MfWorld@@UAEHPAVMfProtoActor@@@Z
int MfWorld::RegisterActorPrototype(MfProtoActor *proto)
{
    if (!proto)
        return 0;
    if (MfProtoDB::PrototypeExists(proto->Name()))
        return 0;
    MfProtoDB::AddPrototype(proto);
    return 1;
}

// 0x10099950 ?InstantiateActor@MfWorld@@UAEPAVMfAiActor@@PAVMfProtoActor@@@Z
MfAiActor *MfWorld::InstantiateActor(MfProtoActor *proto)
{
    MfLiveActor *actor = CreateActor();
    if (actor) {
        actor->AttachToPrototype(proto, 1);
        RegisterActor(actor);
    }
    return actor;
}

// TODO 0x10099990 ?LoadActor@MfWorld@@UAEPAVMfAiActor@@ABVMfString@@PBVMfAiSpace@@ABVMfTransform3@@H@Z: (84% similar)
// the space and the zero swap esi/ebp and the space conversions are computed in other
// places (72% similar)
// Loads an actor file into the space at xform and tells the hooks.
MfAiActor *MfWorld::LoadActor(const MfString &file, const MfAiSpace *aiSpace, const MfTransform3 &xform, int flags)
{
    if (!aiSpace)
        return 0;
    MfSpace *space = (MfSpace *)aiSpace;
    MfActorHookData data;
    data.name = MfString(file);
    data.rid = 0;
    data.hid = 0;
    data.pSpace = space;
    data.pActor = 0;
    gHookManager->CallHooks('WLAR', &data);
    MfFileProgress::SetProgress(3, NULL_STRING);
    MfLiveActor *actor = g_world->CreateActor();
    MfString drive, dir, ext, name;
    file.SplitPath(&drive, &dir, &name, &ext);
    int error = actor->Load(name, 0, flags);
    if (error) {
        data.status = (MfRequestStatus)error;
        data.pActor = 0;
        data.pSpace = space;
        gHookManager->CallHooks('WLAc', &data);
        return 0;
    }
    actor->CreateRenderNodes();
    actor->SetControlMode(3);
    space->AddActor(actor, xform);
    data.status = MF_SUCCESS;
    data.pActor = actor;
    data.pSpace = space;
    data.name = actor->GetInstanceName();
    gHookManager->CallHooks('WLAc', &data);
    return actor;
}

// 0x10099c20 ?CreateActor@MfWorld@@QAEPAVMfLiveActor@@XZ
MfLiveActor *MfWorld::CreateActor()
{
    MfActorHookData data;
    data.pActor = 0;
    gHookManager->CallHooks('WCAc', &data);
    if (!data.pActor)
        data.pActor = new MfApiActor(MF_THREAD_NAME_unknown);
    return (MfLiveActor *)data.pActor;
}

// 0x10099cd0 ?RequestDestroyActor@MfWorld@@UAEXPAVMfLiveActor@@@Z
void MfWorld::RequestDestroyActor(MfLiveActor *actor)
{
    MfActorHookData data;
    data.pActor = actor;
    data.pSpace = 0;
    if (gHookManager->CallHooks('WDAR', &data) > 0)
        return;
    MfDeleteActorReqData *request = new MfDeleteActorReqData(actor, 0);
    MfMsg *msg = new MfMsg(MSG_DESTROY_ACTOR, this, this);
    msg->SetData(request);
    g_msgDispatcher->Post(msg);
}

// 0x10099dd0 ?DeleteActor@MfWorld@@UAEXPBVMfAiActor@@@Z
void MfWorld::DeleteActor(const MfAiActor *actor)
{
    RequestDestroyActor((MfLiveActor *)actor);
}

// EQUIV 0x10099e00 ?HandleDestroyActorRequest@MfWorld@@QAEXAAVMfMsg@@@Z: reviewed: register choice only, every instruction and stack slot agrees (the original reloads the actor for the Release into edi (the request's register), this into eax)
void MfWorld::HandleDestroyActorRequest(MfMsg &msg)
{
    MfDeleteActorReqData *data = (MfDeleteActorReqData *)msg.mData;
    MfLiveActor *actor = data->mActor;
    if (!UnregisterActor(actor))
        return;
    g_msgDispatcher->Unregister(actor);
    if (actor->mSpace)
        actor->mSpace->RemoveActorSync(actor, 1);
    data->mActor->Release();
    MfRenderer::Instance()->RenderAllViews();
    MfMsg *done = new MfMsg(10, this, 0);
    done->mToSelf = 1;
    g_msgDispatcher->Post(done);
}

// 0x10099ef0 ?GoAuthor@MfWorld@@QAEXH@Z
void MfWorld::GoAuthor(int keepScripts)
{
    gHookManager->CallHooks('UpAu', 0);
    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;
    if (!keepScripts && debugger)
        debugger->Suspend(1);
    g_msgDispatcher->Pause();
    if (!keepScripts) {
        MfDblListItem *pos;
        for (MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pos); space;
             space = (MfSpace *)mSpaceList.GetNext(pos))
            space->GoAuthor();
    }
    mPauseTime = GetTickCount();
    mRunning = 0;
    m74 = mStockMouseEnabled;
    mStockMouseEnabled = 1;
    MfRenderer::Instance()->RenderAllViews();
    if (theSoundManager)
        theSoundManager->PauseAllSounds();
}

// 0x10099ec0 ?SaveSpace@MfWorld@@QAEHPAVMfSpace@@@Z
int MfWorld::SaveSpace(MfSpace *space)
{
    if (space->SpaceFileName().Length() == 0)
        return 0;
    return space->Save(space->SpaceFileName());
}

// 0x10099fa0 ?GoLive@MfWorld@@QAEXKH@Z
void MfWorld::GoLive(unsigned long clock, int keepScripts)
{
    mAccumPauseTime = 0;
    mRunning = 1;
    if (clock > 0)
        mClock = clock;
    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;
    if (!keepScripts && debugger)
        debugger->Suspend(0);
    g_msgDispatcher->Start();
    if (theSoundManager)
        theSoundManager->StopAllSounds();
    MfMouse *mouse = gMouse;
    if (mouse) {
        mouse->mLButtonDownRoutes.Clear();
        mouse->mLButtonUpRoutes.Clear();
        mouse->mRButtonDownRoutes.Clear();
        mouse->mRButtonUpRoutes.Clear();
        mouse->mMouseMoveRoutes.Clear();
    }
    if (gKeyboard)
        gKeyboard->RemoveRoutes(0);
    if (gJoysticks[0])
        gJoysticks[0]->RemoveRoutes(0);
    if (gJoysticks[1])
        gJoysticks[1]->RemoveRoutes(0);
    MfDblListItem *pos;
    for (MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pos); space; space = (MfSpace *)mSpaceList.GetNext(pos)) {
        MfSpaceUpdateHookData data;
        data.pSpace = space;
        data.update = 1;
        data.resuming = 0;
        gHookManager->CallHooks('UpLi', &data);
        if (data.update)
            space->GoLive();
    }
}

// 0x1009a0c0 ?ContinueLive@MfWorld@@QAEXH@Z
void MfWorld::ContinueLive(int keepScripts)
{
    if (mRunning == 1)
        return;
    mRunning = 1;
    MfAiPdbDebugger *debugger = MfScDebugger::pdbgTheDebugger;
    if (!keepScripts && debugger)
        debugger->Suspend(0);
    g_msgDispatcher->Resume();
    theSoundManager->ResumeAllSounds();
    if (!keepScripts) {
        MfDblListItem *pos;
        for (MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pos); space;
             space = (MfSpace *)mSpaceList.GetNext(pos)) {
            MfSpaceUpdateHookData data;
            data.pSpace = space;
            data.update = 1;
            data.resuming = 1;
            gHookManager->CallHooks('UpLi', &data);
            if (data.update)
                space->ContinueLive();
        }
    }
    if (mPauseTime > 0)
        mAccumPauseTime += GetTickCount() - mPauseTime;
    mStockMouseEnabled = m74;
}

// 0x1009a1a0 ?FlushSpaceList@MfWorld@@QAEXPAVMfSpace@@@Z
void MfWorld::FlushSpaceList(MfSpace *keep)
{
    EnterCriticalSection(&mCritSect);
    MfDblList *spaces = &mSpaceList;
    MfDblListItem *pos;
    MfSpace *space = (MfSpace *)spaces->GetFirst(pos);
    while (space) {
        if (space != keep) {
            MfSpaceHookData data;
            data.pSpace = space;
            gHookManager->CallHooks('WDSp', &data);
            space->Release();
        }
        space = (MfSpace *)spaces->GetNext(pos);
    }
    spaces->Clear();
    if (keep)
        spaces->Add(keep);
    LeaveCriticalSection(&mCritSect);
}

// 0x1009a290 ?RegisterActor@MfWorld@@QAEXPAVMfActor@@@Z
void MfWorld::RegisterActor(MfActor *actor)
{
    if (!mActors.Find(actor))
        mActors.Add(actor);
}

// 0x1009a2c0 ?GetActor@MfWorld@@UAEPAVMfAiActor@@H@Z
MfAiActor *MfWorld::GetActor(int id)
{
    MfDblListItem *pos;
    MfActor *actor = (MfActor *)mActors.GetFirst(pos);
    while (actor) {
        if (actor->UniqueID() == id)
            return actor;
        actor = (MfActor *)mActors.GetNext(pos);
    }
    return 0;
}

// 0x1009a320 ?UnregisterActor@MfWorld@@QAEHPAVMfActor@@@Z
int MfWorld::UnregisterActor(MfActor *actor)
{
    if (MfLiveActor::plaPendingOptionTarget == actor)
        MfLiveActor::plaPendingOptionTarget = 0;
    MfDblListItem *item = mActors.Find(actor);
    if (item) {
        mActors.Delete(item);
        return 1;
    }
    return 0;
}

// 0x1009a370 ?ConstructSpaceFilePath@MfWorld@@SA?AVMfString@@ABV2@H@Z
// The file of the space name: under the data folder's SPACES (or the remote
// server), upper case, with the .ENV extension.
MfString MfWorld::ConstructSpaceFilePath(const MfString &name, int remote)
{
    MfString path;
    if (remote)
        path << "REMOTE:\\";
    else {
        path = GetDataPath();
        path << "\\SPACES\\";
    }
    path << name;
    path.ConvertToUpper();
    int hasExtension = path.ContainsSubString(".ENV", 0) > -1;
    if (!hasExtension)
        path << ".ENV";
    return path;
}

// 0x1009a550 ?GetSpaceFileName@MfWorld@@SA?AVMfString@@ABV2@H@Z
// The name of a space file relative to the spaces folder (or the remote
// server): file upper case, without the folder up to SPACES.
MfString MfWorld::GetSpaceFileName(const MfString &file, int remote)
{
    MfString local, drive, dir, name, ext;
    MfString upper(file);
    upper.ConvertToUpper();
    if (remote) {
        int pos = upper.ContainsSubString("REMOTE:\\", 0);
        if (pos >= 0)
            local = upper.RightString(upper.Length() + pos - 8);
        else
            local = upper;
        local.SplitPath(&drive, &dir, &name, &ext);
        dir << name;
        dir.ConvertToUpper();
        return dir;
    }
    upper.SplitPath(&drive, &dir, &name, &ext);
    int pos = dir.ContainsSubString("\\SPACES\\", 0);
    if (pos >= 0)
        dir = dir.RightString(dir.Length() + pos - 8);
    dir << name;
    return dir;
}

// 0x1009a910 ?ConstructActorFilePath@MfWorld@@SA?AVMfString@@ABV2@@Z
// The file of the actor name under the data folder's ACTORS, upper case.
MfString MfWorld::ConstructActorFilePath(const MfString &name)
{
    MfString path = GetDataPath();
    path << "\\ACTORS\\";
    path << name;
    path.ConvertToUpper();
    return path;
}

// 0x1009aa60 ?FindPiccoloSourceByModule@MfWorld@@QAE?AVMfString@@V2@@Z
// The behaviour script of a module path actor\behaviour...: the script
// of the first live instance of the actor prototype.
MfString MfWorld::FindPiccoloSourceByModule(MfString module)
{
    MfString actorName;
    MfString source;
    MfStringList parts;
    MfFSMMutable::SplitPath(&parts, module);
    MfDblListItem *pli;
    actorName = *(MfString *)parts.GetFirst(pli);
    int loaded;
    MfProtoActor *proto = MfProtoDB::GetPrototype(actorName, loaded, 0);
    if (proto) {
        MfActor *first = (MfActor *)proto->Instances()->GetFirst(pli);
        if (first) {
            MfLiveActor *actor = dynamic_cast<MfLiveActor *>(first);
            if (actor)
                source = actor->GetHFSMManager()->GetScript(module);
        }
    }
    return source;
}

// 0x1009ac30 ?WritePiccoloSourceByModule@MfWorld@@QAEXVMfString@@0@Z
// Gives the behaviour of the module path the script source (through the
// first live instance of the actor prototype).
void MfWorld::WritePiccoloSourceByModule(MfString module, MfString source)
{
    MfString actorName;
    MfString unused;
    MfStringList parts;
    MfFSMMutable::SplitPath(&parts, module);
    MfDblListItem *pli;
    actorName = *(MfString *)parts.GetFirst(pli);
    int loaded;
    MfProtoActor *proto = MfProtoDB::GetPrototype(actorName, loaded, 0);
    if (proto) {
        MfActor *first = (MfActor *)proto->Instances()->GetFirst(pli);
        if (first) {
            MfLiveActor *actor = dynamic_cast<MfLiveActor *>(first);
            if (actor)
                actor->GetHFSMManager()->Hook29(module, source);  // sets the script of the module
        }
    }
}

// 0x1009adf0 ?GetInterfaceName@MfWorld@@UAEXPADHAAH1@Z
void MfWorld::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiWorld", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10006370 ?App@MfWorld@@QAEPAVMfAiAppWorld@@XZ
MfAiAppWorld *MfWorld::App()
{
    return mApp;
}

// 0x1009ae30 ?AddRef@MfWorld@@UAEHXZ
int MfWorld::AddRef()
{
    return ++mRefCount;
}

// 0x1009aef0 ?SetDataPath@MfWorld@@UAEXABVMfString@@@Z
void MfWorld::SetDataPath(const MfString &path)
{
    ::SetDataPath(path);
}

// 0x1009afc0 ?DisableToolMouseBehavior@MfWorld@@UAEXXZ
void MfWorld::DisableToolMouseBehavior()
{
    mStockMouseEnabled = 0;
}

// 0x1009ae60 ?GetAllSpaces@MfWorld@@UAEXPAVMfDblList@@@Z
void MfWorld::GetAllSpaces(MfDblList *spaces)
{
    EnterCriticalSection(&mCritSect);
    MfDblListItem *pos;
    MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pos);
    while (space) {
        spaces->Add((MfAiSpace *)space);
        space = (MfSpace *)mSpaceList.GetNext(pos);
    }
    LeaveCriticalSection(&mCritSect);
}

// 0x1009aec0 ?SaveActor@MfWorld@@UAEHPBVMfAiActor@@@Z
int MfWorld::SaveActor(const MfAiActor *actor)
{
    return ((MfActor *)actor)->Save(MF_SKILL_NO_SAVE);
}

// 0x1009af00 ?Start@MfWorld@@UAEXH@Z
void MfWorld::Start(int arg)
{
    if (!mRunning)
        GoLive(mApp->GetClock(), arg);
}

// 0x1009af30 ?Stop@MfWorld@@UAEXXZ
void MfWorld::Stop()
{
    if (mRunning == 1)
        GoAuthor(0);
}

// 0x1009af50 ?Continue@MfWorld@@UAEXXZ
void MfWorld::Continue()
{
    ContinueLive(0);
}

// 0x1009af60 ?Reset@MfWorld@@UAEXXZ
void MfWorld::Reset()
{
    int running = mRunning;
    if (running)
        Stop();
    MfDblListItem *pos;
    MfSpace *space = (MfSpace *)mSpaceList.GetFirst(pos);
    while (space) {
        space->ResetEnv();
        space = (MfSpace *)mSpaceList.GetNext(pos);
    }
    if (running)
        Start(0);
}

// 0x1009afb0 ?EnableToolMouseBehavior@MfWorld@@UAEXXZ
void MfWorld::EnableToolMouseBehavior()
{
    mStockMouseEnabled = 1;
}

// 0x1009afd0 ?CreateMaterial@MfWorld@@UAEPAVMfMaterial@@XZ
MfMaterial *MfWorld::CreateMaterial()
{
    return new MfMaterial;
}

// 0x1009b030 ??_GMfMaterial@@UAEPAXI@Z (the local copy for `new MfMaterial`)

// 0x1009b050 ?GetPropertyNames@MfWorld@@UAEXPAVMfStringList@@@Z
void MfWorld::GetPropertyNames(MfStringList *names)
{
    mProperties.GetPropertyNames(*names, 0);
}

// 0x1009b070 ?GetProperty@MfWorld@@UAEHABVMfString@@AAVMfProperty@@@Z
int MfWorld::GetProperty(const MfString &name, MfProperty &property)
{
    MfProperty *found = mProperties.Find(name);
    if (found) {
        property = *found;
        return 1;
    }
    return 0;
}

// TODO 0x1009b0a0 ?SetProperty@MfWorld@@UAEXAAVMfProperty@@@Z: (83% similar) the original calls the imported
// MfProperty::Name (VC5 inline budget), this inlines it (73% similar)
// Sets the world property of the name, adding it if it is new.
void MfWorld::SetProperty(MfProperty &property)
{
    int found = 0;
    MfProperty *existing = mProperties.Find(property.Name());
    if (existing) {
        *existing = property;
        found = 1;
    }
    if (!found)
        mProperties.Add(new MfProperty(property));
}

// 0x1009b140 ?SetProperty@MfWorld@@UAEXPAVMfProperty@@@Z
void MfWorld::SetProperty(MfProperty *property)
{
    mProperties.Replace(property);
}

// 0x1009b160 ?GetProperty@MfWorld@@UAEHABVMfString@@PAPAVMfProperty@@@Z
int MfWorld::GetProperty(const MfString &name, MfProperty **property)
{
    *property = 0;
    *property = mProperties.Find(name);
    return *property != 0;
}

// a teleport request (MfWorld::TeleportActor), handled on the message thread
class MfTeleportActorData : public MfMsgDataBlock {
public:
    MfTeleportActorData(MfAiActor *actor, MfTransform3 xform, const MfString &space);
    virtual ~MfTeleportActorData();

    MfAiActor *mActor;                                      // 0x08
    MfString mSpace;                                        // 0x0c the space file
    MfTransform3 mXform;                                    // 0x10 where the actor goes
};

// 0x1009b190 ?TeleportActor@MfWorld@@UAEHPAVMfAiActor@@ABVMfString@@HABVMfTransform3@@@Z
// Asks the hooks, then posts the move of actor to the space file space to
// the world's own queue; 0 if a hook refused.
int MfWorld::TeleportActor(MfAiActor *actor, const MfString &space, int, const MfTransform3 &xform)
{
    MfActorHookData data;
    data.name = MfString(space);
    data.pActor = actor;
    data.pSpace = 0;
    data.rid = 0;
    data.hid = 0;
    if (gHookManager->CallHooks('WTeR', &data) > 0)
        return 0;
    MfTeleportActorData *block = new MfTeleportActorData(actor, xform, space);
    MfMsg *msg = new MfMsg(MSG_TELEPORT_ACTOR, this, this);
    msg->SetData(block);
    g_msgDispatcher->Post(msg);
    return 1;
}

// 0x1009b2f0 ??0MfTeleportActorData@@QAE@PAVMfAiActor@@VMfTransform3@@ABVMfString@@@Z
MfTeleportActorData::MfTeleportActorData(MfAiActor *actor, MfTransform3 xform, const MfString &space)
{
    mActor = actor;
    mSpace = space;
    mXform = xform;
}

// 0x1009b3b0 ??_GMfTeleportActorData@@UAEPAXI@Z

// 0x1009b3d0 ??1MfTeleportActorData@@UAE@XZ
MfTeleportActorData::~MfTeleportActorData()
{
}

// TODO 0x1009b420 ?HandleTeleportActorRequest@MfWorld@@QAEXAAVMfMsg@@@Z: (72% similar) the request pointer
// and the hook data are kept in other registers and slots, and the branches after the
// space lookup are laid out differently (37% similar)
// Moves the actor into the space (loading it if it is not open) and tells
// the hooks how it went.
void MfWorld::HandleTeleportActorRequest(MfMsg &msg)
{
    MfTeleportActorData *request = (MfTeleportActorData *)msg.mData;
    MfActorHookData data;
    data.name = MfString(request->mSpace);
    data.pActor = request->mActor;
    data.pSpace = 0;
    data.rid = 0;
    data.hid = 0;
    MfString drive, dir, name;
    request->mSpace.SplitPath(&drive, &dir, &name, 0);
    name.ConvertToUpper();
    MfString path = ConstructSpaceFilePath(name, 0);
    MfLiveActor *actor = (MfLiveActor *)request->mActor;
    if (!mActors.Find(actor)) {
        MfLogger::Log(2, "TeleportActor", "Asynch Handling",
                      "The teleporting actor was deleted in a previous asynchronous call.");
    } else {
        MfSpace *space = mSpaceList.Contains(path);
        if (space) {
            if (actor->mSpace)
                actor->mSpace->RemoveActorSync(actor, 1);
            space->AddActorSync(actor, request->mXform, 1);
            data.status = MF_SUCCESS;
        } else {
            space = (MfSpace *)LoadSpace(name, 0);
            if (!space)
                data.status = MF_ERROR;
            else {
                if (actor->mSpace)
                    actor->mSpace->RemoveActorSync(actor, 1);
                space->AddActorSync(actor, request->mXform, 1);
                data.status = MF_SUCCESS;
            }
        }
        data.pSpace = space;
        gHookManager->CallHooks('WTel', &data);
        MfRenderer::Instance()->RenderAllViews();
    }
}

// folded with the other empty message handlers at 0x1000def0
void MfWorld::DefaultMsgHandler(MfMsg &)
{
}

// 0x1009ae40 ?Release@MfWorld@@UAEHXZ
int MfWorld::Release()
{
    int count;
    if ((count = --mRefCount) == 0)
        delete this;
    return count;
}

// 0x1009b720 ?InitializeDataPath@@YAXXZ
// Loads the content plugin once (its interface is fetched and let go).
void InitializeDataPath()
{
    static int initialized;
    if (!initialized) {
        initialized = 1;
        MfAiContent *content;
        if (MfInterfaceRegistry::GetInterface("ContentInfoI", (MfAi **)&content))
            content->Release();
    }
}

// 0x1009b7b0 ?GetDataPath@@YA?AVMfString@@XZ
// The data folder of the current content, else ".".
MfString GetDataPath()
{
    MfString path(".");
    MfAiContent *content;
    if (MfInterfaceRegistry::GetInterface("ContentInfoI", (MfAi **)&content)) {
        path = content->GetDataPath();
        content->Release();
    }
    return path;
}

// 0x1009b890 ?GetImagePath@@YA?AVMfString@@XZ
MfString GetImagePath()
{
    MfString path = GetDataPath();
    path << "\\Images\\";
    return path;
}

// 0x1009b910 ?SetDataPath@@YAXABVMfString@@@Z
// Makes path the content's data folder and points the sound manager and the
// renderer at its Sounds and Images folders.
void SetDataPath(const MfString &path)
{
    MfAiContent *content;
    if (MfInterfaceRegistry::GetInterface("ContentInfoI", (MfAi **)&content)) {
        content->SetContentPath(path);
        content->Release();
    }
    MfString sounds(path);
    sounds << "\\Sounds\\";
    theSoundManager->SetSharedPath(sounds);
    MfString images(path);
    images << "\\Images\\";
    MfRenderer::Instance()->SetSearchPath(images);
}

// 0x1009ba20 ?InitializeDataFolders@@YAXXZ
void InitializeDataFolders()
{
    MfAiContent *content;
    if (MfInterfaceRegistry::GetInterface("ContentInfoI", (MfAi **)&content)) {
        content->CreateDataFolders();
        content->Release();
    }
}

// 0x1009baa0 ?GetInstances@MfWorld@@UAEHPAVMfProtoActor@@AAVMfDblList@@@Z
// Adds the instances of proto to instances.
int MfWorld::GetInstances(MfProtoActor *proto, MfDblList &instances)
{
    if (proto) {
        MfDblListItem *pli;
        for (MfActor *actor = (MfActor *)proto->Instances()->GetFirst(pli); actor;
             actor = (MfActor *)proto->Instances()->GetNext(pli))
            instances.Add((MfAiActor *)actor);
        return 1;
    }
    return 0;
}

// Not claimed: in the original this member is folded with an identical function
// (1007f9e0, MfActor::ActivateBehavior); the port needs its own definition.
int MfWorld::LoadActorGroup(const MfString &)
{
    return 0;
}

// Not claimed: in the original this member is folded with an identical function
// (1007fa30, MfActor::CreateHoldActivity); the port needs its own definition.
int MfWorld::SaveActorGroup(const MfAiActor *, const MfString &)
{
    return 0;
}

// the class's static members (zero in the original's data, iUniqueNumber 1)
int MfWorld::enableNetwork;
int MfWorld::iUniqueNumber = 1;
