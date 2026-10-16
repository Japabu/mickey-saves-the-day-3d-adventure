// MfRuntime (RuntimeLib.dll 100034b0-10004990). The object starts with the
// inline members of the exported classes it includes, in address order;
// several were folded by the linker with identical functions of other classes:
// 0x100034b0 ?SetEventID@MfLiveActor@@QAEXH@Z
// 0x100034c0 ?GetHFSMManager@MfLiveActor@@QAEPAVMfFSMManager@@XZ
// 0x100034d0 ?GetLastOptionID@MfLiveActor@@UAEHXZ
// 0x100034e0 ?SetOptionTarget@MfLiveActor@@UAEXPBVMfAiActor@@@Z
// 0x10003500 ?GetRootActivity@MfLiveActor@@UAEPAVMfAiActivity@@XZ
// 0x10003520 ?GetLastMessage@MfLiveActor@@UAEABVMfString@@XZ (= MfSpace::GetLiveActors)
// 0x10003530 ?GetLastEventID@MfLiveActor@@UAEHXZ (= MfSpace::GetRoot, MfLiveActor::LastConfig)
// 0x10003540 ?GetLastPickInfo@MfLiveActor@@UAEPBUMfPickData@@XZ
// 0x10003550 ?GetLastContourFailDownInfo@MfLiveActor@@UAEPBUMfContourFailDownData@@XZ
// 0x10003560 ?GetLastLocoFailureInfo@MfLiveActor@@UAEHXZ
// 0x10003570 ?GetLastManipFailureInfo@MfLiveActor@@UAEHXZ
// 0x10003580 ?AttachTarget@MfActor@@UAEHPBVMfAiActor@@ABVMfString@@1H@Z (= several MfActor stubs)
// 0x10003600 ?GetBlockCount@MfLiveActor@@QAEHXZ
// 0x10003610 ?GetAnchorNodeStop@MfLiveActor@@UAEPAVMfAiActorNode@@XZ
// 0x10003630 ?OwnerActor@MfLiveActor@@MAEPAVMfAiActor@@XZ
// 0x10003650 ?CatchMsg@MfLiveActor@@UAEXAAVMfMsg@@@Z
// 0x10003690 ?Type@MfApiActor@@UAEHXZ
// 0x100036a0 ?IsType@MfApiActor@@UAEHH@Z
// (0x100036e0-0x100038c0: MfLocoSkill and MfManipSkill, claimed in their units)
// 0x100038d0 ?GetDuration@MfActivity@@UAEHXZ (= MfActivity::GetDuration, ...)
#include <windows.h>
#include <string.h>
#include "MfRuntime.h"
#include "MfApiActor.h"
#include "MfLocoSkill.h"
#include "MfManipSkill.h"
#include "MfRenderer.h"
#include "MfSoundManager.h"
#include "MfWorld.h"
#include "MfJoystick.h"
#include "MfPoJoystick.h"
#include "MfLogger.h"
#include "MfAiApplication.h"
#include "MfScPiccolo.h"
#include "MfHookManager.h"
#include "MfInterfaceRegistry.h"
#include "MfKeyboard.h"
#include "MfKeyManager.h"
#include "MfMouse.h"
#include "MfMsgDispatcher.h"
#include "MfStream.h"
#include "MfViewEventHandler.h"
#include "MfProtoActor.h"

// 0x10003590 ?SetPlannedActivityOptions@MfLiveActor@@UAEXHH@Z
// The options the planned activities use: kind 1 the loco options (with 0x200
// the reach is three times the bounding height), kind 2 the manipulation options.
// It sits in the MfAiActor vtable, so the code addresses the members from the MfAiActor
// part (+0x14): mPlannedLocoOptions at 0x2fc is [this + 0x2e8], mPrototype is [this + 8].
void MfLiveActor::SetPlannedActivityOptions(int kind, int options)
{
    if (kind == 1) {
        mPlannedLocoOptions = options;
        if (mPrototype && (options & 0x200)) {
            float radius, height;
            mPrototype->GetBoundingCylinder(radius, height);
            mPlannedLocoReach = mPlannedLocoParam = height * 3.0f;
        } else {
            mPlannedLocoParam = 0;
            mPlannedLocoReach = 0;
        }
    }
    if (kind == 2)
        mPlannedManipOptions = options;
}

