#include "CSBLLSteamTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"
#include "MBRuntime.h"

// 0x100802f0 ??0CSBLLSteamTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLSteamTile::CSBLLSteamTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard), m_nTimerStart(-1)
{
}

// 0x10080330 ?GetZOffset@CSBLLSteamTile@@UBEJXZ
long CSBLLSteamTile::GetZOffset() const
{
    return 20;
}

// 0x10080390 ??1CSBLLSteamTile@@UAE@XZ
CSBLLSteamTile::~CSBLLSteamTile()
{
}

// EQUIV 0x100803b0 ?Update@CSBLLSteamTile@@UAEXXZ: reviewed: the original calls pClock's CreateObject out of line,
// VC6 inlines it here (the trapdoor's Update, the same start, matches)
void CSBLLSteamTile::Update()
{
    MBComPtr<IMBGameClock> pClock(MBIID_IMBGameClock);
    if (m_nTimerStart == -1 && GetCurrentSequence() == kSteamRoot) {
        m_nTimerStart = pClock->GetGameTime();
        m_nTimerStart += MBrandom(0, GetTimeout() / 4);
    } else {
        // the steam blows the actor off the tile it points at
        CGameBoardPos posEffected = GetBoardPosition();
        posEffected.Move(kmapGBDirection[m_objProperties.m_nProperties & kDirectionMask]);
        CLinkingLogsTile &refTile = m_pGameBoard->GetTileAt(posEffected);
        if (m_nTimerStart + GetTimeout() < pClock->GetGameTime()) {
            SetCurrentSequence(kSteamSpurt, false);
            m_nTimerStart = -1;
        }
        if (IsTrapSprung() && refTile.GetActor()) {
            refTile.GetActor()->GoHome(CSBLinkingLogsActor::Steam);
            refTile.SetActor(0);
        }
    }
}

// 0x10080530 ?OnAnimEvent@CSBLLSteamTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLSteamTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    switch (event) {
    case 4:
    case 6:
        switch (GetCurrentSequence()) {
        case kSteamSpurt:
            SetCurrentSequence(kSteamRoot, false);
            break;
        }
        break;
    }
}

// 0x10080580 ?CanMoveTo@CSBLLSteamTile@@UBE_NABVCLinkingLogsTile@@@Z
bool CSBLLSteamTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    return false;
}

// 0x100805a0 ?OnActorPreenter@CSBLLSteamTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLSteamTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
}

// 0x100805c0 ?OnActorEnter@CSBLLSteamTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLSteamTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
}

// 0x100805e0 ?OnActorStop@CSBLLSteamTile@@UAE_NAAVCSBLinkingLogsActor@@@Z
bool CSBLLSteamTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    return false;
}

// 0x10080600 ?OnActorExit@CSBLLSteamTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLSteamTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
}

// 0x10080620 ?OnTileEvent@CSBLLSteamTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLSteamTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
}

// 0x10080640 ?ShouldMoveInplace@CSBLLSteamTile@@UAE_NXZ
bool CSBLLSteamTile::ShouldMoveInplace()
{
    return false;
}

// 0x10080660 ?GetVelocity@CSBLLSteamTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLSteamTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    return 80.0f;
}

// 0x10080680 ?GetStartPoint@CSBLLSteamTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLSteamTile::GetStartPoint(D3DXVECTOR3 &vStartPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vStartPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x10080770 ?GetStartVec@CSBLLSteamTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLSteamTile::GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vStartVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

// 0x100807c0 ?GetEndPoint@CSBLLSteamTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLSteamTile::GetEndPoint(D3DXVECTOR3 &vEndPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vEndPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x100808b0 ?GetEndVec@CSBLLSteamTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLSteamTile::GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vEndVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

// 0x10080900 ?IsTrapSprung@CSBLLSteamTile@@IAE_NXZ
bool CSBLLSteamTile::IsTrapSprung()
{
    return GetCurrentSequence() != kSteamRoot && GetCurrentFrame() > 0;
}

// Template and inline instances this unit also emits:

// 0x10080350 ??_GCSBLLSteamTile@@UAEPAXI@Z

// 0x1007fcb0 _$E22
// 0x1007fcd0 _$E19
// 0x100802a0 _$E21
// 0x100802c0 _$E20
// 0x10080940 _$E44
// 0x10080980 _$E40
// 0x100809a0 _$E43
