#include "CSBLLTrapDoorTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"
#include "MBRuntime.h"

// 0x10081000 ??0CSBLLTrapDoorTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLTrapDoorTile::CSBLLTrapDoorTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard), m_nTimerStart(-1)
{
}

// 0x10081040 ?GetZOffset@CSBLLTrapDoorTile@@UBEJXZ
long CSBLLTrapDoorTile::GetZOffset() const
{
    return 0;
}

// 0x100810a0 ??1CSBLLTrapDoorTile@@UAE@XZ
CSBLLTrapDoorTile::~CSBLLTrapDoorTile()
{
}

// 0x100810c0 ?Update@CSBLLTrapDoorTile@@UAEXXZ
void CSBLLTrapDoorTile::Update()
{
    MBComPtr<IMBGameClock> pClock(MBIID_IMBGameClock);
    if (m_nTimerStart == -1 && GetCurrentSequence() == kTrapdoorClosed) {
        // a closed trapdoor opens again after its timeout plus up to a quarter of it
        m_nTimerStart = pClock->GetGameTime();
        m_nTimerStart += MBrandom(0, GetTimeout() / 4);
    } else if (m_nTimerStart + GetTimeout() < pClock->GetGameTime()) {
        long nSequence = GetCurrentSequence();
        SetCurrentSequence(++nSequence, false);
        if (nSequence == kTrapdoorOpening) {
            m_nTimerStart = -1;
            if (GetActor()) {
                GetActor()->GoHome(CSBLinkingLogsActor::TrapDoor);
                SetActor(0);
            }
        } else {
            m_nTimerStart = pClock->GetGameTime();
        }
    } else if (GetActor() && !CanMoveTo(*this)) {
        GetActor()->GoHome(CSBLinkingLogsActor::TrapDoor);
        SetActor(0);
    }
}

// 0x10081230 ?OnAnimEvent@CSBLLTrapDoorTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLTrapDoorTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    switch (event) {
    case 4:
    case 6:
        if (GetCurrentSequence() == kTrapdoorOpening)
            SetCurrentSequence(kTrapdoorClosing, false);
        else if (GetCurrentSequence() == kTrapdoorClosing)
            SetCurrentSequence(kTrapdoorClosed, false);
        break;
    }
}

// 0x100812b0 ?CanMoveTo@CSBLLTrapDoorTile@@UBE_NABVCLinkingLogsTile@@@Z
bool CSBLLTrapDoorTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    long nSequence = GetCurrentSequence();
    if (nSequence == kTrapdoorOpening || nSequence == kTrapdoorClosing)
        return false;
    return true;
}

// 0x100812f0 ?OnActorPreenter@CSBLLTrapDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLTrapDoorTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
}

// 0x10081310 ?OnActorEnter@CSBLLTrapDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLTrapDoorTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
}

// 0x10081330 ?OnActorStop@CSBLLTrapDoorTile@@UAE_NAAVCSBLinkingLogsActor@@@Z
bool CSBLLTrapDoorTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    m_pGameBoard->OnEndMove(GetBoardPosition());
    return false;
}

// 0x100813b0 ?OnActorExit@CSBLLTrapDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLTrapDoorTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
    SetActor(0);
}

// 0x100813d0 ?OnTileEvent@CSBLLTrapDoorTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLTrapDoorTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
}

// 0x100813f0 ?ShouldMoveInplace@CSBLLTrapDoorTile@@UAE_NXZ
bool CSBLLTrapDoorTile::ShouldMoveInplace()
{
    return false;
}

// 0x10081410 ?GetVelocity@CSBLLTrapDoorTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLTrapDoorTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    return 80.0f;
}

// 0x10081430 ?GetStartPoint@CSBLLTrapDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLTrapDoorTile::GetStartPoint(D3DXVECTOR3 &vStartPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vStartPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, (float)ptCenter.z);
}

// 0x10081520 ?GetStartVec@CSBLLTrapDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLTrapDoorTile::GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vStartVec = vEndPt - vStartPt;
}

// 0x10081590 ?GetEndPoint@CSBLLTrapDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLTrapDoorTile::GetEndPoint(D3DXVECTOR3 &vEndPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vEndPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, (float)ptCenter.z);
}

// 0x10081680 ?GetEndVec@CSBLLTrapDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLTrapDoorTile::GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vEndVec = vEndPt - vStartPt;
}

// Template and inline instances this unit also emits:

// 0x10081060 ??_GCSBLLTrapDoorTile@@UAEPAXI@Z

// Template and inline instances this unit also emits:
// 0x10071460 ?CreateObject@?$MBComPtr@VIMBGameClock@@@@QAEJABUMBGUID@@@Z

// 0x100809c0 _$E22
// 0x100809e0 _$E19
// 0x10080fb0 _$E21
// 0x10080fd0 _$E20
// 0x100816f0 _$E44
// 0x10081730 _$E40
// 0x10081750 _$E43
