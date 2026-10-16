// MfWorld: the world: the spaces, the registered actors, the clock; and the
// MfAiWorld the application sees. Layout from ??0MfWorld@@QAE@PAVMfAiAppWorld@@@Z;
// vtable order from ??_7MfWorld@@6BMfRootObj@@@ and ??_7MfWorld@@6BMfAiWorld@@@.
#ifndef RUNTIMELIB_MFWORLD_H
#define RUNTIMELIB_MFWORLD_H
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "MfAiWorld.h"
#include "MfCriticalSection.h"
#include "MfDblList.h"
#include "MfPropertyTable.h"
#include "MfRootObj.h"
#include "MfSpace.h"
#include "MfSpaceList.h"
#include "MfString.h"
class MfActor;
class MfAiAppWorld;
class MfLiveActor;

class __declspec(dllexport) MfWorld : public MfRootObj, public MfAiWorld {
public:
    typedef void (MfWorld::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'Worl' };

    MfWorld(MfAiAppWorld *app);
    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
    virtual void DefaultMsgHandler(MfMsg &msg);
    virtual int Type() { return TYPE; }
protected:
    virtual void InitMsgTable();
public:
    virtual ~MfWorld();
    virtual void RequestDestroyActor(MfLiveActor *actor);
    virtual void RequestDestroySpace(MfSpace *space);

    // MfAi
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    // MfAiWorld
    virtual int CreateUniqueID() { return iUniqueNumber++; }
    virtual void SetDataPath(const MfString &path);
    virtual void Start(int);
    virtual void Stop();
    virtual void Continue();
    virtual void Reset();
    virtual int IsRunning() { return mRunning; }
    virtual MfAiSpace *CreateSpace() { return NewSpace(); }
    virtual MfAiSpace *LoadSpace(const MfString &, long);
    virtual int CloseSpace(const MfAiSpace *);
    virtual int SaveSpace(const MfAiSpace *space) { return SaveSpace((MfSpace *)space); }
    virtual int RegisterActorPrototype(MfProtoActor *);
    virtual MfAiActor *InstantiateActor(MfProtoActor *);
    virtual MfAiActor *LoadActor(const MfString &, const MfAiSpace *, const MfTransform3 &, int);
    virtual int TeleportActor(MfAiActor *, const MfString &, int, const MfTransform3 &);
    virtual int LoadActorGroup(const MfString &);
    virtual void DeleteActor(const MfAiActor *);
    virtual int SaveActorGroup(const MfAiActor *, const MfString &);
    virtual int SaveActor(const MfAiActor *);
    virtual MfAiActor *GetActor(int);
    virtual void ProcessMessages();
    virtual int UpdateAllSpaces(unsigned long);
    virtual void GetAllSpaces(MfDblList *);
    virtual void SetRenderable(int on) { mRenderable = on; }
    virtual void DisableToolMouseBehavior();
    virtual void EnableToolMouseBehavior();
    virtual int GetProperty(const MfString &, MfProperty &);
    virtual int GetProperty(const MfString &, MfProperty **);
    virtual void SetProperty(MfProperty &);
    virtual void SetProperty(MfProperty *);
    virtual void GetPropertyNames(MfStringList *);
    virtual MfMaterial *CreateMaterial();
    virtual int GetInstances(MfProtoActor *, MfDblList &);

    static int CreateUniqueNumber() { return iUniqueNumber++; }
    static MfString ConstructActorFilePath(const MfString &);
    static MfString ConstructSpaceFilePath(const MfString &, int);
    static MfString GetSpaceFileName(const MfString &, int);

    MfAiAppWorld *App();
    MfSpaceList &SpaceList() { return mSpaceList; }
    int ActorExists(MfActor *actor) { return mActors.Find(actor) != 0; }
    int Renderable() { return mRenderable; }
    int StockMouseEnabled() { return mStockMouseEnabled; }
    unsigned long GetClock() { return mClock; }
    unsigned long AccumPauseTime() { return mAccumPauseTime; }
    void RegWorkerThreadAccess() { InterlockedIncrement(&mWorkerThreads); }
    void UnRegWorkerThreadAccess() { InterlockedDecrement(&mWorkerThreads); }
    MfLiveActor *CreateActor();
    MfSpace *NewSpace();
    int SaveSpace(MfSpace *space);
    void FlushSpaceList(MfSpace *space);
    void RegisterActor(MfActor *actor);
    int UnregisterActor(MfActor *actor);
    void GoLive(unsigned long clock, int);
    void GoAuthor(int);
    void ContinueLive(int);
    void HandleDestroyActorRequest(MfMsg &msg);
    void HandleDestroySpaceRequest(MfMsg &msg);
    void HandleTeleportActorRequest(MfMsg &msg);
    MfString FindPiccoloSourceByModule(MfString module);
    void WritePiccoloSourceByModule(MfString module, MfString source);

    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;
    static int enableNetwork;
protected:
    void _WaitForWorkerThreadsToFinish();
    static int iUniqueNumber;

public:
    MsgHandler *mMsgTable;                                  // 0x18
    long mWorkerThreads;                                    // 0x1c
    int mRenderable;                                        // 0x20
    int mRunning;                                           // 0x24
    unsigned long mClock;                                   // 0x28
    unsigned long mAccumPauseTime;                          // 0x2c
    unsigned long mPauseTime;                               // 0x30
    MfCriticalSection mCritSect;                            // 0x34
    MfSpaceList mSpaceList;                                 // 0x4c
    MfDblList mActors;                                      // 0x5c
    MfAiAppWorld *mApp;                                     // 0x6c
    int mStockMouseEnabled;                                 // 0x70
    int m74;                                                // 0x74
    MfPropertyTable mProperties;                            // 0x78
    long mRefCount;                                         // 0x90
};

extern MfWorld *g_world;

#endif
