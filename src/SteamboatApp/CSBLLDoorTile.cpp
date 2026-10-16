#include "CSBLLDoorTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBLinkingLogsGameBoard.h"
#include "SteamboatNode.h"
#include <windows.h>

// 0x1007d2b0 ??0CSBLLDoorTile@@QAE@ABUtileinfo@CLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLLDoorTile::CSBLLDoorTile(const CLinkingLogsTile::tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : CLinkingLogsTile(refInfo, pBoard), m_nTimerStart(-1)
{
}

// 0x1007d2f0 ?GetZOffset@CSBLLDoorTile@@UBEJXZ
long CSBLLDoorTile::GetZOffset() const
{
    return 0;
}

// 0x1007d350 ??1CSBLLDoorTile@@UAE@XZ
CSBLLDoorTile::~CSBLLDoorTile()
{
}

// 0x1007d370 ?Update@CSBLLDoorTile@@UAEXXZ
void CSBLLDoorTile::Update()
{
    // an open door closes again after its timeout and turns its switch off
    if (!IsLocked() && m_nTimerStart != -1 && GetCurrentSequence() == kDoorOpen) {
        MBComPtr<IMBGameClock> pClock(MBIID_IMBGameClock);
        if (m_nTimerStart + GetTimeout() < pClock->GetGameTime()) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(204, MAKELONG(kSwitch, 2), this);
            SetCurrentSequence(kDoorClosing, false);
            SteamboatNode::GetActiveNode()->StartSound("LLSEdoorClose");
            m_nTimerStart = -1;
        }
    }
}

// 0x1007d530 ?OnAnimEvent@CSBLLDoorTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBLLDoorTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    switch (event) {
    case 4:
    case 6:
        switch (GetCurrentSequence()) {
        case kDoorOpening:
            SetCurrentSequence(kDoorOpen, false);
            break;
        case kDoorClosing:
            SetCurrentSequence(kDoorClosed, false);
            break;
        case kDoorOpen: {
            MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
            m_nTimerStart = clock->GetGameTime();
            break;
        }
        }
        break;
    }
}

// 0x1007d6a0 ?CanMoveTo@CSBLLDoorTile@@UBE_NABVCLinkingLogsTile@@@Z
bool CSBLLDoorTile::CanMoveTo(const CLinkingLogsTile &refFromTile) const
{
    bool bCanMove = false;
    if (IsWalkable() && GetType(*this) == kDoor) {
        long nSequence = GetCurrentSequence();
        if (nSequence == kDoorOpen || nSequence == kDoorOpening)
            bCanMove = true;
    } else if (IsWalkable())
        bCanMove = true;
    return bCanMove;
}

// 0x1007d710 ?OnActorPreenter@CSBLLDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLDoorTile::OnActorPreenter(CSBLinkingLogsActor &refActor)
{
}

// 0x1007d730 ?OnActorEnter@CSBLLDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLDoorTile::OnActorEnter(CSBLinkingLogsActor &refActor)
{
    SetActor(&refActor);
    if (GetType(*this) == kDoor) {
        LockState(true);
    } else if (GetType(*this) == kSwitch) {
        // stepping on the switch opens its door
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(204, MAKELONG(kSwitch, 1), this);
        SteamboatNode::GetActiveNode()->StartSound("LLSEbuttonpress");
        SetCurrentSequence(kDoorSwitchOn, false);
    }
}

// 0x1007d8a0 ?OnActorStop@CSBLLDoorTile@@UAE_NAAVCSBLinkingLogsActor@@@Z
bool CSBLLDoorTile::OnActorStop(CSBLinkingLogsActor &refActor)
{
    m_pGameBoard->OnEndMove(GetBoardPosition());
    return false;
}

// 0x1007d920 ?OnActorExit@CSBLLDoorTile@@UAEXAAVCSBLinkingLogsActor@@@Z
void CSBLLDoorTile::OnActorExit(CSBLinkingLogsActor &refActor)
{
    LockState(false);
    SetActor(0);
}

// 0x1007d940 ?OnTileEvent@CSBLLDoorTile@@UAEXJPAVCLinkingLogsTile@@@Z
void CSBLLDoorTile::OnTileEvent(long event, CLinkingLogsTile *pTile)
{
    if (!IsLocked()) {
        switch ((event >> 16) & 0xffff) {
        case 1:
            if (GetType(*this) == kDoor && pTile->GetID() == GetID()) {
                if (GetCurrentSequence() != kDoorOpen && GetCurrentSequence() != kDoorOpening) {
                    SetCurrentSequence(kDoorOpening, false);
                    SteamboatNode::GetActiveNode()->StartSound("LLSEdoorOpen");
                }
            }
            break;
        case 2:
            if (GetType(*this) == kSwitch && pTile->GetID() == GetID())
                SetCurrentSequence(kDoorSwitchOff, false);
            break;
        }
    }
}

// 0x1007da30 ?ShouldMoveInplace@CSBLLDoorTile@@UAE_NXZ
bool CSBLLDoorTile::ShouldMoveInplace()
{
    return false;
}

// 0x1007da50 ?GetVelocity@CSBLLDoorTile@@UAEMW4direction@CGameBoardPos@@@Z
float CSBLLDoorTile::GetVelocity(CGameBoardPos::direction eDirection)
{
    return 90.0f;
}

// 0x1007da70 ?GetStartPoint@CSBLLDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLDoorTile::GetStartPoint(D3DXVECTOR3 &vStartPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vStartPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007db60 ?GetStartVec@CSBLLDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLDoorTile::GetStartVec(D3DXVECTOR3 &vStartVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vStartVec = vEndPt - vStartPt;
}

// 0x1007dbd0 ?GetEndPoint@CSBLLDoorTile@@UBEXAAUD3DXVECTOR3@@@Z
void CSBLLDoorTile::GetEndPoint(D3DXVECTOR3 &vEndPt) const
{
    MBPoint3 ptCenter = GetCenter();
    vEndPt = D3DXVECTOR3((float)ptCenter.x, (float)ptCenter.y, 0.0f);
}

// 0x1007dcc0 ?GetEndVec@CSBLLDoorTile@@UBEXAAUD3DXVECTOR3@@ABU2@1@Z
void CSBLLDoorTile::GetEndVec(D3DXVECTOR3 &vEndVec, const D3DXVECTOR3 &vStartPt, const D3DXVECTOR3 &vEndPt) const
{
    vEndVec = vEndPt - vStartPt;
}

// Template and inline instances this unit also emits:

// 0x1007d310 ??_GCSBLLDoorTile@@UAEPAXI@Z

// 0x1007cc70 _$E22
// 0x1007cc90 _$E19
// 0x1007d260 _$E21
// 0x1007d280 _$E20
// 0x1007dd30 _$E75
// 0x1007dd70 _$E71
// 0x1007dd90 _$E74
