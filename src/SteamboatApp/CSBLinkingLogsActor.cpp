#include "CSBLinkingLogsGameBoard.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SteamboatNode.h"
#include <math.h>

// 0x1006e780 ??0CSBLinkingLogsActor@@QAE@PAVCLinkingLogsTile@@PAVCSBLinkingLogsGameBoard@@@Z
CSBLinkingLogsActor::CSBLinkingLogsActor(CLinkingLogsTile *pCurrentTile, CSBLinkingLogsGameBoard *pBoard)
    : m_fVelocity(0.75f), m_fKeyFramePos(0), m_nLastUpdateTime(-1), m_pCurrentTile(pCurrentTile), m_pBoard(pBoard),
      m_posNewDestination(pBoard->RowCount() - 1, pBoard->ColCount() - 1, 1, 0, 0, 0),
      m_posDestination(pBoard->RowCount() - 1, pBoard->ColCount() - 1, 1, 0, 0, 0),
      m_posHome(pCurrentTile->GetBoardPosition()), m_pAnim(0), m_pCraneAnim(0), m_bPreenterComplete(false),
      m_bGoingHome(false)
{
    // the walk sequences of the anim per direction
    m_mapDirectionToSequence[CGameBoardPos::GBUp] = 3;
    m_mapDirectionToSequence[CGameBoardPos::GBDown] = 1;
    m_mapDirectionToSequence[CGameBoardPos::GBLeft] = 0;
    m_mapDirectionToSequence[CGameBoardPos::GBRight] = 2;
    m_objCurrentAction = ActionStep();
    SteamboatNode::AddAnimToWorld(MBString("LLANCrane"), m_pCraneAnim);
    m_pCraneAnim->Play();
    m_pCraneAnim->Hide();
}

