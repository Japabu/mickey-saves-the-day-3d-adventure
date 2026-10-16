#pragma once
// CSBLLConveyerTile.h: a linking-logs conveyer: carries the actor along (SBLLConveyerTile.obj, declaration from
// SteamboatApp.pdb).
#include "CLinkingLogsTile.h"

class CSBLLConveyerTile : public CLinkingLogsTile {
public:
    CSBLLConveyerTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard);
    virtual ~CSBLLConveyerTile();
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
};
