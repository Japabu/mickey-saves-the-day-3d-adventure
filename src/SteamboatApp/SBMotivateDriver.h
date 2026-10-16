#pragma once
// SBMotivateDriver.h: the game's connection to the Motivate runtime: it owns the AI world,
// runtime and current space and installs the hooks through which Motivate calls the game.
// Declaration from SteamboatApp.pdb; the space and world accessors are inline.
#include "MBInterfaces.h"
#include "MfAi.h"

class MfAiRuntime;
class RenderApp;
class ViewEventHandler;
struct MfActorHookData;
struct MfActorRWHookData;
struct MfFileHookData;
struct MfSpaceHookData;
struct MfUserHookData;

class SBMotivateDriver {
public:
    virtual ~SBMotivateDriver();
    static SBMotivateDriver *__cdecl GetInstance();
    MfAiSpace *Init(IMBGameEngine *engine, const MBString &mofaDir);
    static void __cdecl Update();
    static MfAiSpace *__cdecl GetMotivateSpace() { return mMofaSpace; }
    static MfAiWorld *__cdecl GetMotivateWorld() { return mMofaWorld; }
    static MfAiActorNode *__cdecl GetMfCameraNode();
    static MfAiRuntime *__cdecl GetMfRuntime();
    static int __cdecl LoadSpaceHook(MfSpaceHookData *hook, void *This);
    static int __cdecl DeleteSpaceHook(MfSpaceHookData *hook, void *This);
    static int __cdecl LoadActorHook(MfActorHookData *hook, void *This);
    static int __cdecl UserEventHook(MfUserHookData *hook, void *This);
    static int __cdecl TeleportHook(MfActorHookData *hook, void *This);
    static int __cdecl DeleteActorHook(MfActorHookData *hook, void *This);
    static int __cdecl FileDestinationHook(MfFileHookData *hook, void *This);
    static int __cdecl OverrideReadActorHook(MfActorRWHookData *hook, void *This);
    static int __cdecl AddActorHook(MfActorHookData *hook, void *This);
    static int __cdecl CreateActorHook(MfActorHookData *hook, void *This);
    static bool __cdecl InitializeLock();
    static void __cdecl AcquireLock() {}               // no out-of-line copy in the binary: inline and empty
    static void __cdecl ReleaseLock() {}
    static void __cdecl TerminateLock();
protected:
    SBMotivateDriver();
    static MfAiWorld *mMofaWorld;
    static MfAiRuntime *mMofaRuntime;
    static MfAiSpace *mMofaSpace;
    static RenderApp *mMofaApp;
    static ViewEventHandler *mMofaEventHandler;
    static IMBGameEngine *mEngine;
    static void *sMutex;
private:
    static SBMotivateDriver *_instance;
};
