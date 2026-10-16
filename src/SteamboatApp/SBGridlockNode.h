#pragma once
// SBGridlockNode.h: the scene of the Gridlock puzzle (SBGridlockNode.obj, declaration from
// SteamboatApp.pdb).
#include "SBActivityNode.h"
#include "CActivityLeveler.h"

class CSBGridLockGameBoard;

class SBGridlockNode : public SBActivityNode {
public:
    SBGridlockNode(const SBGridlockNode &);
    SBGridlockNode(IMBGameEngine *engine, MBString spaceCode);
    SBGridlockNode(IMBGameEngine *engine, MfAiSpace *space);
    virtual ~SBGridlockNode();                          // deleting destructor at vftable+0x20
    virtual void OnStart();                             // vftable+0x0c
    virtual void OnEnd();                               // vftable+0x18
    void UpdateLevel(long nLevelCompleted);
protected:
    virtual void ListenToEvent(long channel, long event, void *data); // vftable+0x0c (subobject +0x8), this adjust +0x8
    virtual void Idle();                                // vftable+0x0c (subobject +0x4), this adjust +0x4
    void InitGame();
    void ReinitGame();
    CSBGridLockGameBoard *m_pGameBoard;                 // +0x00d0 size 0x4
    CActivityLeveler m_objLeveler;                      // +0x00d4 size 0x60
    bool m_bPlayAsMickey;                               // +0x0134 size 0x1
};
