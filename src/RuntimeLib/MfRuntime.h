// MfRuntime: the runtime object behind the MfAiRuntime interface that
// MfInitializeRuntime hands to the application.
#ifndef RUNTIMELIB_MFRUNTIME_H
#define RUNTIMELIB_MFRUNTIME_H

#include "MfAiRuntime.h"

class __declspec(dllexport) MfRuntime : public MfAiRuntime {
public:
    MfRuntime();
    virtual ~MfRuntime();
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual int Initialize(const char *, const char *);
    virtual int Shutdown();
    virtual MfAiKeyboard *GetKeyboard();
    virtual MfAiMouse *GetMouse();
    virtual MfAiWorld *CreateWorld(MfAiAppWorld *);
    virtual MfAiWorld *GetWorld();
    virtual int InitializeJoysticks(void *, void *);
    virtual void PollJoysticks();
    virtual MfSoundManager *LoadSound(void *, void *, MfString);
    virtual MfRenderer *LoadRenderer(void *, void *, MfString);
private:
    int VerifyLibraryVersions(MfString);
public:
    long mRefCount;                                         // 0x04
    int mInitialized;                                       // 0x08
};

__declspec(dllexport) void MfInitializeRuntime(MfAiRuntime **runtime);

#endif
