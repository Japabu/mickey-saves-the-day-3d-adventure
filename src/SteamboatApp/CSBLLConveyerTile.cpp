#include "CSBLLConveyerTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"
#include <math.h>

// The unit vector of each tile direction bit.
D3DXVECTOR3 kmapDirectionVec[] = {
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f),
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
    D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f)
};

// 0x1007c0a0 ??0CSBLLConveyerTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLConveyerTile::CSBLLConveyerTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard)
{
}

// 0x1007c0d0 ?GetZOffset@CSBLLConveyerTile@@UBEJXZ
long CSBLLConveyerTile::GetZOffset() const
{
    return -5;
}

// 0x1007c130 ??1CSBLLConveyerTile@@UAE@XZ
CSBLLConveyerTile::~CSBLLConveyerTile()
{
}

// 0x1007c150 ?Update@CSBLLConveyerTile@@UAEXXZ
void CSBLLConveyerTile::Update()
{
}

// 0x1007c170 ?OnAnimEvent@CSBLLConveyerTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLConveyerTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
}

// 0x1007c190 ?CanMoveTo@CSBLLConveyerTile@@UBE_NABVCLinkingLogsTile@@@Z
bool CSBLLConveyerTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    return IsWalkable();
}

// 0x1007c1b0 ?OnActorPreenter@CSBLLConveyerTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLConveyerTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
}

// 0x1007c1d0 ?OnActorEnter@CSBLLConveyerTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLConveyerTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
}

// EQUIV 0x1007c1f0 ?OnActorStop@CSBLLConveyerTile@@UAE_NAAVCSBLinkingLogsActor@@@Z: reviewed: same code, the
// direction index and the properties sit in swapped registers (edi/ebp; comparison orders tried)
bool CSBLLConveyerTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    // the conveyer carries the actor to its last tile in the same direction
    CGameBoardPos posNewDestinaton = GetBoardPosition();
    CGameBoardPos posNext = GetBoardPosition();
    tiledirection eDirection = GetDirection(*this);
    while (posNext.Move(kmapGBDirection[eDirection])) {
        CLinkingLogsTile &refTile = m_pGameBoard->GetTileAt(posNext);
        if (GetDirection(*this) != GetDirection(refTile)) {
            if (GetBoardPosition() == posNewDestinaton)
                posNewDestinaton = posNext;
            break;
        }
        if (GetType(refTile) != kConveyer)
            break;
        posNewDestinaton = posNext;
    }
    refActor.SetPathTo(posNewDestinaton);
    return true;
}

// 0x1007c380 ?OnActorExit@CSBLLConveyerTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLConveyerTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
    SetActor(0);
}

// 0x1007c3a0 ?OnTileEvent@CSBLLConveyerTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLConveyerTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
}

// 0x1007c3c0 ?ShouldMoveInplace@CSBLLConveyerTile@@UAE_NXZ
bool CSBLLConveyerTile::ShouldMoveInplace()
{
    return true;
}

// Template and inline instances this unit also emits:

// 0x1007c3e0 ?GetVelocity@CSBLLConveyerTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLConveyerTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    // the belt speeds up a walk along it, slows down one against it
    float fVelocity = 60.0f;
    switch (GetDirection(*this)) {
    case kUp:
        switch (eDirection) {
        case CGameBoardPos::GBUp:
            fVelocity = CLinkingLogsTile::GetVelocity() + 60.0f;
            break;
        case CGameBoardPos::GBDown:
            fVelocity = 60.0f - CLinkingLogsTile::GetVelocity();
            break;
        case CGameBoardPos::GBLeft:
        case CGameBoardPos::GBRight:
            break;
        default:
            fVelocity = CLinkingLogsTile::GetVelocity();
            break;
        }
        break;
    case kDown:
        switch (eDirection) {
        case CGameBoardPos::GBUp:
            fVelocity = 60.0f - CLinkingLogsTile::GetVelocity();
            break;
        case CGameBoardPos::GBDown:
            fVelocity = CLinkingLogsTile::GetVelocity() + 60.0f;
            break;
        case CGameBoardPos::GBLeft:
        case CGameBoardPos::GBRight:
            break;
        default:
            fVelocity = CLinkingLogsTile::GetVelocity();
            break;
        }
        break;
    case kLeft:
        switch (eDirection) {
        case CGameBoardPos::GBLeft:
            fVelocity = CLinkingLogsTile::GetVelocity() + 60.0f;
            break;
        case CGameBoardPos::GBRight:
            fVelocity = 60.0f - CLinkingLogsTile::GetVelocity();
            break;
        case CGameBoardPos::GBUp:
        case CGameBoardPos::GBDown:
            break;
        default:
            fVelocity = CLinkingLogsTile::GetVelocity();
            break;
        }
        break;
    case kRight:
        switch (eDirection) {
        case CGameBoardPos::GBLeft:
            fVelocity = 60.0f - CLinkingLogsTile::GetVelocity();
            break;
        case CGameBoardPos::GBRight:
            fVelocity = CLinkingLogsTile::GetVelocity() + 60.0f;
            break;
        case CGameBoardPos::GBUp:
        case CGameBoardPos::GBDown:
            break;
        default:
            fVelocity = CLinkingLogsTile::GetVelocity();
            break;
        }
        break;
    }
    return fVelocity;
}