// 0x100038e0 ?MfInitializeRuntime@@YAXPAPAVMfAiRuntime@@@Z
void MfInitializeRuntime(MfAiRuntime **runtime)
{
    *runtime = new MfRuntime;
}

// 0x10003930 ??0MfRuntime@@QAE@XZ
MfRuntime::MfRuntime() : mRefCount(1), mInitialized(0)
{
}

// 0x10003950 ??1MfRuntime@@UAE@XZ
MfRuntime::~MfRuntime()
{
    if (mInitialized)
        Shutdown();
}

HINSTANCE g_hInstance;
// the joysticks InitializeJoysticks makes
MfJoystick *gJoysticks[2];
extern MfAiKeyboard *g_paiKeyboard;
extern MfAiMouse *g_paiMouse;
// 0x10003970 ?GetInterfaceName@MfRuntime@@UAEXPADHAAH1@Z
void MfRuntime::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiRuntime", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x100039b0 ?AddRef@MfRuntime@@UAEHXZ
int MfRuntime::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}

// 0x100039c0 ?Release@MfRuntime@@UAEHXZ
int MfRuntime::Release()
{
    int refs = InterlockedDecrement(&mRefCount);
    if (refs == 0 && this)
        delete this;
    return refs;
}

// 0x100039f0 _DllMain@12
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        g_hInstance = instance;
    return TRUE;
}

// the libraries whose version must be the runtime's
static const char *gLibraries[] = { "mfutil", "mflinalg", "mfactorlib", "runtimelib", 0 };

// TODO 0x10003a10 ?VerifyLibraryVersions@MfRuntime@@AAEHVMfString@@@Z: the frame is 4 bytes
// larger and the library pointer is set up after the first test (62% similar)
// Warns of every library whose file version is not version; 0 if any.
int MfRuntime::VerifyLibraryVersions(MfString version)
{
    int result = 1;
    for (const char **library = gLibraries; *library; library++) {
        unsigned long length;
        MfString found(MfVersionResource(GetModuleHandle(*library)).GetString((unsigned short *)L"FileVersion", &length));
        if (found != version) {
            MfString message;
            message.Format("The library %s is version %s (expected version %s).This may cause unexpected behavior.",
                           *library, (const char *)found, (const char *)version);
            MessageBox(0, message, "Motivate Runtime", MB_ICONEXCLAMATION);
            result = 0;
        }
    }
    return result;
}

// the application interface the runtime publishes
class MfApplication : public MfAiApplication {
public:
    MfApplication() : mRefCount(1) {}
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef() { return ++mRefCount; }
    virtual int Release();
    virtual int GetApplicationType() { return 1; }
    virtual ~MfApplication() {}

    int mRefCount;                                          // 0x04
};

void ShowAccessKeyMessage();
// registers the "SCRIPT" source type with the source debugger (1006bf90)
void RegisterScriptProvider();
// creates the skills of skill files (MfSkill.cpp, 10051bf0)
MfStreamObject *CreateSkill();
// loads the hook plugins (10004990)
static void LoadHookPlugins();
void MfPoInitializeClasses();
void MfPoShutdownClasses();
void InitializeDataPath();

// EQUIV 0x10003b60 ?Initialize@MfRuntime@@UAEHPBD0@Z: reviewed: the frame is 4 bytes larger
// (the stream constructor entry lives in the dead key parameter); every instruction and EH
// funclet is the same apart from the stack offsets
// Checks the access key and the library versions, publishes the application
// and hook manager interfaces and starts Piccolo, the dispatcher, keyboard,
// mouse, the script classes, the hook plugins, sound and renderer.
int MfRuntime::Initialize(const char *key, const char *version)
{
    if (mInitialized)
        return 0;
    MfKeyManager keys;
    int network;
    if (!keys.CheckRuntimeKey(key, &network)) {
        ShowAccessKeyMessage();
        exit(0);
    }
    if (network)
        MfWorld::enableNetwork = 1;
    if (version)
        VerifyLibraryVersions(version);
    MfApplication *application = new MfApplication;
    MfInterfaceRegistry::AddInterface(application);
    application->Release();
    gHookManager = new MfHookManager;
    MfInterfaceRegistry::AddInterface(gHookManager);
    MfScPiccolo::Initialize();
    g_msgDispatcher = new MfMsgDispatcher;
    new MfKeyboard;
    new MfMouse;
    MfString errors;
    MfScPiccolo::InitializePlugins(&errors);
    MfPoInitializeClasses();
    MfPoLibrary::Initialize();
    MfScPiccolo::LoadPiccoloLibFunctions(1);
    MfScPiccolo::LoadPiccoloDataPathFunctions(1);
    MfScPiccolo::InitializeExtensions(&errors);
    InitializeDataPath();
    MfScPiccolo::bIsRunning = 1;
    RegisterScriptProvider();
    MfStreamConstructorEntry skills(MfProtoSkill::STREAM_TYPE, CreateSkill);
    LoadHookPlugins();
    MfSoundManager::Initialize();
    MfRenderer::Initialize();
    mInitialized = 1;
    return 1;
}

