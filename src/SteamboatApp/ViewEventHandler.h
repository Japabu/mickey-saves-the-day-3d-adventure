#pragma once
// ViewEventHandler.h: turns mouse clicks in the 3D view into picks of Motivate actors
// (ViewEventHandler.obj, declaration from SteamboatApp.pdb).
#include "MfHookData.h"

class MfAiActor;
class SBMotivateDriver;

class ViewEventHandler {
public:
    ViewEventHandler(SBMotivateDriver *thePlayer);
    virtual ~ViewEventHandler();
    MfAiActor *HandleLButtonDown(MfMouseHookData *data);
    MfAiActor *HandleLButtonUp(MfMouseHookData *data, MfPickData &pickData);
    virtual bool ConstructRay(float x, float y, MfVector3 &position, MfVector3 &direction);
protected:
    static SBMotivateDriver *player;
};
