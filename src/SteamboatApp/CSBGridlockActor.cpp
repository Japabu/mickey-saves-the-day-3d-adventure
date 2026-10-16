#include "CSBGridlockActor.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBGridLockGameBoard.h"

MBMap<CGameBoardPos::direction, long, std::less<CGameBoardPos::direction> > CSBGridlockActor::m_mapDirectionToSequence;

// 0x10054600 ??0CSBGridlockActor@@QAE@J@Z
CSBGridlockActor::CSBGridlockActor(long nVelocity) : m_pTile(0), m_nVelocity(nVelocity), m_bMoveing(false)
{
    // The animation sequence of each walking direction (the action is added to it).
    if (m_mapDirectionToSequence.size() == 0) {
        m_mapDirectionToSequence[CGameBoardPos::GBUp] = 3;
        m_mapDirectionToSequence[CGameBoardPos::GBDown] = 1;
        m_mapDirectionToSequence[CGameBoardPos::GBLeft] = 0;
        m_mapDirectionToSequence[CGameBoardPos::GBRight] = 2;
    }
}

// 0x10054860 ??_GCSBGridlockActor@@UAEPAXI@Z

// 0x10054950 ??0CSBGridlockActor@@QAE@ABV0@@Z
CSBGridlockActor::CSBGridlockActor(const CSBGridlockActor &refActorToCopy)
    : m_queueActions(refActorToCopy.m_queueActions), m_objCurrentAction(refActorToCopy.m_objCurrentAction),
      m_pTile(refActorToCopy.m_pTile), m_nVelocity(refActorToCopy.m_nVelocity), m_bMoveing(refActorToCopy.m_bMoveing)
{
    m_objActorPath = refActorToCopy.m_objActorPath;
}

// 0x10054a70 ??1CSBGridlockActor@@UAE@XZ
CSBGridlockActor::~CSBGridlockActor()
{
}

// 0x10054b60 ?Attach@CSBGridlockActor@@UAEXAAVCSBGridlockGameTile@@@Z
void CSBGridlockActor::Attach(CSBGridlockGameTile &refTile)
{
    m_pTile = &refTile;
    m_bMoveing = false;
    m_objCurrentAction.m_posRoot = refTile.GetBoardPosition();
}

// 0x10054bf0 ?GetBoardPosition@CSBGridlockGameTile@@QBE?AVCGameBoardPos@@XZ

// 0x10054c20 ?SetPosition@CSBGridlockActor@@UAEXABVCGameBoardPos@@W4direction@2@@Z
void CSBGridlockActor::SetPosition(const CGameBoardPos &posTilePosition, CGameBoardPos::direction eStartDirection)
{
    m_objCurrentAction.m_eDirection = eStartDirection;
}

// 0x10055750 ??0ActionStep@CSBGridlockActor@@QAE@W4direction@CGameBoardPos@@W4action@1@ABV3@@Z (inline)

// TODO 0x10054c40 ?MoveTo@CSBGridlockActor@@UAEXABVCGameBoardPos@@_N@Z: all but the end matches; the original still inlines the push_back of the stop step and calls the last ActionStep constructor out of line, VC6 here the reverse (inline budget; insert(end(), ...) for the last two inlines both inserts but then the constructor too) (98% similar)
void CSBGridlockActor::MoveTo(const CGameBoardPos &posTilePosition, bool bLastMove)
{
    ActionStep stepRest;
    GetRestPosition(stepRest);
    CGameBoardPos::direction eDirection = stepRest.m_posRoot.GetDirectionTo(posTilePosition);
    m_objActorPath.SetPath(m_pTile->GetOrigin(), m_pTile->GetGameBoard()->GetTileAt(posTilePosition, true).GetOrigin(), m_nVelocity);

    // Turn from the direction the actor faces at the end of its queue to the new one, a quarter
    // turn at a time.
    switch (eDirection) {
    case CGameBoardPos::GBLeft:
        if (stepRest.m_eDirection == eDirection)
            break;
        switch (stepRest.m_eDirection) {
        case CGameBoardPos::GBDown:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBRight:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnCounterClock, stepRest.m_posRoot));
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBUp:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        }
        break;
    case CGameBoardPos::GBDown:
        if (stepRest.m_eDirection == eDirection)
            break;
        switch (stepRest.m_eDirection) {
        case CGameBoardPos::GBRight:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBUp:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnClock, stepRest.m_posRoot));
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBLeft:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        }
        break;
    case CGameBoardPos::GBRight:
        if (stepRest.m_eDirection == eDirection)
            break;
        switch (stepRest.m_eDirection) {
        case CGameBoardPos::GBDown:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBUp:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBUp, ActorTurnClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBLeft:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnCounterClock, stepRest.m_posRoot));
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        }
        break;
    case CGameBoardPos::GBUp:
        if (stepRest.m_eDirection == eDirection)
            break;
        switch (stepRest.m_eDirection) {
        case CGameBoardPos::GBDown:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBDown, ActorTurnClock, stepRest.m_posRoot));
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBRight:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBRight, ActorTurnCounterClock, stepRest.m_posRoot));
            break;
        case CGameBoardPos::GBLeft:
            m_queueActions.push_back(ActionStep(CGameBoardPos::GBLeft, ActorTurnClock, stepRest.m_posRoot));
            break;
        }
        break;
    }
    if (stepRest.m_eAction == ActorIdle)
        m_queueActions.push_back(ActionStep(eDirection, ActorStart, stepRest.m_posRoot));
    m_queueActions.push_back(ActionStep(eDirection, ActorWalk, stepRest.m_posRoot));
    if (bLastMove) {
        m_queueActions.push_back(ActionStep(eDirection, ActorStop, posTilePosition));
        m_queueActions.push_back(ActionStep(eDirection, ActorIdle, posTilePosition));
    }
    m_bMoveing = true;
    OnSequenceEnded();
}