// 0x1006ebb0 ??1CSBLinkingLogsActor@@UAE@XZ
CSBLinkingLogsActor::~CSBLinkingLogsActor()
{
    // the last reference of an animation takes it out of the world
    if (m_pAnim && m_pAnim->Release() == 1) {
        IMBUnknown *iUnknown;
        m_pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
    if (m_pCraneAnim && m_pCraneAnim->Release() == 1) {
        IMBUnknown *iUnknown;
        m_pCraneAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// TODO 0x1006ee60 ?SetActor@CSBLinkingLogsActor@@QAEX_N@Z: 58% similar; the original keeps this in a
// stack slot as well (frame 4 bytes larger) and computes the inlined GetCenter's x and y completely
// before the GetPosition/GetZ call (registers); same calls and stores
void CSBLinkingLogsActor::SetActor(bool bPlayMickey)
{
    m_bMickye = bPlayMickey;
    MBString animName(bPlayMickey ? "LLANMickey" : "LLANMinnie");
    if (SteamboatNode::AddAnimToWorld(animName, m_pAnim) == 0) {
        m_pAnim->Play();
        UpdateAnimSequence();
        PlaceCenterd(m_pCurrentTile->GetCenter());
        m_rectAnim = m_pAnim->GetBounds();
    }
}

// TODO 0x1006f090 ?MoveTo@CSBLinkingLogsActor@@QAEXABVCGameBoardPos@@@Z: 58% similar; inline budget and
// registers: the original calls list::insert out of line for every push_back but the last four
// (Start/Walk with an out-of-line _Buynode, Stop/Idle fully inlined) and keeps eDirection in ebx, 0
// in edi (VC6: edi, ebx). The turn table is the gridlock actor's (CSBGridlockActor::MoveTo)
void CSBLinkingLogsActor::MoveTo(const CGameBoardPos &posDestination)
{
    CGameBoardPos posFinalDestination(posDestination);
    if (!m_bGoingHome) {
        CGameBoardPos::direction eDirection = BuildPath(posFinalDestination);
        if (eDirection != CGameBoardPos::GBInvalid) {
            m_posNewDestination = posDestination;
            m_queueActions.clear();
            // stop first unless already walking that way
            if (!(m_objCurrentAction.m_eDirection == eDirection && m_objCurrentAction.m_eAction == ActorWalk) &&
                m_objCurrentAction.m_eAction != ActorIdle) {
                if (m_objCurrentAction.m_eAction != ActorStop)
                    m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorStop));
                m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorIdle));
            }
            // turn a quarter at a time
            switch (eDirection) {
            case CGameBoardPos::GBRight:
                if (m_objCurrentAction.m_eDirection == eDirection)
                    break;
                switch (m_objCurrentAction.m_eDirection) {
                case CGameBoardPos::GBDown:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnCounterClock));
                    break;
                case CGameBoardPos::GBUp:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnClock));
                    break;
                case CGameBoardPos::GBLeft:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnCounterClock));
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnCounterClock));
                    break;
                }
                break;
            case CGameBoardPos::GBDown:
                if (m_objCurrentAction.m_eDirection == eDirection)
                    break;
                switch (m_objCurrentAction.m_eDirection) {
                case CGameBoardPos::GBRight:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnClock));
                    break;
                case CGameBoardPos::GBUp:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnClock));
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnClock));
                    break;
                case CGameBoardPos::GBLeft:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnCounterClock));
                    break;
                }
                break;
            case CGameBoardPos::GBLeft:
                if (m_objCurrentAction.m_eDirection == eDirection)
                    break;
                switch (m_objCurrentAction.m_eDirection) {
                case CGameBoardPos::GBDown:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnClock));
                    break;
                case CGameBoardPos::GBRight:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnCounterClock));
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnCounterClock));
                    break;
                case CGameBoardPos::GBUp:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnCounterClock));
                    break;
                }
                break;
            case CGameBoardPos::GBUp:
                if (m_objCurrentAction.m_eDirection == eDirection)
                    break;
                switch (m_objCurrentAction.m_eDirection) {
                case CGameBoardPos::GBDown:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnClock));
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnClock));
                    break;
                case CGameBoardPos::GBRight:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnCounterClock));
                    break;
                case CGameBoardPos::GBLeft:
                    m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnClock));
                    break;
                }
                break;
            }
            if (m_objCurrentAction.m_eDirection != eDirection || m_objCurrentAction.m_eAction != ActorWalk) {
                m_queueActions.push_back(ActionStep(eDirection, ActorStart));
                m_queueActions.push_back(ActionStep(eDirection, ActorWalk));
                if (!m_pCurrentTile->ShouldMoveInplace())
                    m_nLastUpdateTime = -1;
            }
            m_queueActions.push_back(ActionStep(eDirection, ActorStop));
            m_queueActions.push_back(ActionStep(eDirection, ActorIdle));
            if (m_objCurrentAction.m_eAction != ActorWalk || m_objCurrentAction.m_eDirection != eDirection)
                OnSequenceEnded();
            m_posNewDestination = posFinalDestination;
            m_pBoard->OnStartMove(m_pCurrentTile->GetBoardPosition(), posFinalDestination);
        }
    }
}

// 0x1006f7a0 ?IsMoveing@CSBLinkingLogsActor@@QBE_NXZ
bool CSBLinkingLogsActor::IsMoveing() const
{
    return m_nLastUpdateTime != -1;
}

// the crane's speed (a double literal: the code multiplies by a float constant)
const float GOHOME_VELOCITY = 120.0;