// 0x10003d90 ??_GMfApplication@@UAEPAXI@Z

// 0x10003db0 ?ShowAccessKeyMessage@@YAXXZ (original (unnamed):) tells the user that the runtime needs an access key
void ShowAccessKeyMessage()
{
    MessageBox(0, "To use the Motivate Runtime with your own application,\nan access key is required. Please contact the Motion\nFactory for assistance.", "Motivate Runtime", MB_ICONSTOP);
}

// TODO 0x10003dd0 ?Shutdown@MfRuntime@@UAEHXZ: registers rotate through the vtable calls, the
// hook data is set up in another order and the early return gets its own epilogue (56% similar)
// Stops everything Initialize started and tells the hooks.
int MfRuntime::Shutdown()
{
    if (!mInitialized)
        return 0;
    if (g_world) {
        MfInterfaceRegistry::RemoveInterface("MfAiWorld");
        if (g_world)
            ((MfAiWorld *)g_world)->Release();
    }
    MfString errors;
    MfScPiccolo::bIsRunning = 0;
    MfPoShutdownClasses();
    MfPoLibrary::Shutdown();
    MfScPiccolo::Shutdown(&errors);
    if (gKeyboard)
        gKeyboard->Release();
    if (gMouse)
        gMouse->Release();
    g_msgDispatcher->Shutdown();
    delete g_msgDispatcher;
    MfSoundManager::Shutdown();
    MfRenderer::Shutdown();
    mInitialized = 0;
    MfRuntimeHookData data;
    data.value = 1;
    gHookManager->CallHooks('WRtS', &data);
    MfInterfaceRegistry::RemoveInterface("MfAiHookManager");
    gHookManager->Release();
    return 1;
}

// 0x10003f60 ?GetKeyboard@MfRuntime@@UAEPAVMfAiKeyboard@@XZ
MfAiKeyboard * MfRuntime::GetKeyboard()
{
    return g_paiKeyboard;
}

// 0x10003f70 ?GetMouse@MfRuntime@@UAEPAVMfAiMouse@@XZ
MfAiMouse * MfRuntime::GetMouse()
{
    return g_paiMouse;
}

// 0x10003f80 ?CreateWorld@MfRuntime@@UAEPAVMfAiWorld@@PAVMfAiAppWorld@@@Z
// Makes the world (it registers itself as g_world), starts it and gives it
// the handler of the views' events.
MfAiWorld *MfRuntime::CreateWorld(MfAiAppWorld *app)
{
    if (!mInitialized)
        return 0;
    new MfWorld(app);
    g_world->GoLive(0, 0);
    new MfViewEventHandler(g_world);
    return g_world;
}

// 0x10004050 ?GetWorld@MfRuntime@@UAEPAVMfAiWorld@@XZ
MfAiWorld *MfRuntime::GetWorld()
{
    return g_world;
}


// 0x10004060 ?EnumJoysticks@@YGHPAUDIDEVICEINSTANCEA@@PAX@Z
// DirectInput's device callback: the first two joysticks found.
static BOOL __stdcall EnumJoysticks(DIDEVICEINSTANCEA *instance, void *directInput)
{
    if (gJoysticks[0] && !gJoysticks[0]->mInitialized)
        return gJoysticks[0]->Initialize(instance, (IDirectInputA *)directInput);
    if (gJoysticks[1] && !gJoysticks[1]->mInitialized)
        return gJoysticks[1]->Initialize(instance, (IDirectInputA *)directInput);
    return 0;
}

