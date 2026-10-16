#pragma once
// CSBLLSteamTile.h: a linking-logs steam vent: blows the actor home when its timer runs out (SBLLSteamTile.obj, declaration from
// SteamboatApp.pdb).
#include "CLinkingLogsTile.h"

class CSBLLSteamTile : public CLinkingLogsTile {
public:
    CSBLLSteamTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard);
    virtual ~CSBLLSteamTile();
    virtual long GetZOffset() const;
    virtual float GetVelocity(CGameBoardPos::direction eDirection);
    virtual void Update();
    virtual void OnAnimEvent(long event, const MBAnimEventStruct &refEventData);
    virtual bool CanMoveTo(const CLinkingLogsTile &refFromTile) const;
    virtual void OnActorPreenter(CSBLinkingLogsActor &refActor);
    virtual void OnActorEnter(CSBLinkingLogsActor &refActor);
    virtual bool OnActorStop(CSBLinkingLogsActor &refActor);
    virtual void OnActorExit(CSBLinkingLogsActor &refActor);
    virtual void OnTileEvent(long event, CLinkingLogsTile *pTile);
    virtual bool ShouldMoveInplace();
    virtual void GetStartPoint(D3DXVECTOR3 &vStartPt) const;
    virtual void GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const;
    virtual void GetEndPoint(D3DXVECTOR3 &vEndPt) const;
    virtual void GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const;
protected:
    bool IsTrapSprung();
    long m_nTimerStart;                                 // +0x0074
};