// TODO 0x1006f7c0 ?Update@CSBLinkingLogsActor@@QAEXXZ: 61% similar; the walking part matches up to the
// crane switch; there the original copies vCurrentPoint into vStartPoint with integer moves before the
// switch, builds the zero tangent before the difference vector, and merges the common
// StartSound("LLSEcrane") tails of the first two crane cases (VC6 copies through the FPU inside the
// cases and keeps both tails). Same calls, constants and stores
void CSBLinkingLogsActor::Update()
{
    // advance along the key frame; going home the crane carries the actor off the board and back
    if (IsMoveing()) {
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        long now = clock->GetGameTime();
        long elapsed = now - m_nLastUpdateTime;
        m_nLastUpdateTime = now;
        if (!m_bGoingHome)
            m_fKeyFramePos += elapsed * 0.001 / m_fDistance * m_fVelocity;
        else
            m_fKeyFramePos += elapsed * 0.001 / m_fDistance * GOHOME_VELOCITY;
        if (1.0f < m_fKeyFramePos)
            m_fKeyFramePos = 1.0f;
        D3DXVECTOR3 vCurrentPoint;
        m_objPathFrames.Hermite(vCurrentPoint, m_fKeyFramePos);
        if (!m_bGoingHome) {
            UpdateTiles(vCurrentPoint);
            UpdatePath();
            if (m_fKeyFramePos == 1.0f) {
                GetNextStep(m_objCurrentAction);
                UpdateAnimSequence();
                if (!m_pCurrentTile->OnActorStop(*this))
                    m_nLastUpdateTime = -1;
                else
                    m_nLastUpdateTime = clock->GetGameTime();
                m_fKeyFramePos = 0;
            }
            PlaceCenterd(MBPoint3((long)vCurrentPoint.x, (long)vCurrentPoint.y, (long)vCurrentPoint.z));
        } else {
        switch (m_objCurrentAction.m_eAction) {
        case ActorIdle:
            UpdateAnimSequence();
            GetNextStep(m_objCurrentAction);
            break;
        case ActorPickupCraneUp:
        case ActorDropoffCraneDown:
            PlaceCenterd(MBPoint3((long)vCurrentPoint.x, (long)vCurrentPoint.y, (long)vCurrentPoint.z));
            m_pCraneAnim->Place(MBPoint3((long)vCurrentPoint.x, (long)vCurrentPoint.y, m_pAnim->GetPosition().z - 1));
            break;
        case ActorPickupCraneDown:
        case ActorDropoffCraneUp:
            m_pCraneAnim->Place(MBPoint3((long)vCurrentPoint.x, (long)vCurrentPoint.y, m_pAnim->GetPosition().z - 1));
            break;
        }
        if (m_fKeyFramePos == 1.0f) {
            D3DXVECTOR3 vStartPoint = vCurrentPoint;
            D3DXVECTOR3 vEndPoint;
            switch (m_objCurrentAction.m_eAction) {
            case ActorPickupCraneDown: {
                // down to the actor
                vEndPoint = D3DXVECTOR3(vCurrentPoint.x, -50.0f, vCurrentPoint.z);
                m_objPathFrames.SetPoints(vStartPoint, vEndPoint - vCurrentPoint, vEndPoint, D3DXVECTOR3(0, 0, 0));
                float x = vEndPoint.x - vStartPoint.x;
                float y = vEndPoint.y - vStartPoint.y;
                float z = vEndPoint.z - vStartPoint.z;
                float fDistance = (float)sqrt(x * x + y * y + z * z);
                m_fDistance = fDistance < 0.0f ? -fDistance : fDistance;
                GetNextStep(m_objCurrentAction);
                m_pCraneAnim->SetCurrentSequence(1);
                m_fKeyFramePos = 0;
                SteamboatNode::GetActiveNode()->StartSound(m_strGoHomeSound.c_str());
                SteamboatNode::GetActiveNode()->StartSound("LLSEcranepickup");
                SteamboatNode::GetActiveNode()->StartSound("LLSEcrane");
                break;
            }
            case ActorPickupCraneUp: {
                // up and over to the home tile
                m_pBoard->GetTileAt(m_posDestination).GetStartPoint(vEndPoint);
                vStartPoint = D3DXVECTOR3(vEndPoint.x, -50.0f, vEndPoint.z);
                m_objPathFrames.SetPoints(vStartPoint, D3DXVECTOR3(0, 0, 0), vEndPoint, vEndPoint - vStartPoint);
                m_fKeyFramePos = 0;
                float x = vEndPoint.x - vStartPoint.x;
                float y = vEndPoint.y - vStartPoint.y;
                float z = vEndPoint.z - vStartPoint.z;
                float fDistance = (float)sqrt(x * x + y * y + z * z);
                m_fDistance = fDistance < 0.0f ? -fDistance : fDistance;
                GetNextStep(m_objCurrentAction);
                SteamboatNode::GetActiveNode()->StartSound("LLSEcrane");
                break;
            }
            case ActorDropoffCraneDown: {
                m_pBoard->GetTileAt(m_posDestination).GetStartPoint(vStartPoint);
                vEndPoint = D3DXVECTOR3(vStartPoint.x, -50.0f, vStartPoint.z);
                m_objPathFrames.SetPoints(vStartPoint, vEndPoint - vStartPoint, vEndPoint, D3DXVECTOR3(0, 0, 0));
                m_fKeyFramePos = 0;
                float x = vEndPoint.x - vStartPoint.x;
                float y = vEndPoint.y - vStartPoint.y;
                float z = vEndPoint.z - vStartPoint.z;
                float fDistance = (float)sqrt(x * x + y * y + z * z);
                m_fDistance = fDistance < 0.0f ? -fDistance : fDistance;
                m_pCraneAnim->SetCurrentSequence(0);
                GetNextStep(m_objCurrentAction);
                SteamboatNode::GetActiveNode()->StartSound("LLSEcrane");
                SteamboatNode::GetActiveNode()->StartSound("LLSEcranepickup");
                break;
            }
            case ActorDropoffCraneUp: {
                // the actor is back on the board
                CLinkingLogsTile *pHome = &m_pBoard->GetTileAt(m_posDestination);
                GetNextStep(m_objCurrentAction);
                pHome->OnActorEnter(*this);
                m_pCurrentTile = pHome;
                m_nLastUpdateTime = -1;
                m_bGoingHome = false;
                break;
            }
            }
        }
        }
    }
}