// 0x10055790 ?IsMoveing@CSBGridlockActor@@UBE_NXZ
bool CSBGridlockActor::IsMoveing() const
{
    return m_bMoveing;
}

// EQUIV 0x100557b0 ?Update@CSBGridlockActor@@UAEXXZ: reviewed: the constant 8 is compared as an immediate instead of from a register
void CSBGridlockActor::Update()
{
    if (IsMoveing()) {
        MBPoint3 ptNext;
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        if (!m_objActorPath.IsRunning()) {
            if (m_objCurrentAction.m_eAction == ActorWalk)
                m_objActorPath.Start(clock);
        } else if (m_objCurrentAction.m_eAction == ActorWalk) {
            bool bArrived = !m_objActorPath.NextPoint(ptNext, clock);
            m_pTile->SetOrigin(ptNext);
            if (bArrived) {
                GetNextStep(m_objCurrentAction);
                UpdateAnimSequence();
            }
        }
    } else if (!IsMoveing())
        UpdateAnimSequence();
}

// 0x10055930 ?OnAnimEvent@CSBGridlockActor@@UAEXJABUMBAnimEventStruct@@@Z
void CSBGridlockActor::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    switch (event) {
    case 4:
    case 6:
        OnSequenceEnded();
    }
}

// 0x10055960 ?OnSequenceEnded@CSBGridlockActor@@IAEXXZ
void CSBGridlockActor::OnSequenceEnded()
{
    ActionStep step;
    GetNextStep(step);
    if (m_objCurrentAction != step) {
        m_objCurrentAction = step;
        UpdateAnimSequence();
    }
}

// 0x10055a50 ?UpdateAnimSequence@CSBGridlockActor@@IAEXXZ
void CSBGridlockActor::UpdateAnimSequence()
{
    m_pTile->SetCurrentSequence(m_mapDirectionToSequence[m_objCurrentAction.m_eDirection] + m_objCurrentAction.m_eAction);
}

// 0x10055ac0 ?GetRestPosition@CSBGridlockActor@@IAEXAAUActionStep@1@@Z
void CSBGridlockActor::GetRestPosition(ActionStep &refStep)
{
    if (m_queueActions.size())
        refStep = m_queueActions.back();
    else
        refStep = m_objCurrentAction;
}

// 0x10055b30 ?GetNextStep@CSBGridlockActor@@IAEXAAUActionStep@1@@Z
void CSBGridlockActor::GetNextStep(ActionStep &refStep)
{
    if (m_queueActions.size()) {
        refStep = m_queueActions.front();
        m_queueActions.pop_front();
    } else {
        refStep = m_objCurrentAction;
        m_bMoveing = false;
    }
}

// The containers' template instances (emitted by the code above):
// 0x100544a0 ??1?$MBMap@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@@@QAE@XZ
// 0x10054540 ??0?$MBMap@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@@@QAE@XZ
// 0x100548a0 ??1?$MBList@UActionStep@CSBGridlockActor@@@@QAE@XZ
// 0x10054930 ??1ActionStep@CSBGridlockActor@@QAE@XZ
// 0x10055bf0 ?insert@?$list@UActionStep@CSBGridlockActor@@V?$MBAllocator@UActionStep@CSBGridlockActor@@@@@std@@QAE?AViterator@12@V312@ABUActionStep@CSBGridlockActor@@@Z
// 0x10055c70 ?erase@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10055df0 ?insert@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAE?AU?$pair@Viterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@_N@2@ABU?$pair@$$CBW4direction@CGameBoardPos@@J@2@@Z
// 0x10055f70 ?erase@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAE?AViterator@12@V312@@Z
// 0x10056670 ??0?$pair@Viterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@_N@std@@QAE@ABViterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@1@AB_N@Z
// 0x100566a0 ?_Insert@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBW4direction@CGameBoardPos@@J@2@@Z
// 0x10056a30 ?_Dec@const_iterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAEXXZ
// 0x10056b20 ?_Inc@const_iterator@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@QAEXXZ
// 0x10056c00 ?_Construct@std@@YAXPAUActionStep@CSBGridlockActor@@ABU23@@Z
// 0x10056c80 ?_Construct@std@@YAXPAU?$pair@$$CBW4direction@CGameBoardPos@@J@1@ABU21@@Z

// Instances the linker took from other objects (this unit emits them too):
// 0x10056d30 ??0MBPoint3@@QAE@JJJ@Z

// Template and inline instances this unit also emits:
// 0x100565b0 ?_Erase@?$_Tree@W4direction@CGameBoardPos@@U?$pair@$$CBW4direction@CGameBoardPos@@J@std@@U_Kfn@?$map@W4direction@CGameBoardPos@@JU?$less@W4direction@CGameBoardPos@@@std@@V?$MBAllocator@J@@@4@U?$less@W4direction@CGameBoardPos@@@4@V?$MBAllocator@J@@@std@@IAEXPAU_Node@12@@Z

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10054410 _$E62
// 0x10054430 _$E58
// 0x10054450 _$E61
// 0x10054470 _$E59
// 0x10056cb0 _$E73
// 0x10056cf0 _$E69
// 0x10056d10 _$E72
