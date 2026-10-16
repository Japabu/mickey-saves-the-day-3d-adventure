#pragma once
// CSBLinkingLogsActor.h: Mickey or Minnie walking the linking-logs board (SBLinkingLogsActor.obj,
// declaration from SteamboatApp.pdb).
#include "CLinkingLogsTile.h"

class CSBLinkingLogsActor {
public:
    enum action {
        ActorIdle = 0,
        ActorStart = 4,
        ActorWalk = 8,
        ActorTurnCounterClock = 12,
        ActorTurnClock = 16,
        ActorStop = 20,
        ActorPickupCraneDown = 30,
        ActorPickupCraneUp = 31,
        ActorDropoffCraneDown = 32,
        ActorDropoffCraneUp = 33
    };
    enum gohome_type {
        Crane = 0,
        Steam = 1,
        TrapDoor = 2
    };
    struct ActionStep {
        ActionStep(const ActionStep &refStep) : m_eDirection(refStep.m_eDirection), m_eAction(refStep.m_eAction) {}
        ActionStep(CGameBoardPos::direction eDirection, action eAction) : m_eDirection(eDirection), m_eAction(eAction) {}
        ActionStep() : m_eDirection(CGameBoardPos::GBUp), m_eAction(ActorIdle) {}
        const ActionStep &operator=(const ActionStep &refStep)
        {
            m_eDirection = refStep.m_eDirection;
            m_eAction = refStep.m_eAction;
            return *this;
        }
        int operator==(const ActionStep &refStep) const
        {
            return m_eDirection == refStep.m_eDirection && m_eAction == refStep.m_eAction;
        }
        int operator!=(const ActionStep &refStep) const { return !(*this == refStep); }
        CGameBoardPos::direction m_eDirection;          // +0x0000
        action m_eAction;                               // +0x0004
    };
    typedef MBList<ActionStep> actionqueue;
    CSBLinkingLogsActor(CLinkingLogsTile *pCurrentTile, CSBLinkingLogsGameBoard *pBoard);
    virtual ~CSBLinkingLogsActor();
    void SetActor(bool bPlayMickey);
    void MoveTo(const CGameBoardPos &posDestination);
    void SetPathTo(const CGameBoardPos &posDestination);
    bool IsMoveing() const;
    void Update();
    void OnAnimEvent(long event, const MBAnimEventStruct &refEventData);
    void GoHome(gohome_type eGoHomeType);
    CLinkingLogsTile *GetCurrentTile() { return m_pCurrentTile; }
    CGameBoardPos GetDestination();
    CGameBoardPos::direction GetDirection();
protected:
    void GetNextStep(ActionStep &refStep);
    void UpdateAnimSequence();
    void OnSequenceEnded();
    void PlaceCenterd(MBPoint3 ptPosition);
    CGameBoardPos::direction BuildPath(CGameBoardPos &posDestination);
    void UpdateTiles(const D3DXVECTOR3 &vCurrentPoint);
    void UpdatePath();
    void NotifyTiles(const CGameBoardPos &posNext, CGameBoardPos::direction eDirection);
    CKeyFrameInterp m_objPathFrames;                    // +0x0004
    float m_fVelocity;                                  // +0x0038
    float m_fDistance;                                  // +0x003c
    float m_fKeyFramePos;                               // +0x0040
    long m_nLastUpdateTime;                             // +0x0044
    CLinkingLogsTile *m_pCurrentTile;                   // +0x0048
    CSBLinkingLogsGameBoard *m_pBoard;                  // +0x004c
    CGameBoardPos m_posNewDestination;                  // +0x0050
    CGameBoardPos m_posDestination;                     // +0x0068
    CGameBoardPos m_posHome;                            // +0x0080
    IMBAnim *m_pAnim;                                   // +0x0098
    IMBAnim *m_pCraneAnim;                              // +0x009c
    MBRect m_rectAnim;                                  // +0x00a0
    actionqueue m_queueActions;                         // +0x00b0
    ActionStep m_objCurrentAction;                      // +0x00bc
    MBMap<CGameBoardPos::direction, long, std::less<CGameBoardPos::direction> > m_mapDirectionToSequence; // +0x00c4
    bool m_bMoveing;                                    // +0x00d4
    bool m_bPreenterComplete;                           // +0x00d5
    bool m_bGoingHome;                                  // +0x00d6
    bool m_bMickye;                                     // +0x00d7
    MBString m_strGoHomeSound;                          // +0x00d8
};