// 0x100040c0 ?InitializeJoysticks@MfRuntime@@UAEHPAX0@Z
int MfRuntime::InitializeJoysticks(void *instance, void *window)
{
    gJoysticks[0] = new MfJoystick(1);
    gJoysticks[1] = new MfJoystick(2);
    gJoysticks[0]->SetWindow((HWND)window);
    gJoysticks[1]->SetWindow((HWND)window);
    MfPoJoystick::Initialize();
    IDirectInputA *directInput;
    HRESULT result = DirectInputCreateA((HINSTANCE)instance, 0x500, &directInput, 0);
    if (!result) {
        directInput->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoysticks, directInput, DIEDFL_ATTACHEDONLY);
        directInput->Release();
        gJoysticks[0]->ReacquireInput();
        gJoysticks[1]->ReacquireInput();
        return 1;
    } else {
        MfString message;
        if (result == DIERR_OLDDIRECTINPUTVERSION)
            message = "DirectInputCreate 5.0 FAILED because you have a beta version of DirectX - so there is no "
                      "joystick support";
        else
            message = "DirectInputCreate 5.0 FAILED - so there is no joystick support.";
        MfLogger::Log(0, "Motivate", "_InitJoySticks", message);
        return 0;
    }
}

// 0x10004280 ?PollJoysticks@MfRuntime@@UAEXXZ
void MfRuntime::PollJoysticks()
{
    if (gJoysticks[0]->mInitialized == 1)
        gJoysticks[0]->ReadInput();
    if (gJoysticks[1]->mInitialized == 1)
        gJoysticks[1]->ReadInput();
}

// 0x100042b0 ?LoadSound@MfRuntime@@UAEPAVMfSoundManager@@PAX0VMfString@@@Z
MfSoundManager *MfRuntime::LoadSound(void *window, void *instance, MfString name)
{
    MfSoundManager *manager = theSoundManager;
    if (name == NULL_STRING)
        manager->GetDefaultPlugin(name);
    if (name != NULL_STRING) {
        MfSoundDriverInitInfo info;
        info.mWindow = window;
        info.mInstance = instance;
        manager->LoadPlugin(name, &info);
    }
    return manager;
}

// 0x10004360 ?LoadRenderer@MfRuntime@@UAEPAVMfRenderer@@PAX0VMfString@@@Z
MfRenderer *MfRuntime::LoadRenderer(void *window, void *instance, MfString name)
{
    if (name == NULL_STRING)
        MfRenderer::GetDefaultPlugin(name);
    MfRendererInitInfo info;
    info.mWindow = window;
    info.mInstance = instance;
    info.m08 = 1;
    MfRenderer *renderer = MfRenderer::Instance();
    if (!renderer->LoadPlugin(name, &info))
        return 0;
    return renderer;
}

// 0x10004420 ?GetInterfaceName@MfApplication@@UAEXPADHAAH1@Z
void MfApplication::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    MfString interfaceName("ApplicationI");
    strncpy(name, interfaceName, size - 1);
    major = 1;
    minor = 0;
}

// 0x100044c0 ?Release@MfApplication@@UAEHXZ
int MfApplication::Release()
{
    if (--mRefCount == 0) {
        delete this;
        return 0;
    }
    return mRefCount;
}

// ---- the hook manager -------------------------------------------------------
#include "MfDblList.h"
#include "MfHookManager.h"
#include "MfIndex.h"

MfHookManager *gHookManager;

// one registered hook
struct MfHook {
    MfHook(MfHookFunc func, void *data) : mFunc(func), mData(data) {}
    MfHookFunc mFunc;
    void *mData;
};

// matches the hook that calls a given function
class MfHookFuncMatch : public MfCallBack {
public:
    virtual int CallBack(void *func, void *hook) { return func == ((MfHook *)hook)->mFunc; }
};

static MfDblList *s_hookLists;
static MfIndex *s_hookIndex;

// 0x10004500 ??0MfHookManager@@QAE@XZ
MfHookManager::MfHookManager()
{
    mRefCount = 1;
}

// 0x10004510 ??1MfHookManager@@QAE@XZ
MfHookManager::~MfHookManager()
{
}

// 0x10004520 ?GetInterfaceName@MfHookManager@@UAEXPADHAAH1@Z
void MfHookManager::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiHookManager", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// (folded with MfRuntime::AddRef)
int MfHookManager::AddRef()
{
    return InterlockedIncrement(&mRefCount);
}

