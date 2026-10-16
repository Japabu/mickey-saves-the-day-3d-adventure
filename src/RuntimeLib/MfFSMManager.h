// MfFSMManager: runs an actor's hierarchical finite state machines (the
// actor's behaviour). Only what MfLiveActor uses is declared so far; class
// name and base (MfRootObj) from the RTTI, vtable ??_7MfFSMManager@@6B@
// (100b5b94, 33 slots). The class is not exported: member names are
// descriptive, and the empty virtuals (slots 15-20, 23-28, 30, 31: hooks
// that MfFSMPlayBack overrides) are named after their slot.
#ifndef RUNTIMELIB_MFFSMMANAGER_H
#define RUNTIMELIB_MFFSMMANAGER_H
#include "MfRootObj.h"
#include "MfDblList.h"
#include "MfString.h"
class MfFiniteStateMachine;
class MfFSMMutable;
class MfStateTransition;
class MfLiveActor;
class MfScDataHandle;
class MfScHandleDict;
class MfScVariableStack;
class MfStringList;

// the extension of behaviour files, ".bhvr" (100f5d20)
extern MfString gBhvrExtension;

// a behaviour load that waits for the manager's next run (mQueuedTimers)
struct MfFSMLoadRequest {
    MfFSMLoadRequest(MfString n, int a) : name(n), arg(a) {}
    MfString name;                                          // 0x00
    int arg;                                                // 0x04
};

class MfFSMManager : public MfRootObj {
public:
    typedef void (MfFSMManager::*MsgHandler)(MfMsg &msg);

    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
    virtual MfString GetName();
protected:
    virtual void InitMsgTable();
public:
    virtual ~MfFSMManager();
    // a message an actor sent (MfFSMPlayBack records them; empty here)
    virtual void RecordSentMsg(MfMsg *msg, unsigned long delay) {}
    // runs the state machines on an event message
    virtual void HandleEvent(MfMsg &msg);
    // the behaviour takes over the actor again (SetControlMode)
    virtual void Resume();
    virtual void Enable(int on);
    virtual int ActivateBehavior(MfString name);
    virtual int LoadBehavior(MfString name, int, int);
    virtual int RunQueuedEvents();                          // the list at 0x388
    virtual int RunQueuedTimers();                          // the list at 0x398
    virtual void Hook15() {}
    virtual int Hook16() { return 0; }
    virtual void Hook17(int) {}
    virtual void Hook18() {}
    virtual int Hook19() { return 0; }
    virtual void Hook20() {}
    virtual void Hook21(MfString) {}
    // unloads every behaviour (clears mBehaviors)
    virtual void ClearBehaviors();
    virtual int Hook23(MfString) { return 0; }
    virtual int Hook24() { return 0; }
    virtual void Hook25() {}
    virtual void Hook26() {}
    virtual void Hook27() {}
    virtual int Hook28() { return 0; }
    virtual void Hook29(MfString, MfString) {}
    virtual int Hook30(const MfString &, int, int) { return 0; }
    // MfFSMPlayBack writes the loaded behaviours to the file (empty here)
    virtual void SaveForPlayback(MfString &file) {}
    // compiles the loaded behaviours that are not compiled yet
    virtual void CompileBehaviors(int force, int useCompiled);

    MfFSMManager(MfString dir, MfScDataHandle *actorHandle, MfScDataHandle *spaceHandle, MfLiveActor *actor);  // 1007feb0

    // is the behaviour already loaded? (10080ae0)
    MfFiniteStateMachine *IsBehaviorLoaded(const MfString &name);
    // the part of a behaviour at path ('\\<file>\\<state>\\...')
    MfFSMMutable *FindPart(MfString path);                  // 10080930
    // the script source of the part at path
    MfString GetScript(MfString path);                      // 10080870

    void Start();
    // takes a behaviour the compiler loaded (10080b80)
    void AddMachine(MfFiniteStateMachine *machine);
    void SetSpaceHandle(MfScDataHandle *space);
    void SetActorHandle(MfScDataHandle *actor);
    void DeactivateBehavior(MfString name);
    // unregisters the compiled behaviour and unloads its machine (10081ee0)
    void UnloadBehavior(MfString name);
    // unloads the behaviour on the next run (mQueuedEvents) (100815f0)
    void QueueUnload(MfString name);
    // unloads the behaviour in the other instances of the prototype
    void UnloadInInstances(MfString name);                  // 10081e00
    // links the behaviour's machine again (10081d40)
    void LinkBehavior(MfString name);
    // links the behaviour if another instance of the prototype shares it
    int LinkIfShared(MfString name);                        // 10081ab0
    // links and initializes the behaviour (the first one if all is set)
    void ReinitBehavior(MfString name, int all);            // 10081c70
    // does ReinitBehavior in the other instances of the prototype
    void ReinitInInstances(MfString name, int all);         // 10081b80
    // logs that the compiler's object file could not be saved
    void ReportSaveError(const MfString &errorCode, const MfString &objectFile);    // 10081fc0
    // the path and name of the machine's current state (10082170)
    int GetCurStateName(MfString machine, MfString &path, MfString &name);
    // sets mActive when a state machine is active (10080790)
    void UpdateActive();
    void SetActorName(MfString name) { mActorName = name; }
    void SetActorClass(MfString name) { mActorClass = name; }
    // the .hfsm file of an old-style behaviour (MfLiveActor::OldLoadHFSM)
    void SetHfsmPath(MfString path) { m24 = path; }
    MfString LastSender() { return mLastSender; }
    MfString ActorName() { return mActorName; }
    MfString ActorClass() { return mActorClass; }
    MfString BhvrDir() { return mBhvrDir; }

    MsgHandler *mMsgTable;                                  // 0x14
    MfString mLastSender;                                   // 0x18 the sender of the last event
    MfDblList *mBehaviors;                                  // 0x1c the loaded behaviours
    MfFiniteStateMachine *mEditMachine;                     // 0x20
    MfString m24;                                           // 0x24
    MfString mActorName;                                    // 0x28 the actor's instance name
    MfString mActorClass;                                   // 0x2c the actor's (prototype) name
    MfString mBhvrDir;                                      // 0x30
    MfLiveActor *mActor;                                    // 0x34
    MfScHandleDict *mVariables;                             // 0x38 'actor', 'space'
    MfScVariableStack *mVarStack;                           // 0x3c
    int mActive;                                            // 0x40
    int m44, m48;                                           // 0x44
    int mEnabled;                                           // 0x4c
    int mStarting;                                          // 0x50
    int mHandling;                                          // 0x54
    int mEditing;                                           // 0x58 machines only lend their first state (mCopiedState)
    MfFSMMutable *mCopiedState;                             // 0x5c
    int mNumFired;                                          // 0x60
    MfStateTransition *mFired[(0x384 - 0x64) / 4];          // 0x64 the transitions the last event fired
    MfFiniteStateMachine *mLastLoaded;                      // 0x384
    MfDblList mQueuedEvents;                                // 0x388 machines to unload on the next run
    MfDblList mQueuedTimers;                                // 0x398
    int mRunning;                                           // 0x3a8
    int m3ac;                                               // 0x3ac

    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;

protected:
    void HandleRun(MfMsg &msg);
    void HandleStart(MfMsg &msg);
};

#endif
