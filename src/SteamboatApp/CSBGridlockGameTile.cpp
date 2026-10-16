#include "CSBGridLockGameBoard.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "CSBGridlockActor.h"
#include "CVectorPath.h"
#include "SteamboatNode.h"

// 0x10066320 ??0CSBGridlockGameTile@@QAE@ABV0@@Z
CSBGridlockGameTile::CSBGridlockGameTile(const CSBGridlockGameTile &refObjToCopy)
    : m_pCurrentPath(0), m_pGameBoard(refObjToCopy.m_pGameBoard), m_pPolyClickable(refObjToCopy.m_pPolyClickable),
      m_pButtClickable(refObjToCopy.m_pButtClickable), m_pActor(0), m_recTile(refObjToCopy.m_recTile),
      m_posBoard(refObjToCopy.m_posBoard), m_nProperties(refObjToCopy.m_nProperties),
      m_bMoveing(refObjToCopy.m_bMoveing), m_bDirty(true)
{
    if (refObjToCopy.m_pCurrentPath)
        m_pCurrentPath = new CVectorPath(*refObjToCopy.m_pCurrentPath);
    if (m_pPolyClickable)
        m_pPolyClickable->AddRef();
    if (m_pButtClickable)
        m_pButtClickable->AddRef();
    if (refObjToCopy.m_pActor) {
        m_pActor = new CSBGridlockActor(*refObjToCopy.m_pActor);
        m_pActor->Attach(*this);
    }
}

// 0x10066490 ??_GCSBGridlockGameTile@@UAEPAXI@Z

// 0x100664d0 ??0CSBGridlockGameTile@@QAE@XZ
CSBGridlockGameTile::CSBGridlockGameTile()
    : m_pCurrentPath(0), m_pGameBoard(0), m_pPolyClickable(0), m_pButtClickable(0), m_pActor(0),
      m_posBoard(0, 0, 0, 0, 0, 0), m_nProperties(0), m_bMoveing(false), m_bDirty(true)
{
}

// 0x10066540 ?InitPolyInterface@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::InitPolyInterface()
{
    if (m_pButtClickable) {
        m_pButtClickable->Release();
        m_pButtClickable = 0;
    }
    IMBUnknown *iUnknown;
    MBComCreateInstance(0, MBIID_CMBScreenPoly, &iUnknown);
    iUnknown->QueryInterface(MBIID_IMBScreenPoly, (void **)&m_pPolyClickable);
    if (iUnknown)
        iUnknown->Release();
}

// 0x100665b0 ?InitButtonInterface@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::InitButtonInterface()
{
    if (m_pPolyClickable) {
        m_pPolyClickable->Release();
        m_pPolyClickable = 0;
    }
    IMBUnknown *iUnknown;
    MBComCreateInstance(0, MBIID_CSBAnimButton, &iUnknown);
    iUnknown->QueryInterface(MBIID_ISBAnimButton, (void **)&m_pButtClickable);
    if (iUnknown)
        iUnknown->Release();
}

// 0x10066620 ?Initialize@CSBGridlockGameTile@@UAEXPAVCSBGridLockGameBoard@@ABVCGameBoardPos@@ABVMBRect@@@Z
void CSBGridlockGameTile::Initialize(CSBGridLockGameBoard *pGameBoard, const CGameBoardPos &posTilePosition, const MBRect &refTileRect)
{
    m_pGameBoard = pGameBoard;
    m_posBoard = posTilePosition;
    m_recTile = refTileRect;
    m_bDirty = true;
}

// 0x10066670 ??1CSBGridlockGameTile@@UAE@XZ
CSBGridlockGameTile::~CSBGridlockGameTile()
{
    // The world holds the last references: take the clickables out with them.
    if (m_pButtClickable && m_pButtClickable->Release() <= 2)
        RemoveButtonFromWorld();
    if (m_pPolyClickable && m_pPolyClickable->Release() <= 2)
        RemovePolyFromWorld();
    delete m_pCurrentPath;
    m_pCurrentPath = 0;
    delete m_pActor;
    m_pActor = 0;
}

