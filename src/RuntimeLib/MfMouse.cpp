// MfMouse (RuntimeLib.dll 10090a70-10091030).
#include "MfMouse.h"
#include <string.h>
#include "MfFloor.h"
#include "MfFSMManager.h"
#include "MfInterfaceRegistry.h"
#include "MfLiveActor.h"
#include "MfMsg.h"

extern __declspec(dllimport) MfAiMouse *g_paiMouse;

// the message of a click to the behaviours
enum { MSG_MOUSE_EVENT = 0x27 };

MfMouse *gMouse;

// 0x10090a70 ??0MfMouse@@QAE@XZ
MfMouse::MfMouse()
{
    g_paiMouse = this;
    gMouse = this;
    (&mLButtonDownRoutes)->Clear();
    (&mLButtonUpRoutes)->Clear();
    (&mRButtonDownRoutes)->Clear();
    (&mRButtonUpRoutes)->Clear();
    (&mMouseMoveRoutes)->Clear();
    mPickedActor = 0;
    mRefCount = 0;
    MfInterfaceRegistry::RemoveInterface(MfString("MfAiMouse"));
    MfInterfaceRegistry::AddInterface(this);
}

// 0x10090bc0 ??1MfMouse@@UAE@XZ
MfMouse::~MfMouse()
{
    MfInterfaceRegistry::RemoveInterface(MfString("MfAiMouse"));
}

// 0x10090c70 ?SetMouseRoute@MfMouse@@UAEHHPAVMfAiActor@@@Z
int MfMouse::SetMouseRoute(int event, MfAiActor *actor)
{
    if (!actor)
        return 0;
    MfDblList *routes;
    if (event == LBUTTON_DOWN || event == ALL_EVENTS) {
        routes = &mLButtonDownRoutes;
        if (!routes->Find((MfActor *)actor))
            routes->Add((MfActor *)actor);
    }
    if (event == LBUTTON_UP || event == ALL_EVENTS) {
        routes = &mLButtonUpRoutes;
        if (!routes->Find((MfActor *)actor))
            routes->Add((MfActor *)actor);
    }
    if (event == RBUTTON_DOWN || event == ALL_EVENTS) {
        routes = &mRButtonDownRoutes;
        if (!routes->Find((MfActor *)actor))
            routes->Add((MfActor *)actor);
    }
    if (event == RBUTTON_UP || event == ALL_EVENTS) {
        routes = &mRButtonUpRoutes;
        if (!routes->Find((MfActor *)actor))
            routes->Add((MfActor *)actor);
    }
    if (event == MOUSE_MOVE || event == ALL_EVENTS) {
        routes = &mMouseMoveRoutes;
        if (!routes->Find((MfActor *)actor))
            routes->Add((MfActor *)actor);
    }
    return 1;
}

// 0x10090d60 ?ClearMouseRoute@MfMouse@@UAEHHPAVMfAiActor@@@Z
int MfMouse::ClearMouseRoute(int event, MfAiActor *actor)
{
    if (!actor)
        return 0;
    MfDblList *routes;
    MfDblListItem *pli;
    if (event == LBUTTON_DOWN || event == ALL_EVENTS) {
        routes = &mLButtonDownRoutes;
        pli = routes->Find((MfActor *)actor);
        if (pli)
            routes->Delete(pli);
    }
    if (event == LBUTTON_UP || event == ALL_EVENTS) {
        routes = &mLButtonUpRoutes;
        pli = routes->Find((MfActor *)actor);
        if (pli)
            routes->Delete(pli);
    }
    if (event == RBUTTON_DOWN || event == ALL_EVENTS) {
        routes = &mRButtonDownRoutes;
        pli = routes->Find((MfActor *)actor);
        if (pli)
            routes->Delete(pli);
    }
    if (event == RBUTTON_UP || event == ALL_EVENTS) {
        routes = &mRButtonUpRoutes;
        pli = routes->Find((MfActor *)actor);
        if (pli)
            routes->Delete(pli);
    }
    if (event == MOUSE_MOVE || event == ALL_EVENTS) {
        routes = &mMouseMoveRoutes;
        pli = routes->Find((MfActor *)actor);
        if (pli)
            routes->Delete(pli);
    }
    return 0;
}

// 0x10090e70 ?Notify@MfMouse@@QAEXH@Z
void MfMouse::Notify(int event)
{
    MfDblList *routes = 0;
    MfDblListItem *pli;
    MfMsg msg(MSG_MOUSE_EVENT, 0, 0);
    if (event == LBUTTON_DOWN) {
        routes = &mLButtonDownRoutes;
        msg.m38 = 5;
    } else if (event == LBUTTON_UP) {
        routes = &mLButtonUpRoutes;
        msg.m38 = 6;
    } else if (event == RBUTTON_DOWN) {
        routes = &mRButtonDownRoutes;
        msg.m38 = 7;
    } else if (event == RBUTTON_UP) {
        routes = &mRButtonUpRoutes;
        msg.m38 = 8;
    } else if (event == MOUSE_MOVE) {
        routes = &mMouseMoveRoutes;
        msg.m38 = 9;
    }
    if (routes) {
        for (MfLiveActor *actor = (MfLiveActor *)routes->GetFirst(pli); actor;
             actor = (MfLiveActor *)routes->GetNext(pli)) {
            if (actor->mHFSM)
                actor->mHFSM->HandleEvent(msg);
        }
    }
}

// 0x10090f60 ?GetLastPickedFloor@MfMouse@@UAEPAVMfAiActorNode@@XZ
MfAiActorNode *MfMouse::GetLastPickedFloor()
{
    if (mPickedActor) {
        MfFloor *floor = ((MfLiveActor *)mPickedActor)->AppropriateFloor(mPickPoint, 0, 0, 0);
        if (floor && floor->GetOwner() == mPickedActor)
            return floor;
    }
    return 0;
}

// 0x10090fc0 ?GetInterfaceName@MfMouse@@UAEXPADHAAH1@Z
void MfMouse::GetInterfaceName(char *name, int size, int &major, int &minor)
{
    strncpy(name, "MfAiMouse", size - 1);
    name[size - 1] = 0;
    major = 1;
    minor = 0;
}

// 0x10091000 ?AddRef@MfMouse@@UAEHXZ
int MfMouse::AddRef()
{
    return ++mRefCount;
}

// 0x10091010 ?Release@MfMouse@@UAEHXZ
int MfMouse::Release()
{
    int count = --mRefCount;
    if (count == 0 && this)
        delete this;
    return count;
}
