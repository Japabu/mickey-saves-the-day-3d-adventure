// MfMouse: RuntimeLib's mouse (RTTI name; unit 10090a70-10091030): the actors
// that hear of the clicks of each button, and the last pick of the views.
#ifndef RUNTIMELIB_MFMOUSE_H
#define RUNTIMELIB_MFMOUSE_H
#include "MfAiMouse.h"
#include "MfDblList.h"
#include "MfVector3.h"

class MfMouse : public MfAiMouse {
public:
    // the events an actor can be routed
    enum {
        LBUTTON_DOWN = 0,
        LBUTTON_UP = 1,
        RBUTTON_DOWN = 2,
        RBUTTON_UP = 3,
        MOUSE_MOVE = 4,
        ALL_EVENTS = 5
    };

    MfMouse();
    virtual ~MfMouse();

    // MfAi
    virtual void GetInterfaceName(char *name, int size, int &major, int &minor);
    virtual int AddRef();
    virtual int Release();

    // MfAiMouse
    virtual int SetMouseRoute(int button, MfAiActor *actor);
    virtual int ClearMouseRoute(int button, MfAiActor *actor);
    virtual MfAiActor *GetLastPickedActor() { return mPickedActor; }
    virtual MfAiActorNode *GetLastPickedFloor();

    void SetPick(MfAiActor *actor, MfVector3 point) { mPickedActor = actor; mPickPoint = point; }
    // sends the click of a button to the actors routed for it
    void Notify(int button);

    MfDblList mLButtonDownRoutes;                           // 0x04 MfLiveActors
    MfDblList mLButtonUpRoutes;                             // 0x14
    MfDblList mRButtonDownRoutes;                           // 0x24
    MfDblList mRButtonUpRoutes;                             // 0x34
    MfDblList mMouseMoveRoutes;                             // 0x44
    MfAiActor *mPickedActor;                                // 0x54 the actor last clicked
    MfVector3 mPickPoint;                                   // 0x58 where
    long mRefCount;                                         // 0x64
};

extern MfMouse *gMouse;

#endif
