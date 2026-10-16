#include <windows.h>
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include "SBMotivateDriver.h"
#include "MBRuntime.h"
#include "MfHookData.h"
#include "MfAiHook.h"
#include "MfLogger.h"
#include "RenderApp.h"
#include "SteamboatNode.h"
#include <stdlib.h>

MfAiWorld *SBMotivateDriver::mMofaWorld;
MfAiRuntime *SBMotivateDriver::mMofaRuntime;
MfAiSpace *SBMotivateDriver::mMofaSpace;
RenderApp *SBMotivateDriver::mMofaApp;
ViewEventHandler *SBMotivateDriver::mMofaEventHandler;
IMBGameEngine *SBMotivateDriver::mEngine;
void *SBMotivateDriver::sMutex;
SBMotivateDriver *SBMotivateDriver::_instance;

static const char *kMasterSpace = "steamboat";
static const char *kHCMofaLicense = "779872706787878";

// 0x100892a0 ??0SBMotivateDriver@@IAE@XZ
SBMotivateDriver::SBMotivateDriver()
{
    InitializeLock();
}

// 0x100892d0 ??_GSBMotivateDriver@@UAEPAXI@Z
// 0x10089310 ??1SBMotivateDriver@@UAE@XZ
SBMotivateDriver::~SBMotivateDriver()
{
    MfAiHookManager *hookManager;
    void *data;
    MfInterfaceRegistry::GetInterface("MfAiHookManager", (MfAi **)&hookManager);
    hookManager->RemoveHook('WLSp', (MfHookFunc)LoadSpaceHook, data);
    hookManager->RemoveHook('WDSp', (MfHookFunc)DeleteSpaceHook, data);
    hookManager->RemoveHook('WLAc', (MfHookFunc)LoadActorHook, data);
    hookManager->RemoveHook(MF_HOOK_USER_EVENT, (MfHookFunc)UserEventHook, data);
    hookManager->RemoveHook('WTel', (MfHookFunc)TeleportHook, data);
    hookManager->RemoveHook('WDAc', (MfHookFunc)DeleteActorHook, data);
    hookManager->RemoveHook(MF_HOOK_FILE_DESTINATION, (MfHookFunc)FileDestinationHook, data);
    hookManager->RemoveHook('WAAc', (MfHookFunc)AddActorHook, data);
    hookManager->RemoveHook('WCAc', (MfHookFunc)CreateActorHook, data);
    hookManager->Release();
    if (mMofaApp)
        delete mMofaApp;
    if (mMofaWorld)
        mMofaWorld->Release();
    if (mMofaSpace)
        mMofaSpace->Release();
    TerminateLock();
}

// 0x100894f0 ?GetInstance@SBMotivateDriver@@SAPAV1@XZ
SBMotivateDriver *SBMotivateDriver::GetInstance()
{
    if (!_instance)
        _instance = new SBMotivateDriver;
    return _instance;
}

// 0x10089580 ?InitializeLock@SBMotivateDriver@@SA_NXZ
bool SBMotivateDriver::InitializeLock()
{
    if (!sMutex) {
        sMutex = CreateMutexA(0, FALSE, 0);
        if (!sMutex) {
            MBDebugStr("ERROR: unable to create mutex in SBMotivateDriver\n");
            return false;
        }
    }
    return true;
}

// 0x100895d0 ?InfiniteWaitForObject@@YAKPAXPBDH@Z
unsigned long InfiniteWaitForObject(void *object, const char *file, int line)
{
    unsigned long result = WaitForSingleObject(object, 5000);
    switch (result) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_ABANDONED:
        MBDebugStr("%s(%d): FATAL ERROR: Wait for abandoned object 0x%08X.\n", file, line, object);
        abort();
    case WAIT_TIMEOUT:
        MBDebugStr("%s(%d): FATAL ERROR: Deadlock waiting for object 0x%08X.\n", file, line, object);
        abort();
    default:
        MBDebugStr("%s(%d): FATAL ERROR: ", file, line);
        MBDebugStr("Unknown return (%d) from WaitForSingleObject: object 0x%08X.\n", result, object);
        abort();
    }
    return 0;
}

// 0x100896a0 ?TerminateLock@SBMotivateDriver@@SAXXZ
void SBMotivateDriver::TerminateLock()
{
    if (sMutex) {
        CloseHandle(sMutex);
        sMutex = 0;
    }
}

