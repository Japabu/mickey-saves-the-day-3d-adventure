#pragma once
// CSBGridlockActor.h: the character of the Gridlock puzzle that walks from tile to tile
// (SBGridlockActor.obj, declaration from SteamboatApp.pdb).
#include "CGameBoardPos.h"
#include "CVectorPath.h"

class CSBGridlockGameTile;

class CSBGridlockActor {
    enum action {
        ActorIdle = 0,
        ActorStart = 4,
        ActorWalk = 8,
        ActorTurnCounterClock = 12,
        ActorTurnClock = 16,
        ActorStop = 20
    };
    struct ActionStep {
        ActionStep(const ActionStep &refStep)
            : m_eDirection(refStep.m_eDirection), m_eAction(refStep.m_eAction), m_posRoot(refStep.m_posRoot)
        {
        }
        ActionStep(CGameBoardPos::direction eDirection, action eAction, const CGameBoardPos &refRoot)
            : m_eDirection(eDirection), m_eAction(eAction), m_posRoot(refRoot)
        {
        }
        ActionStep() : m_eDirection(CGameBoardPos::GBUp), m_eAction(action(0)), m_posRoot(0, 0, 0, 0, 0, 0) {}
        const ActionStep &operator=(const ActionStep &refStep)
        {
            m_eDirection = refStep.m_eDirection;
            m_eAction = refStep.m_eAction;
            m_posRoot = refStep.m_posRoot;
            return *this;
        }
        int operator==(const ActionStep &refStep) const
        {
            return m_eDirection == refStep.m_eDirection && m_eAction == refStep.m_eAction && m_posRoot == refStep.m_posRoot;
        }
        int operator!=(const ActionStep &refStep) const
        {
            return m_eDirection != refStep.m_eDirection || m_eAction != refStep.m_eAction || m_posRoot != refStep.m_posRoot;
        }
        CGameBoardPos::direction m_eDirection;          // +0x0000 size 0x4
        action m_eAction;                               // +0x0004 size 0x4
        CGameBoardPos m_posRoot;                        // +0x0008 size 0x18
    };
    typedef MBList<ActionStep> actionqueue;
public:
    CSBGridlockActor(const CSBGridlockActor &refActorToCopy);
    CSBGridlockActor(long nVelocity);
    virtual ~CSBGridlockActor();                        // deleting destructor at vftable+0x00
    virtual void SetPosition(const CGameBoardPos &posTilePosition, CGameBoardPos::direction eStartDirection); // vftable+0x04
    virtual void Attach(CSBGridlockGameTile &refTile);  // vftable+0x08
    virtual void MoveTo(const CGameBoardPos &posTilePosition, bool bLastMove); // vftable+0x0c
    virtual bool IsMoveing() const;                     // vftable+0x10
    virtual void Update();                              // vftable+0x14
    virtual void OnAnimEvent(long event, const MBAnimEventStruct &refEventData); // vftable+0x18
protected:
    void GetRestPosition(ActionStep &refStep);
    void GetNextStep(ActionStep &refStep);
    void UpdateAnimSequence();
    void OnSequenceEnded();
    CVectorPath m_objActorPath;                         // +0x0004 size 0x3c
    actionqueue m_queueActions;                         // +0x0040 size 0xc
    ActionStep m_objCurrentAction;                      // +0x004c size 0x20
    CSBGridlockGameTile *m_pTile;                       // +0x006c size 0x4
    long m_nVelocity;                                   // +0x0070 size 0x4
    bool m_bMoveing;                                    // +0x0074 size 0x1
    static MBMap<CGameBoardPos::direction, long, std::less<CGameBoardPos::direction> > m_mapDirectionToSequence;
};