// 0x10004560 ?Release@MfHookManager@@UAEHXZ
int MfHookManager::Release()
{
    int refs = InterlockedDecrement(&mRefCount);
    if (refs == 0 && this)
        delete this;
    return refs;
}

// 0x10004590 ?AddHook@MfHookManager@@UAEHKP6AHPAUMfHookData@@PAX@Z1W4MfHookPosition@@@Z
int MfHookManager::AddHook(unsigned long id, MfHookFunc func, void *data, MfHookPosition position)
{
    if (!s_hookLists)
        s_hookLists = new MfDblList[MF_NUM_FAST_HOOKS];
    if (!s_hookIndex)
        s_hookIndex = new MfIndex(53);
    MfDblList *list;
    if (id < MF_NUM_FAST_HOOKS)
        list = &s_hookLists[id];
    else {
        list = (MfDblList *)s_hookIndex->Find(id);
        if (!list) {
            list = new MfDblList;
            s_hookIndex->Add(id, list);
        }
    }
    MfHookFuncMatch match;
    if (list->Find(func, match))
        return 0;
    MfHook *hook = new MfHook(func, data);
    if (position == MF_HOOK_LAST)
        list->Append(hook);
    else
        list->Prepend(hook);
    return 1;
}

// 0x10004810 ?RemoveHook@MfHookManager@@UAEHKP6AHPAUMfHookData@@PAX@ZAAPAX@Z
int MfHookManager::RemoveHook(unsigned long id, MfHookFunc func, void *&data)
{
    if (!s_hookIndex || !s_hookLists)
        return 0;
    MfDblList *list;
    if (id < MF_NUM_FAST_HOOKS)
        list = &s_hookLists[id];
    else {
        list = (MfDblList *)s_hookIndex->Find(id);
        if (!list)
            return 0;
    }
    MfHookFuncMatch match;
    MfDblListItem *item = list->Find(func, match);
    if (!item)
        return 0;
    MfHook *hook = (MfHook *)list->GetCurrent(item);
    list->Delete(item);
    data = hook->mData;
    delete hook;
    if (id >= MF_NUM_FAST_HOOKS && list->GetCount() == 0) {
        s_hookIndex->Delete(id);
        delete list;
    }
    return 1;
}

// 0x10004910 ?CallHooks@MfHookManager@@UAEHKPAUMfHookData@@@Z
int MfHookManager::CallHooks(unsigned long id, MfHookData *data)
{
    if (!s_hookIndex || !s_hookLists)
        return 0;
    MfDblList *list;
    if (id < MF_NUM_FAST_HOOKS)
        list = &s_hookLists[id];
    else {
        list = (MfDblList *)s_hookIndex->Find(id);
        if (!list)
            return 0;
    }
    MfDblListItem *item;
    int result = 0;
    for (MfHook *hook = (MfHook *)list->GetFirst(item); hook && result <= 0; hook = (MfHook *)list->GetNext(item))
        result = hook->mFunc(data, hook->mData);
    return result;
}


// lists the plugins of a type (MfPluginLoader.cpp)
void MfFindPlugins(const MfString &prefix, const MfString &type, MfStringList &list);
int MfLoadPlugin(const MfString &type, const MfString &name, MfPluginInfo *info);

// TODO 0x10004990 ?LoadHookPlugins@@YAXXZ: the frame is 4 bytes smaller and the zero is kept
// in esi instead of ebx ()
// Loads the hook plugins (hoo*.dll) and lets each add its hooks.
static void LoadHookPlugins()
{
    MfStringList files;
    MfFindPlugins("hoo", "Hook Manager", files);
    MfHookManager manager;
    MfDblListItem *pli;
    for (MfString *file = files.First(pli); file; file = files.Next(pli)) {
        MfPluginInfo info;
        if (MfLoadPlugin("Hook Manager", *file, &info))
            info.mCreate((void **)&manager);
    }
}

// Matched by this code:
// 0x10004720 ?CallBack@MfHookFuncMatch@@UAEHPAX0@Z
// 0x10004740 ??_EMfDblList@@UAEPAXI@Z
// 0x100047a0 ??_TMfDblList@@QAEXXZ
// 0x100047c0 ??_GMfIndex@@UAEPAXI@Z
// 0x100047e0 ??1MfHookFuncMatch@@UAE@XZ