// 0x1007c550 ?GetStartPoint@CSBLLConveyerTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLConveyerTile::GetStartPoint(D3DXVECTOR3 &vPt) const
{
    // the actor gets on and off at the edges of the belt
    MBPoint3 ptCenter = GetCenter();
    vPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
    switch (GetDirection(*this)) {
    case kUp:
        vPt.y += GetRect().Height() / 2 - 1;
        break;
    case kDown:
        vPt.y -= GetRect().Height() / 2 - 1;
        break;
    case kLeft:
        vPt.x += GetRect().Height() / 2 - 1;
        break;
    case kRight:
        vPt.x -= GetRect().Height() / 2 - 1;
        break;
    }
}

// TODO 0x1007c760 ?GetStartVec@CSBLLConveyerTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z: the original reads the direction
// bits twice and builds the scaled vector in the dead parameter slots
void CSBLLConveyerTile::GetStartVec(D3DXVECTOR3 &vVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    // along the belt, a quarter of the distance long
    vVec = kmapDirectionVec[GetDirection(*this)];
    D3DXVECTOR3 vLength = vEndPt - vStartPt;
    float fLength = (float)sqrt(vLength.x * vLength.x + vLength.y * vLength.y + vLength.z * vLength.z);
    if (fLength < 0.0f)
        fLength = -fLength;
    vVec = vVec * (fLength * 0.25f);
    switch (GetDirection(*this)) {
    case kUp:
    case kDown:
        vVec.y += vLength.y;
        break;
    case kLeft:
    case kRight:
        vVec.x += vLength.x;
        break;
    }
}

// 0x1007c8a0 ?GetEndPoint@CSBLLConveyerTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLConveyerTile::GetEndPoint(D3DXVECTOR3 &vPt) const
{
    // the actor gets on and off at the edges of the belt
    MBPoint3 ptCenter = GetCenter();
    vPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
    switch (GetDirection(*this)) {
    case kUp:
        vPt.y -= GetRect().Height() / 2 - 1;
        break;
    case kDown:
        vPt.y += GetRect().Height() / 2 - 1;
        break;
    case kLeft:
        vPt.x -= GetRect().Height() / 2 - 1;
        break;
    case kRight:
        vPt.x += GetRect().Height() / 2 - 1;
        break;
    }
}

// TODO 0x1007cab0 ?GetEndVec@CSBLLConveyerTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z: the original reads the direction
// bits twice and builds the scaled vector in the dead parameter slots
void CSBLLConveyerTile::GetEndVec(D3DXVECTOR3 &vVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    // along the belt, a quarter of the distance long
    vVec = kmapDirectionVec[GetDirection(*this)];
    D3DXVECTOR3 vLength = vEndPt - vStartPt;
    float fLength = (float)sqrt(vLength.x * vLength.x + vLength.y * vLength.y + vLength.z * vLength.z);
    if (fLength < 0.0f)
        fLength = -fLength;
    vVec = vVec * (fLength * 0.25f);
    switch (GetDirection(*this)) {
    case kUp:
    case kDown:
        vVec.y += vLength.y;
        break;
    case kLeft:
    case kRight:
        vVec.x += vLength.x;
        break;
    }
}

// 0x1007c0f0 ??_GCSBLLConveyerTile@@UAEPAXI@Z

// 0x1007b7d0 _$E22
// 0x1007b7f0 _$E19
// 0x1007bdc0 _$E21
// 0x1007bde0 _$E20
// 0x1007be10 _$E36
// 0x1007be30 _$E35
// 0x1007cbf0 _$E47
// 0x1007cc30 _$E43
// 0x1007cc50 _$E46