// 0x10070050 ?OnAnimEvent@CSBLinkingLogsActor@@QAEXJABUMBAnimEventStruct@@@Z
void CSBLinkingLogsActor::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    IMBUnknown *iEventAnimUnknown = 0;
    IMBUnknown *iActorAnimUnknown = 0;
    switch (event) {
    case 4:
    case 6:
        // the end of one of the actor's own sequences
        if (!m_bGoingHome) {
            refEventData.anim->QueryInterface(MBIID_IMBUnknown, (void **)&iEventAnimUnknown);
            m_pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iActorAnimUnknown);
            if (iEventAnimUnknown == iActorAnimUnknown)
                OnSequenceEnded();
            if (iEventAnimUnknown) {
                iEventAnimUnknown->Release();
                iEventAnimUnknown = 0;
            }
            if (iActorAnimUnknown) {
                iActorAnimUnknown->Release();
                iActorAnimUnknown = 0;
            }
        }
        break;
    }
}

// 0x10070120 ?OnSequenceEnded@CSBLinkingLogsActor@@IAEXXZ
void CSBLinkingLogsActor::OnSequenceEnded()
{
    ActionStep refNextStep;
    GetNextStep(refNextStep);
    if (m_objCurrentAction != refNextStep) {
        m_objCurrentAction = refNextStep;
        UpdateAnimSequence();
        // the walk starts the clock of the move
        if (m_nLastUpdateTime == -1 && m_objCurrentAction.m_eAction == ActorWalk) {
            MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
            m_nLastUpdateTime = clock->GetGameTime();
        }
    }
}

// 0x10070280 ?UpdateAnimSequence@CSBLinkingLogsActor@@IAEXXZ
void CSBLinkingLogsActor::UpdateAnimSequence()
{
    m_pAnim->SetCurrentSequence(m_mapDirectionToSequence[m_objCurrentAction.m_eDirection] + m_objCurrentAction.m_eAction);
    if (m_posNewDestination != m_posDestination) {
        // a new destination: the path changes when the actor can turn
        if (m_objCurrentAction.m_eAction == ActorStart || m_objCurrentAction.m_eAction == ActorWalk || m_objCurrentAction.m_eAction == ActorStop)
            SetPathTo(m_posNewDestination);
    }
}

// 0x10070340 ?GetNextStep@CSBLinkingLogsActor@@IAEXAAUActionStep@1@@Z
void CSBLinkingLogsActor::GetNextStep(ActionStep &refStep)
{
    if (m_queueActions.size()) {
        refStep = m_queueActions.front();
        m_queueActions.pop_front();
    } else {
        refStep = m_objCurrentAction;
    }
}

