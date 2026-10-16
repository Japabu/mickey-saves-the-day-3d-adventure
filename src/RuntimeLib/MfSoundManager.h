// MfSoundManager: plays the sounds of the actors through a sound driver
// plugin (MfISoundDriver). Layout from ??0MfSoundManager@@IAE@XZ (0x34 bytes).
#ifndef RUNTIMELIB_MFSOUNDMANAGER_H
#define RUNTIMELIB_MFSOUNDMANAGER_H
// PlaySound keeps its name (mmsystem.h would rename it)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "MfDblList.h"
#include "MfString.h"
#include "MfVector3.h"
class MfAiActor;
class MfAiActorNode;
class MfStringList;
class MfTransform3;
class MfISoundResource;

// what the sound driver is initialized with (MfRuntime::LoadSound)
struct MfSoundDriverInitInfo {
    void *mWindow;                                          // 0x00
    void *mInstance;                                        // 0x04
};

// what the driver reports to the callbacks
struct MfSoundDriverCallbackInfo {
    int mEvent;                                             // 0x00 0: the sound has finished
    MfISoundResource *mResource;                            // 0x04
};

// a sound of the driver (names of the methods are descriptive)
class MfISoundResource {
public:
    virtual void GetInterfaceName(char *, int, int &, int &) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual int QueryInterface(void *, void **) = 0;
    // 0 when the file could be loaded
    virtual int Load(const MfString &path) = 0;
    virtual void PlayLooped() = 0;
    virtual void Pause() = 0;
    virtual void Play() = 0;
    virtual void SetPosition(MfVector3 *position) = 0;
    virtual void SetVelocity(MfVector3 *velocity) = 0;
    // the sound's parameters (MfPoSoundResource): 0 volume, 1 pan, 2 cutoff
    // distance, 3 attenuation, 4 ambient, 5 stream, 6 loop, 7 duration,
    // 8 sampling rate, 9 sample size
    virtual void GetParameter(int which, float *value) = 0;
    // sets a parameter; 0 is the volume in dB (-100 silent, 0 full; MfSoundKeyframe)
    virtual void SetChannelVolume(int channel, float volume) = 0;
    virtual void *GetUserData() = 0;
    virtual void SetUserData(void *data) = 0;
};

// the sound driver plugin (_GetSoundDriver@4)
class MfISoundDriver {
public:
    virtual void GetInterfaceName(char *, int, int &, int &) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual int QueryInterface(void *, void **) = 0;
    // 0 when the driver could be initialized
    virtual int Initialize(MfSoundDriverInitInfo *info) = 0;
    virtual void SetCallback(void (__cdecl *callback)(MfSoundDriverCallbackInfo *)) = 0;
    virtual void Update() = 0;
    virtual MfISoundResource *CreateResource() = 0;
    // 0x20 and 0x28 are called without arguments, one after the other, by the
    // script method Sound.StopSound (meaning not known)
    virtual void Halt() = 0;
    virtual void SetDopplerFactor(float) = 0;
    virtual void Purge() = 0;
    virtual void SetListenerTransform(const MfTransform3 &transform) = 0;
    virtual void SetListenerVelocity(MfVector3 *velocity) = 0;
    // driver parameters (MfPoSoundDriver): 0 the volume, 1 the speed of sound
    virtual void GetParameter(int which, float *value) = 0;
    virtual void SetParameter(int which, float value) = 0;
};

class __declspec(dllexport) MfSoundManager {
public:
    // a sound being played
    struct SoundInfo {
        SoundInfo() {}
        ~SoundInfo() {}

        MfISoundResource *mResource;                        // 0x00
        MfAiActor *mActor;                                  // 0x04
        MfAiActorNode *mNode;                               // 0x08 where it sounds
        MfVector3 mLastPosition;                            // 0x0c
    };

    static MfSoundManager *Initialize();
    static void Shutdown();
    void GetPluginList(MfStringList &list);
    int LoadPlugin(const MfString &name, MfSoundDriverInitInfo *info);
    MfISoundDriver *GetDriver();
    int SetDriver(MfISoundDriver *driver, MfSoundDriverInitInfo *info);
    void AddCallback(void (__cdecl *callback)(MfSoundDriverCallbackInfo *));
    void RemoveCallback(void (__cdecl *callback)(MfSoundDriverCallbackInfo *));
    int GetDefaultPlugin(MfString &name);
    int SetDefaultPlugin(const MfString &name);
    void SetSharedPath(const MfString &path);
    void SetActorPath(const MfString &path);
    MfString FindSoundPath(const MfString &file);
    void PlaySound(MfISoundResource *sound, MfAiActor *actor, MfAiActorNode *node, int looped);
    MfISoundResource *PlaySound(const MfString &file, MfAiActor *actor, MfAiActorNode *node);
    void StopSound(MfISoundResource *sound);
    void StopSounds(MfAiActor *actor);
    void PauseAllSounds();
    void ResumeAllSounds();
    void StopAllSounds();
    MfAiActorNode *GetListener() { return mListener; }
    void SetListener(MfAiActorNode *node);
    void UpdateSounds();
protected:
    MfSoundManager();
    ~MfSoundManager();
    static void DriverCallback(MfSoundDriverCallbackInfo *info);
    static void ManagerCallback(MfSoundDriverCallbackInfo *info);
    void StopSound(SoundInfo *sound);
    void Warning(const MfString &title, const MfString &text);

public:
    MfISoundDriver *mDriver;                                // 0x00
    HINSTANCE mLibrary;                                     // 0x04 the plugin
    MfString mActorPath;                                    // 0x08 the sounds of the current actor
    MfString mSharedPath;                                   // 0x0c searched after mActorPath
    MfDblList mSounds;                                      // 0x10 SoundInfo
    MfAiActorNode *mListener;                               // 0x20
    MfVector3 mListenerPosition;                            // 0x24
    bool mPaused;                                           // 0x30
};

// the sound manager (MfSoundManager::Initialize creates it)
extern MfSoundManager *theSoundManager;

#endif