// 0x10066750 ??4CSBGridlockGameTile@@QAEHABV0@@Z
// The original returns nothing (eax is whatever the last call left); VC6 reports that as error C4716.
#pragma warning(push)
#pragma warning(disable : 4716)
int CSBGridlockGameTile::operator=(const CSBGridlockGameTile &refObjToCopy)
{
    if (refObjToCopy.m_pCurrentPath) {
        delete m_pCurrentPath;
        m_pCurrentPath = new CVectorPath(*refObjToCopy.m_pCurrentPath);
    }
    IMBScreenPoly *pOldPoly = m_pPolyClickable;
    m_pPolyClickable = 0;
    if (refObjToCopy.m_pPolyClickable) {
        m_pPolyClickable = refObjToCopy.m_pPolyClickable;
        m_pPolyClickable->AddRef();
    }
    if (pOldPoly)
        pOldPoly->Release();
    ISBAnimButton *pOldButton = m_pButtClickable;
    m_pButtClickable = 0;
    if (refObjToCopy.m_pButtClickable) {
        m_pButtClickable = refObjToCopy.m_pButtClickable;
        m_pButtClickable->AddRef();
    }
    if (pOldButton)
        pOldButton->Release();
    m_pGameBoard = refObjToCopy.m_pGameBoard;
    m_recTile = refObjToCopy.m_recTile;
    m_posBoard = refObjToCopy.m_posBoard;
    m_nProperties = refObjToCopy.m_nProperties;
    m_bMoveing = refObjToCopy.m_bMoveing;
    m_bDirty = true;
    delete m_pActor;
    m_pActor = 0;
    if (refObjToCopy.m_pActor) {
        m_pActor = new CSBGridlockActor(*refObjToCopy.m_pActor);
        m_pActor->Attach(*this);
    }
}
#pragma warning(pop)

// 0x10066910 ?MoveTo@CSBGridlockGameTile@@UAEXABVCGameBoardPos@@_N@Z
void CSBGridlockGameTile::MoveTo(const CGameBoardPos &posTilePosition, bool bLastMove)
{
    if (IsActor())
        m_pActor->MoveTo(posTilePosition, bLastMove);
    else {
        MBPoint3 ptEnd = m_pGameBoard->GetTileAt(posTilePosition, true).GetOrigin();
        MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
        m_pCurrentPath = new CVectorPath(GetOrigin(), ptEnd, 28.0f);
        m_pCurrentPath->Start(clock);
    }
    m_pGameBoard->NotifyMoveStarted(*this);
    SetBoardPosition(posTilePosition);
    m_bMoveing = true;
}

// 0x10066ad0 ?SetOrigin@CSBGridlockGameTile@@QAEXABVMBPoint3@@@Z
void CSBGridlockGameTile::SetOrigin(const MBPoint3 &ptNewOrigin)
{
    m_recTile.bottom = m_recTile.Height() + ptNewOrigin.y;
    m_recTile.top = ptNewOrigin.y;
    m_recTile.right = m_recTile.Width() + ptNewOrigin.x;
    m_recTile.left = ptNewOrigin.x;
    m_bDirty = true;
}

// 0x10066b20 ?SetBoardPosition@CSBGridlockGameTile@@UAEXABVCGameBoardPos@@@Z
void CSBGridlockGameTile::SetBoardPosition(const CGameBoardPos &refPosition)
{
    m_posBoard = refPosition;
}

// 0x10066b40 ?AddToButtonGroup@CSBGridlockGameTile@@UAEXAAVCMBButtonGroup@@@Z
void CSBGridlockGameTile::AddToButtonGroup(CMBButtonGroup &refButtonGroup)
{
    if (m_pButtClickable && !IsActor() && IsSelectable())
        refButtonGroup.AddButton(m_pButtClickable, -1);
}