// 0x100703d0 ?BuildPath@CSBLinkingLogsActor@@IAE?AW4direction@CGameBoardPos@@AAV3@@Z
CGameBoardPos::direction CSBLinkingLogsActor::BuildPath(CGameBoardPos &posDestination)
{
    // walk straight towards the destination as far as the tiles allow
    CGameBoardPos posStart = m_pCurrentTile->GetBoardPosition();
    CGameBoardPos::direction eDirection = posStart.GetDirectionTo(posDestination);
    if (eDirection != CGameBoardPos::GBInvalid) {
        CGameBoardPos posCurrent(posStart);
        CGameBoardPos posLast(posCurrent);
        while (posCurrent != posDestination) {
            if (posCurrent.Move(eDirection)) {
                if (!m_pBoard->GetTileAt(posCurrent).CanMoveTo(*m_pCurrentTile)) {
                    posCurrent = posLast;
                    break;
                }
                posLast = posCurrent;
            }
        }
        if (posCurrent != posStart)
            posDestination = posCurrent;
        else
            eDirection = CGameBoardPos::GBInvalid;
    }
    return eDirection;
}

// 0x10070560 ?PlaceCenterd@CSBLinkingLogsActor@@IAEXVMBPoint3@@@Z
void CSBLinkingLogsActor::PlaceCenterd(MBPoint3 ptPosition)
{
    MBPoint3 ptTopLeft = ptPosition - MBPoint3(m_rectAnim.Width() / 2, m_rectAnim.Height() / 2, 0);
    ptTopLeft.z = 160;
    m_pAnim->Place(ptTopLeft);
}

// TODO 0x100705f0 ?SetPathTo@CSBLinkingLogsActor@@QAEXABVCGameBoardPos@@@Z: ebx and edi swapped and
// the original reloads m_pCurrentTile for each call (a pointer for the destination tile tried)
void CSBLinkingLogsActor::SetPathTo(const CGameBoardPos &posDestination)
{
    // a key frame from where the actor is now to the destination tile
    CLinkingLogsTile &refDestTile = m_pBoard->GetTileAt(posDestination);
    m_posDestination = posDestination;
    CGameBoardPos::direction eDirection = GetDirection();
    D3DXVECTOR3 vStartPoint;
    D3DXVECTOR3 vEndPoint;
    D3DXVECTOR3 vEndVec;
    D3DXVECTOR3 vStartVec;
    if (IsMoveing())
        m_objPathFrames.Hermite(vStartPoint, m_fKeyFramePos);
    else
        m_pCurrentTile->GetStartPoint(vStartPoint);
    refDestTile.GetEndPoint(vEndPoint);
    m_fVelocity = m_pCurrentTile->GetVelocity(m_objCurrentAction.m_eAction != ActorIdle && m_objCurrentAction.m_eDirection == eDirection ? eDirection : CGameBoardPos::GBInvalid);
    m_pCurrentTile->GetStartVec(vStartVec, vStartPoint, vEndPoint);
    refDestTile.GetEndVec(vEndVec, vStartPoint, vEndPoint);
    m_objPathFrames.SetPoints(vStartPoint, vStartVec, vEndPoint, vEndVec);
    float x = vEndPoint.x - vStartPoint.x;
    float y = vEndPoint.y - vStartPoint.y;
    float z = vEndPoint.z - vStartPoint.z;
    float fDistance = (float)sqrt(x * x + y * y + z * z);
    m_fDistance = fDistance < 0.0f ? -fDistance : fDistance;
    m_fKeyFramePos = 0.0f;
}

