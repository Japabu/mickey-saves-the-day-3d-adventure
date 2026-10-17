// MfHookManager: the MfAiHookManager that MfRuntime::Initialize creates (RTTI
// name; vtable 100b2484). Hooks of the well-known IDs (enums.h) live in an
// array of lists, other IDs get a list in an index on first use. The runtime
// calls the hooks with the data structures below; the MfHookData family is
// declared in the SteamboatApp PDB, MfMiscHookData and MfActivityHookData
// (MF_HOOK_MISC_DATA, MF_HOOK_ACTIVITY_DATA) are not, so their names and
// member names are descriptive.
#ifndef RUNTIMELIB_MFHOOKMANAGER_H
#define RUNTIMELIB_MFHOOKMANAGER_H
#include "MfAiHookManager.h"
#include "MfProperty.h"
#include "MfRotation3.h"
#include "MfString.h"
#include "MfVector3.h"
#include "enums.h"
class MfAiActivity;
class MfAiActor;
class MfAiActorNode;
class MfAiSpace;
class MfAiWorld;
class MfProtoActor;
class MfRenderView;

typedef int (__cdecl *MfHookFunc)(MfHookData *, void *);

class MfHookManager : public MfAiHookManager {
public:
    MfHookManager();
    ~MfHookManager();
    virtual void GetInterfaceName(char *, int, int &, int &);
    virtual int AddRef();
    virtual int Release();
    virtual int AddHook(unsigned long id, MfHookFunc func, void *data, MfHookPosition position);
    virtual int RemoveHook(unsigned long id, MfHookFunc func, void *&data);
    // calls the hooks of the ID until one returns a positive value; returns it
    virtual int CallHooks(unsigned long id, MfHookData *data);

    long mRefCount;                                         // 0x04
};

// the hook manager (MfRuntime::Initialize)
extern MfHookManager *gHookManager;

struct MfHookData {
    int reserved[4];                                        // 0x00
};

struct MfMouseHookData : public MfHookData {
    MfRenderView *eventView;                                // 0x10
    float x;                                                // 0x14
    float y;                                                // 0x18
    unsigned int flags;                                     // 0x1c
    short wheelDelta;                                       // 0x20
    MfVector3 pos;                                          // 0x24
    MfVector3 dir;                                          // 0x30
    MfAiSpace *pSpace;                                      // 0x3c
};

struct MfActorHookData : public MfHookData {
    int rid;                                                // 0x10
    int hid;                                                // 0x14
    MfRequestStatus status;                                 // 0x18
    MfAiActor *pActor;                                      // 0x1c
    MfAiSpace *pSpace;                                      // 0x20
    MfString name;                                          // 0x24
};

struct MfSpaceHookData : public MfHookData {
    int rid;                                                // 0x10
    int hid;                                                // 0x14
    MfRequestStatus status;                                 // 0x18
    MfAiSpace *pSpace;                                      // 0x1c
    MfString name;                                          // 0x20
};

struct MfUserHookData : public MfHookData {
    MfString event;                                         // 0x10
    MfString data;                                          // 0x14
    void *userData;                                         // 0x18
};

struct MfFileHookData : public MfHookData {
    void *fileHandle;                                       // 0x10
    MfString fileLocation;                                  // 0x14
    int mode;                                               // 0x18
};

struct MfActorRWHookData : public MfHookData {
    MfString filePath;                                      // 0x10
    MfAiActor *pActor;                                      // 0x14
    MfProtoActor *pProto;                                   // 0x18
};

// MF_HOOK_UPDATE_WORLD: the world is about to update its spaces
struct MfWorldHookData : public MfHookData {
    MfAiWorld *pWorld;                                      // 0x10
    unsigned long clock;                                    // 0x14
};

// MF_HOOK_GUARD_COP: an actor is about to be locked or unlocked by another
// (MfLiveActor::ApplyLock); a positive hook result refuses it
struct MfGuardHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    MfAiActorNode *pLocker;                                 // 0x14 the locker's root node, 0 to unlock
};

// 'WlAR': the lock request is granted unless a hook clears allow
struct MfLockHookData : public MfHookData {
    int allow;                                              // 0x10
    MfAiActor *pLocker;                                     // 0x14
    MfAiActor *pActor;                                      // 0x18
};

// 'WAcE': an application event of an actor (MfLiveActor::UpdateAppEventHandler)
struct MfAppEventHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    int event;                                              // 0x14
    int data;                                               // 0x18
};

// MF_HOOK_CAMERA_HAS_CHANGED: a camera node changed (MfCameraNode::SyncAllViews)
struct MfCameraHookData : public MfHookData {
    MfAiActorNode *pCamera;                                 // 0x10
    unsigned int flags;                                     // 0x14 1 moved, 2 changed, 3 zoomed
};

// 'WRtR', 'WRtS': the world has started (MfWorld::MfWorld), the runtime
// shuts down (MfRuntime::Shutdown)
struct MfRuntimeHookData : public MfHookData {
    int value;                                              // 0x10 1
};

// MF_HOOK_MISC_DATA: an actor sent an event or a message, set a property ...
// (MfWorld::enableNetwork mirrors the runtime over the network)
struct MfMiscHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    const MfAiActor *pTarget;                               // 0x14
    int m18;                                                // 0x18
    int kind;                                               // 0x1c 7 SendEvent, 8 SendMessage
    MfString str;                                           // 0x20
    unsigned long delay;                                    // 0x24
    MfProperty property;                                    // 0x28
    MfVector3 position;                                     // 0x34
    MfRotation3 rotation;                                   // 0x40
};

// MF_HOOK_ACTIVITY_DATA: an activity was set, added or deleted
struct MfActivityHookData : public MfHookData {
    MfAiActor *pActor;                                      // 0x10
    const MfAiActivity *pActivity;                          // 0x14
    int activityID;                                         // 0x18
    int m1c;                                                // 0x1c
    int op;                                                 // 0x20 0 set, 2 deleted, 3 finished
};

#endif
