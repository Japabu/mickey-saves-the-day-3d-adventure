#pragma once
// SBSoupRecipeNode.h: the soup recipe book (SBSoupRecipeNode.obj, declaration from SteamboatApp.pdb).
#include "SBActivityNode.h"

class SBSoupRecipeNode : public SBActivityNode {
public:
    SBSoupRecipeNode(const SBSoupRecipeNode &);
    SBSoupRecipeNode(IMBGameEngine *engine, MBString spaceCode);
    virtual ~SBSoupRecipeNode();
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
};