// TODO 0x10070770 ?GoHome@CSBLinkingLogsActor@@QAEXW4gohome_type@1@@Z: 48% similar; inline budget and
// registers: the original keeps this in ebp, inlines the steam case's list clear only one level (the
// iterator ++ and erase(it) called out of line; clear() here calls erase(first, last), erase(begin, end)
// inlines the ++), builds the home tile's centre without the out-of-line MBPoint3 constructor, calls
// _Eos out of line in the crane case's string assignment and multiplies the distance terms in another
// FPU order (x*x first); same calls, constants, stores and order of effects
void CSBLinkingLogsActor::GoHome(gohome_type eGoHomeType)
{
    // back to the home tile: steam and trap doors put the actor there at once, the crane carries it
    if (!m_bGoingHome && m_pCurrentTile->GetBoardPosition() != m_posHome) {
        switch (eGoHomeType) {
        case Steam:
        case TrapDoor: {
            CLinkingLogsTile &refHome = m_pBoard->GetTileAt(m_posHome);
            m_posDestination = m_posHome;
            m_pCurrentTile->OnActorExit(*this);
            refHome.OnActorEnter(*this);
            m_pCurrentTile = &refHome;
            m_nLastUpdateTime = -1;
            m_queueActions.erase(m_queueActions.begin(), m_queueActions.end());
            m_objCurrentAction = ActionStep();
            UpdateAnimSequence();
            PlaceCenterd(refHome.GetCenter());
            if (eGoHomeType == TrapDoor) {
                m_strGoHomeSound = m_bMickye ? "ll07xmc0010" : "ll07xmi0010";
                SteamboatNode::GetActiveNode()->StartSound(m_strGoHomeSound.c_str());
            }
            break;
        }
        case Crane: {
            MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
            m_posDestination = m_posHome;
            m_bGoingHome = true;
            m_objCurrentAction = ActionStep(m_objCurrentAction.m_eDirection, ActorIdle);
            m_queueActions.clear();
            m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorPickupCraneDown));
            m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorPickupCraneUp));
            m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorDropoffCraneDown));
            m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorDropoffCraneUp));
            m_queueActions.push_back(ActionStep(m_objCurrentAction.m_eDirection, ActorStop));
            // the crane comes down from above the actor
            D3DXVECTOR3 vEndPoint;
            if (IsMoveing())
                m_objPathFrames.Hermite(vEndPoint, m_fKeyFramePos);
            else
                m_pCurrentTile->GetStartPoint(vEndPoint);
            D3DXVECTOR3 vStartPoint = vEndPoint;
            vStartPoint.y = -50.0f;
            m_objPathFrames.SetPoints(vStartPoint, D3DXVECTOR3(0, 0, 0), vEndPoint, vStartPoint - vEndPoint);
            float x = vEndPoint.x - vStartPoint.x;
            float y = vEndPoint.y - vStartPoint.y;
            float z = vEndPoint.z - vStartPoint.z;
            float fDistance = (float)sqrt(x * x + y * y + z * z);
            m_fDistance = fDistance < 0.0f ? -fDistance : fDistance;
            m_pCraneAnim->SetCurrentSequence(0);
            m_pCraneAnim->Place(MBPoint3((long)vStartPoint.x, (long)vStartPoint.y, m_pAnim->GetPosition().z - 1));
            m_pCraneAnim->Show();
            SteamboatNode::GetActiveNode()->StartSound("LLSEcrane");
            m_pCurrentTile->OnActorExit(*this);
            UpdateAnimSequence();
            m_nLastUpdateTime = clock->GetGameTime();
            m_fKeyFramePos = 0;
            m_strGoHomeSound = m_bMickye ? "ll07xmc0020" : "ll07xmi0020";
            break;
        }
        }
    }
}

// 0x10070e40 ?GetDirection@CSBLinkingLogsActor@@QAE?AW4direction@CGameBoardPos@@XZ
CGameBoardPos::direction CSBLinkingLogsActor::GetDirection()
{
    return m_pCurrentTile->GetBoardPosition().GetDirectionTo(m_posDestination);
}

// Template and inline instances this unit also emits:

// 0x10070ec0 ?UpdateTiles@CSBLinkingLogsActor@@IAEXABUD3DXVECTOR3@@@Z
void CSBLinkingLogsActor::UpdateTiles(const D3DXVECTOR3 &vCurrentPoint)
{
    // the actor left its tile: exit the old one, enter the new one
    MBPoint ptCurrent((long)vCurrentPoint.x, (long)vCurrentPoint.y);
    if (!m_pCurrentTile->GetRect().ContainsPoint(ptCurrent)) {
        CGameBoardPos posNew = m_pCurrentTile->GetBoardPosition();
        CGameBoardPos::direction eDirection = GetDirection();
        posNew.Move(eDirection);
        CLinkingLogsTile &refNewTile = m_pBoard->GetTileAt(posNew);
        m_pCurrentTile->OnActorExit(*this);
        refNewTile.OnActorEnter(*this);
        m_pCurrentTile = &refNewTile;
        if (m_objCurrentAction.m_eAction == ActorIdle || m_objCurrentAction.m_eDirection != eDirection)
            eDirection = CGameBoardPos::GBInvalid;
        m_fVelocity = refNewTile.GetVelocity(eDirection);
        m_bPreenterComplete = false;
    }
}

