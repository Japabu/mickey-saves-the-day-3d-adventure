#pragma once
// SBMapNode.h: the map of the steamboat, from which the player picks a scene (SBMapNode.obj,
// declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

class SBMapNode : public SBActivityNode {
public:
    SBMapNode(const SBMapNode &);
    SBMapNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBMapNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    MBString mPrevNode;                                 // +0x00d0 size 0x10
    bool mLeaving;                                      // +0x00e0 size 0x1
};
