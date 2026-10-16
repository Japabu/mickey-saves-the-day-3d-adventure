#pragma once
// SBCreditsNode.h: the credits screens (SBCreditsNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

class SBCreditsNode : public SBActivityNode {
public:
    SBCreditsNode(const SBCreditsNode &);
    SBCreditsNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBCreditsNode();
    virtual void OnStart();
    virtual void OnEnd();
    virtual void StartListening();
    virtual void StopListening();
    virtual void OnKeyDown(MBKeyboardEvent &key);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle();
protected:
    virtual void OnActivityEvent(long event, void *data);
    virtual void Done();
    MBString mPrevNode;                                 // +0x00d0 size 0x10
    MBComPtr<IMBAnim> mScreens;                         // +0x00e0 size 0x4
};