// 0x10070ff0 ?UpdatePath@CSBLinkingLogsActor@@IAEXXZ
void CSBLinkingLogsActor::UpdatePath()
{
    // once the end of the current key frame leaves the tile, the actor steps onto the next one
    const D3DXVECTOR3 &vEnd = m_objPathFrames.GetEndPoint();
    MBPoint ptEnd((long)vEnd.x, (long)vEnd.y);
    if (!m_pCurrentTile->GetRect().ContainsPoint(ptEnd)) {
        CGameBoardPos posNext = m_pCurrentTile->GetBoardPosition();
        CGameBoardPos::direction eDirection = posNext.GetDirectionTo(m_posDestination);
        posNext.Move(eDirection);
        CLinkingLogsTile *pCurrentTile = m_pCurrentTile;
        if (!m_pBoard->GetTileAt(posNext).CanMoveTo(*pCurrentTile))
            SetPathTo(pCurrentTile->GetBoardPosition());
        else
            NotifyTiles(posNext, eDirection);
    }
}

// 0x10071120 ?NotifyTiles@CSBLinkingLogsActor@@IAEXABVCGameBoardPos@@W4direction@2@@Z
void CSBLinkingLogsActor::NotifyTiles(const CGameBoardPos &posNext, CGameBoardPos::direction eDirection)
{
    // the next tile and its two neighbours across the way learn that the actor is coming
    if (!m_bPreenterComplete) {
        CGameBoardPos posAdj(posNext);
        CGameBoardPos::direction eSide = CGameBoardPos::GBUp;
        if (eDirection != CGameBoardPos::GBLeft && eDirection != CGameBoardPos::GBRight)
            eSide = CGameBoardPos::GBLeft;
        if (posAdj.Move(eSide))
            m_pBoard->GetTileAt(posAdj).OnActorPreenter(*this);
        m_pBoard->GetTileAt(posNext).OnActorPreenter(*this);
        posAdj = posNext;
        if (posAdj.Move((CGameBoardPos::direction)~eSide))
            m_pBoard->GetTileAt(posAdj).OnActorPreenter(*this);
        m_bPreenterComplete = true;
    }
}

// 0x1006eac0 ?GetBoardPosition@CLinkingLogsTile@@QBE?AVCGameBoardPos@@XZ

// Template and inline instances this unit also emits:

// 0x1006eaf0 ??_GCSBLinkingLogsActor@@UAEPAXI@Z

// 0x1006eb30 ??1?$MBList@UActionStep@CSBLinkingLogsActor@@@@QAE@XZ

// 0x10071500 ??Eiterator@?$list@UActionStep@CSBLinkingLogsActor@@V?$MBAllocator@UActionStep@CSBLinkingLogsActor@@@@@std@@QAE?AV012@H@Z
// 0x10071250 ?insert@?$list@UActionStep@CSBLinkingLogsActor@@V?$MBAllocator@UActionStep@CSBLinkingLogsActor@@@@@std@@QAE?AViterator@12@V312@ABUActionStep@CSBLinkingLogsActor@@@Z
// 0x10071320 ?erase@?$list@UActionStep@CSBLinkingLogsActor@@V?$MBAllocator@UActionStep@CSBLinkingLogsActor@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x100713a0 ?_Buynode@?$list@UActionStep@CSBLinkingLogsActor@@V?$MBAllocator@UActionStep@CSBLinkingLogsActor@@@@@std@@IAEPAU_Node@12@PAU312@0@Z
// 0x100713e0 ?insert@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@std@@QAE?AU?$pair@Viterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@_N@2@ABU?$pair@$$CBW4direction@CGameBoardPos@@J@2@@Z
// 0x10071420 ??0?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAE@ABU?$less@W4direction@CGameBoardPos@@@1@_NABV?$MBAllocator@J@@@Z
// 0x10071520 ?_Init@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@IAEXXZ
// 0x100712d0 ?erase@?$list@UActionStep@CSBLinkingLogsActor@@V?$MBAllocator@UActionStep@CSBLinkingLogsActor@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100715c0 ?_Construct@std@@YAXPAUActionStep@CSBLinkingLogsActor@@ABU23@@Z

// 0x1006e140 _$E32
// 0x1006e160 _$E29
// 0x1006e730 _$E31
// 0x1006e750 _$E30
// 0x100715f0 _$E75
// 0x10071630 _$E71
// 0x10071650 _$E74
