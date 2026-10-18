#include "CSBLLOnewayDoorTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"

// 0x1007e3f0 ??0CSBLLOnewayDoorTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLOnewayDoorTile::CSBLLOnewayDoorTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard)
{
}

// 0x1007e420 ?GetZOffset@CSBLLOnewayDoorTile@@UBEJXZ
long CSBLLOnewayDoorTile::GetZOffset() const
{
    return 0;
}

// 0x1007e480 ??1CSBLLOnewayDoorTile@@UAE@XZ
CSBLLOnewayDoorTile::~CSBLLOnewayDoorTile()
{ }

// 0x1007e4a0 ?Update@CSBLLOnewayDoorTile@@UAEXXZ
void CSBLLOnewayDoorTile::Update()
{
}

// 0x1007e4c0 ?OnAnimEvent@CSBLLOnewayDoorTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLOnewayDoorTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    switch (event) {
    case 4:
    case 6:
        // an opening door stays open, a closing one closed
        switch (GetCurrentSequence()) {
        case kDoorOpening:
            SetCurrentSequence(kDoorOpen, false);
            break;
        case kDoorClosing:
            SetCurrentSequence(kDoorClosed, false);
            break;
        }
        break;
    }
}

// TODO 0x1007e530 ?CanMoveTo@CSBLLOnewayDoorTile@@UBE_NABVCLinkingLogsTile@@@Z: 99% similar; in the kUp
// case the original calls Col() on the source tile's position before this tile's (the kDown case
// matches) and normalizes the && result in bl; swapping the operands moves the temporaries too.
// Same comparisons, calls and result
bool CSBLLOnewayDoorTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    // the door is entered only from the side it opens to: from above when it points down, from below
    // when it points up
    bool bCanMove = false;
    switch (GetDirection(*this)) {
    case kUp:
        if (refFromTile.GetBoardPosition().Col() == GetBoardPosition().Col() && refFromTile.GetBoardPosition().Row() > GetBoardPosition().Row())
            bCanMove = true;
        break;
    case kDown:
        if (refFromTile.GetBoardPosition().Col() == GetBoardPosition().Col() && refFromTile.GetBoardPosition().Row() < GetBoardPosition().Row())
            bCanMove = true;
        break;
    }
    return bCanMove;
}

// 0x1007e800 ?OnActorPreenter@CSBLLOnewayDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLOnewayDoorTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
    // open for an actor coming from the right side
    switch (GetDirection(*this)) {
    case kUp:
        if (GetCurrentSequence() != kDoorOpening && GetCurrentSequence() != kDoorOpen && refActor.GetCurrentTile()->GetBoardPosition().Row() > GetBoardPosition().Row())
            SetCurrentSequence(kDoorOpening, false);
        break;
    case kDown:
        if (GetCurrentSequence() != kDoorOpening && GetCurrentSequence() != kDoorOpen && refActor.GetCurrentTile()->GetBoardPosition().Row() < GetBoardPosition().Row())
            SetCurrentSequence(kDoorOpening, false);
        break;
    }
}

// 0x1007e9e0 ?OnActorEnter@CSBLLOnewayDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLOnewayDoorTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
}

// 0x1007ea00 ?OnActorStop@CSBLLOnewayDoorTile@@UAE_NAAVCSBLinkingLogsActor@@@Z
bool CSBLLOnewayDoorTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    m_pGameBoard->OnEndMove(GetBoardPosition());
    return false;
}

// 0x1007ea80 ?OnActorExit@CSBLLOnewayDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLOnewayDoorTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
    SetCurrentSequence(kDoorClosing, false);
    SetActor(0);
}

// 0x1007eab0 ?OnTileEvent@CSBLLOnewayDoorTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLOnewayDoorTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
}

// 0x1007ead0 ?ShouldMoveInplace@CSBLLOnewayDoorTile@@UAE_NXZ
bool CSBLLOnewayDoorTile::ShouldMoveInplace()
{
    return false;
}

// 0x1007eaf0 ?GetVelocity@CSBLLOnewayDoorTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLOnewayDoorTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    return 80.0f;
}

// 0x1007eb10 ?GetStartPoint@CSBLLOnewayDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLOnewayDoorTile::GetStartPoint(D3DXVECTOR3 &vStartPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vStartPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007ec00 ?GetStartVec@CSBLLOnewayDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLOnewayDoorTile::GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vStartVec = vEndPt - vStartPt;
}

// 0x1007ec70 ?GetEndPoint@CSBLLOnewayDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLOnewayDoorTile::GetEndPoint(D3DXVECTOR3 &vEndPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vEndPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007ed60 ?GetEndVec@CSBLLOnewayDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLOnewayDoorTile::GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vEndVec = vEndPt - vStartPt;
}

// Template and inline instances this unit also emits:
// 0x1007e440 ??_GCSBLLOnewayDoorTile@@UAEPAXI@Z

// 0x1007ddb0 _$E22
// 0x1007ddd0 _$E19
// 0x1007e3a0 _$E21
// 0x1007e3c0 _$E20
// 0x1007edd0 _$E44
// 0x1007ee10 _$E40
// 0x1007ee30 _$E43
