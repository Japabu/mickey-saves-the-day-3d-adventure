#include "CSBLLPathTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"

// The unit vector of each tile direction bit (this unit's own copy; the conveyer tiles use theirs).
static D3DXVECTOR3 kmapDirectionVec[] = {
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f),
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f)
};

// 0x1007f720 ??0CSBLLPathTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLPathTile::CSBLLPathTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard)
{
}

// 0x1007f750 ?GetZOffset@CSBLLPathTile@@UBEJXZ
long CSBLLPathTile::GetZOffset() const
{
    return 0;
}

// 0x1007f7b0 ??1CSBLLPathTile@@UAE@XZ
CSBLLPathTile::~CSBLLPathTile()
{
}

// 0x1007f7d0 ?Update@CSBLLPathTile@@UAEXXZ
void CSBLLPathTile::Update()
{
}

// 0x1007f7f0 ?OnAnimEvent@CSBLLPathTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLPathTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
}

// 0x1007f810 ?CanMoveTo@CSBLLPathTile@@UBE_NABVCLinkingLogsTile@@@Z
bool CSBLLPathTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    return IsWalkable();
}

// 0x1007f830 ?OnActorPreenter@CSBLLPathTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLPathTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
}

// 0x1007f850 ?OnActorEnter@CSBLLPathTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLPathTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
}

// 0x1007f870 ?OnActorStop@CSBLLPathTile@@UAE_NAAVCSBLinkingLogsActor@@@Z
bool CSBLLPathTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    m_pGameBoard->OnEndMove(GetBoardPosition());
    return false;
}

// 0x1007f8f0 ?OnActorExit@CSBLLPathTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLPathTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
    SetActor(0);
}

// 0x1007f910 ?OnTileEvent@CSBLLPathTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLPathTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
}

// 0x1007f930 ?ShouldMoveInplace@CSBLLPathTile@@UAE_NXZ
bool CSBLLPathTile::ShouldMoveInplace()
{
    return false;
}

// 0x1007f950 ?GetVelocity@CSBLLPathTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLPathTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    return 90.0f;
}

// 0x1007f970 ?GetStartPoint@CSBLLPathTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLPathTile::GetStartPoint(D3DXVECTOR3 &vStartPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vStartPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007fa60 ?GetStartVec@CSBLLPathTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLPathTile::GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vStartVec = vEndPt - vStartPt;
}

// 0x1007fad0 ?GetEndPoint@CSBLLPathTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLPathTile::GetEndPoint(D3DXVECTOR3 &vEndPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vEndPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007fbc0 ?GetEndVec@CSBLLPathTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLPathTile::GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vEndVec = vEndPt - vStartPt;
}

// Template and inline instances this unit also emits:
// 0x1007f770 ??_GCSBLLPathTile@@UAEPAXI@Z

// 0x1007ee50 _$E22
// 0x1007ee70 _$E19
// 0x1007f440 _$E21
// 0x1007f460 _$E20
// 0x1007f490 _$E36
// 0x1007f4b0 _$E35
// 0x1007fc30 _$E47
// 0x1007fc70 _$E43
// 0x1007fc90 _$E46