// 0x10066b80 ?AddToMessageMap@CSBGridlockGameTile@@UAEXAAV?$MBMap@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@@@@Z
void CSBGridlockGameTile::AddToMessageMap(iunknownmessagemap &refMessageMap)
{
    IMBUnknown *iUnknown = 0;
    if (m_pButtClickable) {
        // Clicks come from the button, animation events from its animation.
        m_pButtClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        refMessageMap.erase(iUnknown);
        refMessageMap.insert(iunknownmessagemap::value_type(iUnknown, this));
        m_pButtClickable->SetClickMessage(410, iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        m_pButtClickable->GetAnim()->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        refMessageMap.erase(iUnknown);
        refMessageMap.insert(iunknownmessagemap::value_type(iUnknown, this));
        if (iUnknown)
            iUnknown->Release();
    } else if (m_pPolyClickable) {
        m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        refMessageMap.erase(iUnknown);
        refMessageMap.insert(iunknownmessagemap::value_type(iUnknown, this));
        m_pPolyClickable->SetClickMessage(410, iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// 0x10066e40 ?SetTileProperties@CSBGridlockGameTile@@UAEXPAVIMBAnim@@W4tileprop@1@@Z
void CSBGridlockGameTile::SetTileProperties(IMBAnim *pAnim, tileprop eProp)
{
    m_nProperties = eProp;
    if (pAnim) {
        InitButtonInterface();
        m_pButtClickable->SetAnim(pAnim);
        m_pButtClickable->SetBroadcastChannel(400);
        AddButtonToWorld();
        if (IsActor()) {
            delete m_pActor;
            m_pActor = new CSBGridlockActor(6);
            m_pActor->Attach(*this);
            m_pActor->Update();
            m_pButtClickable->SetBehavior((MBButtonBehavior)4);
        } else
            m_pButtClickable->SetBehavior((MBButtonBehavior)2);
    } else {
        InitPolyInterface();
        m_pPolyClickable->SetBroadcastChannel(400);
        AddPolyToWorld();
    }
    m_bDirty = true;
}

// 0x10066f80 ?Update@CSBGridlockGameTile@@UAEXXZ
void CSBGridlockGameTile::Update()
{
    if (m_bMoveing) {
        if (m_pCurrentPath) {
            MBPoint3 ptNewOrigin;
            MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
            m_bMoveing = m_pCurrentPath->NextPoint(ptNewOrigin, clock);
            SetOrigin(ptNewOrigin);
        } else if (IsActor()) {
            m_pActor->Update();
            m_bMoveing = m_pActor->IsMoveing();
        }
        if (!m_bMoveing) {
            delete m_pCurrentPath;
            m_pCurrentPath = 0;
            m_pGameBoard->NotifyMoveEnded(*this);
        }
        Place();
    } else if (m_bDirty) {
        Place();
        m_bDirty = false;
    }
}

// 0x10067100 ?IsSwappable@CSBGridlockGameTile@@UBE_NABV1@@Z
bool CSBGridlockGameTile::IsSwappable(const CSBGridlockGameTile &refTile) const
{
    bool bSwappable = false;
    if (IsMoveable() && refTile.IsMoveable()) {
        if (IsActor()) {
            if (!refTile.IsSelectable())
                bSwappable = true;
        } else if (refTile.IsActor()) {
            if (!IsSelectable())
                bSwappable = true;
        } else if (IsStackable() && refTile.IsStackable()) {
            // A selectable tile swaps with a stacked one that is not at the top or bottom row.
            if (IsSelectable() && !refTile.IsSelectable()) {
                if (refTile.GetBoardPosition().Row() != refTile.GetBoardPosition().MinRow()
                    && refTile.GetBoardPosition().Row() != refTile.GetBoardPosition().MaxRow())
                    bSwappable = true;
            } else if (refTile.IsSelectable() && !IsSelectable()) {
                if (GetBoardPosition().Row() != GetBoardPosition().MinRow()
                    && GetBoardPosition().Row() != GetBoardPosition().MaxRow())
                    bSwappable = true;
            }
        }
    } else if ((!IsActor() && IsMoveable() && refTile.IsStackable())
        || (IsActor() && IsStackable() && refTile.IsStackable())
        || (!refTile.IsActor() && refTile.IsMoveable() && IsStackable())
        || (refTile.IsActor() && refTile.IsStackable() && IsStackable()))
        bSwappable = true;
    return bSwappable;
}

// 0x10067500 ?GetClickableInterface@CSBGridlockGameTile@@UAEPAVIMBUnknown@@XZ
IMBUnknown *CSBGridlockGameTile::GetClickableInterface()
{
    IMBUnknown *iUnknown = 0;
    if (m_pButtClickable)
        m_pButtClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    else if (m_pPolyClickable)
        m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    return iUnknown;
}

// 0x10067550 ?OnAnimEvent@CSBGridlockGameTile@@UAEXJABUMBAnimEventStruct@@@Z
void CSBGridlockGameTile::OnAnimEvent(long event, const MBAnimEventStruct &refEventData)
{
    if (m_pActor && m_pButtClickable && m_pButtClickable->GetAnim() == refEventData.anim)
        m_pActor->OnAnimEvent(event, refEventData);
}

// 0x100675c0 ?SetCurrentSequence@CSBGridlockGameTile@@UAE_NJ@Z
bool CSBGridlockGameTile::SetCurrentSequence(long i)
{
    bool bResult = true;
    if (m_pButtClickable) {
        if (m_pButtClickable->GetAnim()->IsStalled() || i != m_pButtClickable->GetAnim()->GetCurrentSequenceIndex()) {
            bResult = m_pButtClickable->GetAnim()->SetCurrentSequence(i);
            Place();
        }
    }
    return bResult;
}

// EQUIV 0x10067730 ?Place@CSBGridlockGameTile@@AAEXXZ: reviewed: the centre is computed the same ((right - left) / 2 + left, (bottom - top) / 2 + top, then the z call); the original loads the four rectangle members once into ebx/ebp/ecx/eax, VC6 here loads left and top twice with no store in between; same calls and stores
void CSBGridlockGameTile::Place()
{
    if (m_pButtClickable) {
        MBPoint3 ptCenter = GetCenter();
        m_pButtClickable->PlaceCentered(ptCenter);
    } else if (m_pPolyClickable) {
        m_pPolyClickable->SetRect(m_recTile, 1);
        m_pPolyClickable->SetZ(500);
    }
}

// 0x100677f0 ?AddPolyToWorld@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::AddPolyToWorld()
{
    if (m_pPolyClickable) {
        IMBUnknown *iUnknown = 0;
        m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        m_pPolyClickable->SetClickMessage(410, iUnknown);
        SteamboatNode::AddToWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// 0x10067860 ?RemovePolyFromWorld@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::RemovePolyFromWorld()
{
    if (m_pPolyClickable) {
        IMBUnknown *iUnknown = 0;
        m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
    }
}

// 0x100678a0 ?AddButtonToWorld@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::AddButtonToWorld()
{
    if (m_pButtClickable) {
        IMBUnknown *iUnknown = 0;
        m_pButtClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        m_pButtClickable->SetClickMessage(410, iUnknown);
        SteamboatNode::AddToWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// 0x10067910 ?RemoveButtonFromWorld@CSBGridlockGameTile@@AAEXXZ
void CSBGridlockGameTile::RemoveButtonFromWorld()
{
    if (m_pButtClickable) {
        IMBUnknown *iUnknown = 0;
        m_pButtClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// The message map's template instances (emitted by the code above):
// 0x10067960 ?insert@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@_N@2@ABU?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@2@@Z
// 0x10067ae0 ?equal_range@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@V123@@2@ABQAVIMBUnknown@@@Z
// 0x10067b20 ?_Insert@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@2@@Z
// 0x10067eb0 ?upper_bound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AViterator@12@ABQAVIMBUnknown@@@Z
// 0x10067ee0 ?_Dec@const_iterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAEXXZ
// 0x10067fd0 ?_Ubound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@IBEPAU_Node@12@ABQAVIMBUnknown@@@Z
// 0x10068040 ?_Distance@std@@YAXViterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@1@0AAI@Z
// 0x10068080 ?_Construct@std@@YAXPAU?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@1@ABU21@@Z
// 0x10054bf0 ?GetBoardPosition@CSBGridlockGameTile@@QBE?AVCGameBoardPos@@XZ (inline, out of line in SBGridlockActor.obj)

// Instances the linker took from other objects (this unit emits them too):
// 0x1000bd50 ??1?$MBComPtr@VIMBGameClock@@@@QAE@XZ
// 0x1000fe00 ?_QueryInterface@?$MBComPtr@VIMBGameClock@@@@AAEJABQAVIMBUnknown@@@Z
// 0x1005ded0 ?erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1005fb40 ?erase@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10060180 ?lower_bound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAE?AViterator@12@ABQAVIMBUnknown@@@Z
// 0x10064580 ?_Inc@const_iterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCSBGridlockGameTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCSBGridlockGameTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCSBGridlockGameTile@@@@@std@@QAEXXZ

// Static initializers and their atexit stubs: the PDB numbers them by the original header set, which
// ours does not reproduce yet, so they cannot be claimed by name.
// 0x10065ce0 _$E57
// 0x10065d00 _$E54
// 0x100662d0 _$E56
// 0x100662f0 _$E55
// 0x100680b0 _$E72
// 0x100680f0 _$E68
// 0x10068110 _$E71
