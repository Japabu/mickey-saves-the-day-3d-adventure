// MfSoundManager (RuntimeLib.dll 10004b20-10005f80): the sounds of the actors
// through a sound driver plugin.
#include "MfSoundManager.h"
#include "MfAiActor.h"
#include "MfAiActorNode.h"
#include "MfLiveActor.h"
#include "MfLogger.h"
#include "MfMsg.h"
#include "MfMsgDispatcher.h"
#include "MfStringList.h"
#include "MfTransform3.h"
#include "UtilityApp.h"

// mfutil
class __declspec(dllimport) MfDirectoryFileInfo {
public:
    MfString GetFileName() const;
};

class __declspec(dllimport) MfDirectory : public MfDblList {
public:
    MfDirectory();
    virtual ~MfDirectory();
    int Open(const MfString &path);
    void Close();
    MfDirectoryFileInfo *First()
    {
        mCurrent = pliFirst;
        if (mCurrent)
            return (MfDirectoryFileInfo *)mCurrent->pData;
        return 0;
    }
    MfDirectoryFileInfo *Next()
    {
        if (mCurrent) {
            mCurrent = mCurrent->pliNext;
            if (mCurrent)
                return (MfDirectoryFileInfo *)mCurrent->pData;
        }
        return 0;
    }

    MfString mPath;                                         // 0x10
    int m14;                                                // 0x14
    MfDblListItem *mCurrent;                                // 0x18
};

class __declspec(dllimport) MfRegularExpression {
public:
    MfRegularExpression(const MfString &pattern);
    virtual ~MfRegularExpression();
    void Optimize();
    int Matches(const MfString &s);

    char m04[0x80];                                         // 0x04
};

class __declspec(dllimport) MfPathSearch {
public:
    MfPathSearch(const MfString &file, const MfString &path);
    virtual ~MfPathSearch();
    MfString Search();

    MfString mFile;                                         // 0x04
    MfString mPath;                                         // 0x08
};


// 0x10004b20 _$E28 (original $E: the static initializers of soundSection)
// 0x10004b30 _$E25
// 0x10004b50 _$E27
// 0x10004b60 _$E26
static MfString soundSection("Sound Options");
// 0x10004b70 _$E33 (original $E: the static initializers of soundKey)
// 0x10004b80 _$E30
// 0x10004ba0 _$E32
// 0x10004bb0 _$E31
static MfString soundKey("SoundDriver");

MfSoundManager *theSoundManager;

// 0x10004bc0 ?Initialize@MfSoundManager@@SAPAV1@XZ
MfSoundManager *MfSoundManager::Initialize()
{
    if (!theSoundManager)
        theSoundManager = new MfSoundManager;
    return theSoundManager;
}

// 0x10004c30 ?Shutdown@MfSoundManager@@SAXXZ
void MfSoundManager::Shutdown()
{
    if (theSoundManager) {
        theSoundManager->StopAllSounds();
        delete theSoundManager;
        theSoundManager = 0;
    }
}

// 0x10004c70 ??0MfSoundManager@@IAE@XZ
MfSoundManager::MfSoundManager()
    : mDriver(0), mLibrary(0), mListener(0), mPaused(false)
{
}

// 0x10004ce0 ??1MfSoundManager@@IAE@XZ
MfSoundManager::~MfSoundManager()
{
    if (mDriver)
        mDriver->Release();
    if (mLibrary)
        FreeLibrary(mLibrary);
}

// EQUIV 0x10004d60 ?GetPluginList@MfSoundManager@@QAEXAAVMfStringList@@@Z: reviewed: register allocation: the loop step goes through eax and a copy to ecx in the original, ecx directly here; identical otherwise
void MfSoundManager::GetPluginList(MfStringList &list)
{
    MfDirectory directory;
    UtilityApp app;
    MfString path = app.PluginDirectory();
    if (path == NULL_STRING || !directory.Open(path)) {
        MfString title("Plugin Loader");
        Warning(title, MfString("There are no plugin directories defined.") +
                           MfString("Plugin directories are specified in the") +
                           MfString("PLUGINS/PLUGINSD registry entry."));
        return;
    }
    MfRegularExpression expression("^Snd.*\\.dll$");
    expression.Optimize();
    for (MfDirectoryFileInfo *info = directory.First(); info; info = directory.Next()) {
        MfString name = info->GetFileName();
        if (expression.Matches(name) == 1)
            list.Add(name);
    }
    directory.Close();
}