// 0x100896d0 ?Init@SBMotivateDriver@@QAEPAVMfAiSpace@@PAVIMBGameEngine@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
MfAiSpace *SBMotivateDriver::Init(IMBGameEngine *engine, const MBString &mofaDir)
{
    mEngine = engine;
    MfLogger::EnableWindowOutput(0);
    if (!mMofaRuntime) {
        MfInitializeRuntime(&mMofaRuntime);
        mMofaRuntime->Initialize(kHCMofaLicense, 0);
    }
    mMofaApp = new RenderApp;
    mMofaWorld = mMofaRuntime->CreateWorld(mMofaApp);
    mMofaWorld->SetDataPath(mofaDir.c_str());
    mMofaSpace = mMofaWorld->LoadSpace(kMasterSpace, 0);

    MfAiHookManager *hookManager;
    MfInterfaceRegistry::GetInterface("MfAiHookManager", (MfAi **)&hookManager);
    hookManager->AddHook('WLSp', (MfHookFunc)LoadSpaceHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WDSp', (MfHookFunc)DeleteSpaceHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WLAc', (MfHookFunc)LoadActorHook, this, MF_HOOK_LAST);
    hookManager->AddHook(MF_HOOK_USER_EVENT, (MfHookFunc)UserEventHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WTel', (MfHookFunc)TeleportHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WDAc', (MfHookFunc)DeleteActorHook, this, MF_HOOK_LAST);
    hookManager->AddHook(MF_HOOK_FILE_DESTINATION, (MfHookFunc)FileDestinationHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WAAc', (MfHookFunc)AddActorHook, this, MF_HOOK_LAST);
    hookManager->AddHook('WCAc', (MfHookFunc)CreateActorHook, this, MF_HOOK_LAST);
    hookManager->Release();
    return mMofaSpace;
}

// 0x10089990 ?Update@SBMotivateDriver@@SAXXZ
void SBMotivateDriver::Update()
{
    if (mMofaWorld) {
        mMofaWorld->ProcessMessages();
        mMofaWorld->UpdateAllSpaces(timeGetTime());
    }
}

// 0x100899d0 ?GetMfCameraNode@SBMotivateDriver@@SAPAVMfAiActorNode@@XZ
MfAiActorNode *SBMotivateDriver::GetMfCameraNode()
{
    MBComPtr<IMBNodeManager> nodemgr = mEngine->GetNodeMgr();
    IMBNode *node = nodemgr->GetActiveNode();
    MfAiActorNode *cameraNode = ((SteamboatNode *)node)->GetMfCameraNode();
    node->Release();
    return cameraNode;
}

// 0x10089a70 ?GetMfRuntime@SBMotivateDriver@@SAPAVMfAiRuntime@@XZ
MfAiRuntime *__cdecl SBMotivateDriver::GetMfRuntime()
{
    return mMofaRuntime;
}

// 0x10089a90 ?OverrideReadActorHook@SBMotivateDriver@@SAHPAUMfActorRWHookData@@PAX@Z
int SBMotivateDriver::OverrideReadActorHook(MfActorRWHookData *hook, void *This)
{
    MfString path(hook->filePath);
    MfString drive;
    MfString dir;
    MfString actorName;
    MfString ext;
    path.SplitPath(&drive, &dir, &actorName, &ext);
    return 1;
}

// EQUIV 0x10089bc0 ?FileDestinationHook@SBMotivateDriver@@SAHPAUMfFileHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::FileDestinationHook(MfFileHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 310, hook);
    return 0;
}

// EQUIV 0x10089c90 ?AddActorHook@SBMotivateDriver@@SAHPAUMfActorHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::AddActorHook(MfActorHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 312, hook);
    return 0;
}

// EQUIV 0x10089d60 ?CreateActorHook@SBMotivateDriver@@SAHPAUMfActorHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::CreateActorHook(MfActorHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 313, hook);
    return 0;
}

// EQUIV 0x10089e30 ?DeleteActorHook@SBMotivateDriver@@SAHPAUMfActorHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::DeleteActorHook(MfActorHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 305, hook);
    return 0;
}

// EQUIV 0x10089f00 ?TeleportHook@SBMotivateDriver@@SAHPAUMfActorHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::TeleportHook(MfActorHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 304, hook);
    return 0;
}

// EQUIV 0x10089fd0 ?LoadSpaceHook@SBMotivateDriver@@SAHPAUMfSpaceHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::LoadSpaceHook(MfSpaceHookData *hook, void *This)
{
    mMofaSpace = hook->pSpace;
    MBDebugStr("SBMotivateDriver::LoadSpaceHook: space %s loaded\n", (const char *)hook->name);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 301, hook);
    return 0;
}

// 0x1008a0c0 ?DeleteSpaceHook@SBMotivateDriver@@SAHPAUMfSpaceHookData@@PAX@Z
int SBMotivateDriver::DeleteSpaceHook(MfSpaceHookData *hook, void *This)
{
    MBDebugStr("SBMotivateDriver::DeleteSpaceHook: space deleted\n");
    if (hook->pSpace == mMofaSpace)
        mMofaSpace = 0;
    return 0;
}

// EQUIV 0x1008a100 ?LoadActorHook@SBMotivateDriver@@SAHPAUMfActorHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::LoadActorHook(MfActorHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 302, hook);
    return 0;
}

// EQUIV 0x1008a1d0 ?UserEventHook@SBMotivateDriver@@SAHPAUMfUserHookData@@PAX@Z: reviewed: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls the out-of-line copy (matched in CActivityLeveler.cpp); nothing else differs. Was: VC6 inlines _QueryInterface into the broadcaster constructor, the original calls it (as SteamboatNode::PlaySfx)
int SBMotivateDriver::UserEventHook(MfUserHookData *hook, void *This)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(300, 303, hook);
    return 0;
}

// Not claimed (static initializers and header instances of this object):
// 0x10088c60 _$E18
// 0x10088c80 _$E15
// 0x10089250 _$E17
// 0x10089270 _$E16
// TODO 0x1008a2a0 $E396: static initializer from the headers (calls ??_B?1???id@?$ctype@G@std@@$D@@9@51, $E389); its $E number depends on the original header set
// 0x1008a2e0 _$E63
// TODO 0x1008a300 $E392: static initializer from the headers (registers ?id@?$collate@D@std@@$E with atexit); its $E number depends on the original header set
