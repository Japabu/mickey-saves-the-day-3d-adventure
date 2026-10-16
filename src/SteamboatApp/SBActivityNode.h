#pragma once
// SBActivityNode.h: base node of the activity (mini-game) scenes (declaration from SteamboatApp.pdb).
#include "SteamboatNode.h"

class SBActivityNode : public SteamboatNode {
public:
    SBActivityNode(const SBActivityNode &);
    SBActivityNode(IMBGameEngine *engine, MBString spaceCode);
    SBActivityNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBActivityNode();                          // deleting destructor at vftable+0x20
    virtual void StartListening();                      // vftable+0x88
    virtual void StopListening();                       // vftable+0x8c
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c (subobject +0x8), this adjust +0x8
protected:
    virtual void RenderScene();                         // vftable+0xf4
    virtual void OnActivityEvent(long event, void *data); // vftable+0x124
};