// 0x10005000 ?LoadPlugin@MfSoundManager@@QAEHABVMfString@@PAUMfSoundDriverInitInfo@@@Z
int MfSoundManager::LoadPlugin(const MfString &name, MfSoundDriverInitInfo *info)
{
    if (mDriver)
        return 0;
    UtilityApp app;
    MfString directory = app.PluginDirectory();
    MfString path;
    if (directory != NULL_STRING)
        path = directory + MfString("\\") + name;
    else
        path = name;
    mLibrary = LoadLibrary(path);
    if (!mLibrary) {
        MfString message = MfString("Could not find sound driver plugin: ") + name;
        Warning("Plugin Loader", message);
        return 0;
    }
    typedef int (__stdcall *GetSoundDriverFunc)(MfISoundDriver **driver);
    GetSoundDriverFunc getSoundDriver = (GetSoundDriverFunc)GetProcAddress(mLibrary, "_GetSoundDriver@4");
    if (!getSoundDriver) {
        MfString message = MfString("Could not find entry point in sound driver plugin: ") + name;
        Warning("Plugin Loader", message);
        FreeLibrary(mLibrary);
        mLibrary = 0;
        return 0;
    }
    MfISoundDriver *driver;
    if (!getSoundDriver(&driver)) {
        MfString message = MfString("Could not obtain interface from sound driver plugin: ") + name;
        Warning("Plugin Loader", message);
        FreeLibrary(mLibrary);
        mLibrary = 0;
        return 0;
    }
    if (!SetDriver(driver, info)) {
        driver->Release();
        return 0;
    }
    return 1;
}

// 0x100052e0 ?GetDriver@MfSoundManager@@QAEPAVMfISoundDriver@@XZ
MfISoundDriver *MfSoundManager::GetDriver()
{
    return mDriver;
}

// 0x100052f0 ?SetDriver@MfSoundManager@@QAEHPAVMfISoundDriver@@PAUMfSoundDriverInitInfo@@@Z
int MfSoundManager::SetDriver(MfISoundDriver *driver, MfSoundDriverInitInfo *info)
{
    if (mDriver || !driver)
        return 0;
    if (driver->Initialize(info)) {
        MfString text("Could not initialize sound driver.");
        MfString title("Set Driver");
        Warning(title, text);
        return 0;
    }
    mDriver = driver;
    driver->SetCallback(DriverCallback);
    AddCallback(ManagerCallback);
    return 1;
}

// the callbacks the driver's events are passed to
// 0x100053d0 _$E38 (original $E: the static initializers of callbacks)
// 0x100053e0 _$E35
// 0x10005400 _$E37
// 0x10005410 _$E36
static MfDblList callbacks;

typedef void (__cdecl *MfSoundCallback)(MfSoundDriverCallbackInfo *);

// 0x10005420 ?DriverCallback@MfSoundManager@@KAXPAUMfSoundDriverCallbackInfo@@@Z
void MfSoundManager::DriverCallback(MfSoundDriverCallbackInfo *info)
{
    MfDblListItem *pos;
    for (MfSoundCallback callback = (MfSoundCallback)callbacks.GetFirst(pos); callback;
         callback = (MfSoundCallback)callbacks.GetNext(pos))
        callback(info);
}

// 0x10005460 ?ManagerCallback@MfSoundManager@@KAXPAUMfSoundDriverCallbackInfo@@@Z
void MfSoundManager::ManagerCallback(MfSoundDriverCallbackInfo *info)
{
    SoundInfo *sound = (SoundInfo *)info->mResource->GetUserData();
    if (info->mEvent == 0 && sound) {
        if (sound->mActor) {
            MfActor *actor = (MfActor *)sound->mActor;
            g_msgDispatcher->Post(new MfMsg(MSG_SOUND_FINISHED, 0, 0, 0, actor));
        }
        theSoundManager->StopSound(sound);
    }
}

// 0x10005500 ?AddCallback@MfSoundManager@@QAEXP6AXPAUMfSoundDriverCallbackInfo@@@Z@Z
void MfSoundManager::AddCallback(void (__cdecl *callback)(MfSoundDriverCallbackInfo *))
{
    if (!callbacks.Find(callback))
        callbacks.Append(callback);
}

