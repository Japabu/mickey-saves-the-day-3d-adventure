// MfProximityManager: watches the proximity definitions of a space's actors
// and sends the actors proximity events (RTTI name, vtable 100b70bc; unit
// 10091030-10092230). RuntimeLib internal: the method names are descriptive.
// The pairs of actors in range and in contact are kept in two indexes, keyed
// by the actors' IDs (first << 16 | second).
#ifndef RUNTIMELIB_MFPROXIMITYMANAGER_H
#define RUNTIMELIB_MFPROXIMITYMANAGER_H
#include "MfLiveActorList.h"
#include "MfRootObj.h"
class MfHookManager;
class MfIndex;
class MfLiveActor;
class MfMsgDataBlock;
class MfProtoProxDef;
class MfSpace;

class MfProximityManager : public MfRootObj {
public:
    typedef void (MfProximityManager::*MsgHandler)(MfMsg &msg);
    enum { TYPE = 'PrxM' };

    MfProximityManager(MfSpace *space);
    ~MfProximityManager();

    virtual void CatchMsg(MfMsg &msg)
    {
        MsgHandler handler = mMsgTable[msg.mID];
        (this->*handler)(msg);
    }
    virtual MfString GetName() { return MfString("ProximityManager"); }
    virtual int Type() { return TYPE; }
protected:
    virtual void InitMsgTable();

public:
    void HandleTimer(MfMsg &msg);
    void ContinueLive();
    void GoAuthor();
    // checks every pair of watched actors, a quarter of them per call
    void Update();
    void AddActor(MfLiveActor *actor);
    void RemoveActor(MfLiveActor *actor);

protected:
    MfMsgDataBlock *MakeEventData(int event, MfLiveActor *actor, MfLiveActor *other);
    void CheckRange(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight, float radius,
                    float otherRadius);
    void CheckContact(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight);
    int InRange(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight, float radius,
                float otherRadius);
    int InContact(MfLiveActor *actor, MfLiveActor *other, MfProtoProxDef *def, float otherHeight);
    int IsPaired(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other);
    void RemovePairs(MfIndex *pairs, MfLiveActor *actor);
    void *AddPair(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other);
    void *RemovePair(MfIndex *pairs, MfLiveActor *actor, MfLiveActor *other);
    void ClearPairs(MfIndex *pairs);
    MfProtoProxDef *FindProxDef(MfLiveActor *actor, MfLiveActor *other);

public:
    static MsgHandler aMethods[NUM_MSGS];
    static int bMsgTableIsSet;

    MsgHandler *mMsgTable;                                  // 0x14
    MfSpace *mSpace;                                        // 0x18
    int mTimer;                                             // 0x1c -1: not running
    MfIndex *mInRange;                                      // 0x20
    MfIndex *mInContact;                                    // 0x24
    MfLiveActorList mActors;                                // 0x28 the actors with proximity definitions
    int mLive;                                              // 0x3c
    MfHookManager *mHooks;                                  // 0x40
    int mQuarter;                                           // 0x44 0-3, the part Update checks
};

#endif