// 0x10005530 ?RemoveCallback@MfSoundManager@@QAEXP6AXPAUMfSoundDriverCallbackInfo@@@Z@Z
void MfSoundManager::RemoveCallback(void (__cdecl *callback)(MfSoundDriverCallbackInfo *))
{
    MfDblListItem *pos = callbacks.Find(callback);
    if (pos)
        callbacks.Delete(pos);
}

// 0x10005560 ?GetDefaultPlugin@MfSoundManager@@QAEHAAVMfString@@@Z
int MfSoundManager::GetDefaultPlugin(MfString &name)
{
    UtilityApp app;
    MfString none(NULL_STRING);
    name = app.GetProfileString(soundSection, soundKey, none);
    return 1;
}

// 0x10005600 ?SetDefaultPlugin@MfSoundManager@@QAEHABVMfString@@@Z
int MfSoundManager::SetDefaultPlugin(const MfString &name)
{
    UtilityApp app;
    app.WriteProfileString(soundSection, soundKey, name);
    return 1;
}

// 0x10005650 ?SetSharedPath@MfSoundManager@@QAEXABVMfString@@@Z
void MfSoundManager::SetSharedPath(const MfString &path)
{
    mSharedPath = path;
}

// 0x10005670 ?SetActorPath@MfSoundManager@@QAEXABVMfString@@@Z
void MfSoundManager::SetActorPath(const MfString &path)
{
    mActorPath = path;
}

// 0x10005690 ?FindSoundPath@MfSoundManager@@QAE?AVMfString@@ABV2@@Z
MfString MfSoundManager::FindSoundPath(const MfString &file)
{
    MfString path(NULL_STRING);
    if (mActorPath != NULL_STRING) {
        path = mActorPath + MfString("\\") + file;
        HANDLE handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (handle == INVALID_HANDLE_VALUE) {
            MfPathSearch search(file, mSharedPath);
            return search.Search();
        }
        CloseHandle(handle);
    }
    return path;
}

// EQUIV 0x10005800 ?PlaySound@MfSoundManager@@QAEXPAVMfISoundResource@@PAVMfAiActor@@PAVMfAiActorNode@@H@Z: reviewed: the new SoundInfo and the sound swap esi/edi throughout; every instruction is otherwise the same
void MfSoundManager::PlaySound(MfISoundResource *sound, MfAiActor *actor, MfAiActorNode *node, int looped)
{
    SoundInfo *info = new SoundInfo;
    info->mResource = sound;
    info->mActor = actor;
    if (actor && !node)
        node = actor->GetRootNode();
    info->mNode = node;
    mSounds.Add(info);
    sound->AddRef();
    sound->SetUserData(info);
    UpdateSounds();
    if (!mPaused) {
        if (looped)
            sound->PlayLooped();
        else
            sound->Play();
    }
}

// 0x100058a0 ?PlaySound@MfSoundManager@@QAEPAVMfISoundResource@@ABVMfString@@PAVMfAiActor@@PAVMfAiActorNode@@@Z
MfISoundResource *MfSoundManager::PlaySound(const MfString &file, MfAiActor *actor, MfAiActorNode *node)
{
    if (!mDriver)
        return 0;
    if (actor)
        SetActorPath(((MfLiveActor *)actor)->SoundPath());
    MfString path = FindSoundPath(file);
    if (path == NULL_STRING)
        return 0;
    MfISoundResource *sound = mDriver->CreateResource();
    sound->SetUserData(0);
    if (sound->Load(path)) {
        sound->Release();
        return 0;
    }
    PlaySound(sound, actor, node, 1);
    return sound;
}

// 0x100059a0 ?StopSound@MfSoundManager@@QAEXPAVMfISoundResource@@@Z
void MfSoundManager::StopSound(MfISoundResource *sound)
{
    MfDblListItem *pos;
    for (SoundInfo *info = (SoundInfo *)mSounds.GetFirst(pos); info; info = (SoundInfo *)mSounds.GetNext(pos)) {
        if (info->mResource == sound) {
            StopSound(info);
            return;
        }
    }
}

// 0x100059f0 ?StopSound@MfSoundManager@@IAEXPAUSoundInfo@1@@Z
void MfSoundManager::StopSound(SoundInfo *sound)
{
    MfDblListItem *pos = mSounds.Find(sound);
    mSounds.Delete(pos);
    sound->mResource->SetUserData(0);
    sound->mResource->Pause();
    sound->mResource->Release();
    delete sound;
}

// EQUIV 0x10005a50 ?StopSounds@MfSoundManager@@QAEXPAVMfAiActor@@@Z: reviewed: helper inlined vs called: the original calls the local MfDblList's Append through its vtable (slot 2, mfutil's Append = Insert(pData, LP_AFTER, pliLast)); RuntimeLib/MfDblList.h defines Append inline with that body, so this calls the virtual Insert(info, 1, pliLast) directly. The list is a local of exact type, so the same function runs; the rest is identical (shifted)
void MfSoundManager::StopSounds(MfAiActor *actor)
{
    MfDblList stopped;
    MfDblListItem *pos;
    SoundInfo *info;
    for (info = (SoundInfo *)mSounds.GetFirst(pos); info; info = (SoundInfo *)mSounds.GetNext(pos)) {
        if (info->mActor == actor)
            stopped.Append(info);
    }
    for (info = (SoundInfo *)stopped.GetFirst(pos); info; info = (SoundInfo *)stopped.GetNext(pos))
        StopSound(info);
}

// 0x10005b30 ?PauseAllSounds@MfSoundManager@@QAEXXZ
void MfSoundManager::PauseAllSounds()
{
    if (mPaused)
        return;
    mPaused = true;
    MfDblListItem *pos;
    for (SoundInfo *info = (SoundInfo *)mSounds.GetFirst(pos); info; info = (SoundInfo *)mSounds.GetNext(pos))
        info->mResource->Pause();
}

// 0x10005b70 ?ResumeAllSounds@MfSoundManager@@QAEXXZ
void MfSoundManager::ResumeAllSounds()
{
    if (!mPaused)
        return;
    mPaused = false;
    MfDblListItem *pos;
    for (SoundInfo *info = (SoundInfo *)mSounds.GetFirst(pos); info; info = (SoundInfo *)mSounds.GetNext(pos))
        info->mResource->Play();
}

// 0x10005bb0 ?StopAllSounds@MfSoundManager@@QAEXXZ
void MfSoundManager::StopAllSounds()
{
    MfDblList sounds;
    sounds.Copy(mSounds);
    MfDblListItem *pos;
    for (SoundInfo *info = (SoundInfo *)sounds.GetFirst(pos); info; info = (SoundInfo *)sounds.GetNext(pos))
        StopSound(info);
    mPaused = false;
}

// 0x10005c50 ?SetListener@MfSoundManager@@QAEXPAVMfAiActorNode@@@Z
void MfSoundManager::SetListener(MfAiActorNode *node)
{
    mListener = node;
    if (node)
        mListenerPosition = node->GetGlobalTransform().translation();
}

// 0x10005c80 ?UpdateSounds@MfSoundManager@@QAEXXZ
void MfSoundManager::UpdateSounds()
{
    MfDblList *sounds = &mSounds;
    MfDblListItem *pos;
    for (SoundInfo *info = (SoundInfo *)sounds->GetFirst(pos); info; info = (SoundInfo *)sounds->GetNext(pos)) {
        if (info->mNode) {
            MfTransform3 local, home, global;
            info->mNode->GetTransform(local, home, global);
            MfVector3 position = global.translation();
            info->mResource->SetPosition(&position);
            info->mResource->SetVelocity(&(position - info->mLastPosition));
            info->mLastPosition = position;
        }
    }
    if (mListener && mDriver) {
        mDriver->SetListenerTransform(mListener->GetGlobalTransform());
        MfVector3 position = mListener->GetGlobalTransform().translation();
        mDriver->SetListenerVelocity(&(position - mListenerPosition));
        mListenerPosition = position;
    }
    if (mDriver)
        mDriver->Update();
}

// 0x10005eb0 ?Warning@MfSoundManager@@IAEXABVMfString@@0@Z
void MfSoundManager::Warning(const MfString &title, const MfString &text)
{
    MfString source("Sound Manager");
    MfLogger::Log(1, source, title, text);
}
