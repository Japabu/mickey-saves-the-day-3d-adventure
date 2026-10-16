#include "CLinkingLogsTile.h"
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "SteamboatNode.h"

CLinkingLogsTile::sequencemap CLinkingLogsTile::m_mapSequenceRoot;
bool CLinkingLogsTile::m_bSequenceMapInit;

// 0x1002ff90 ??0CLinkingLogsTile@@QAE@ABUtileinfo@0@PAVCSBLinkingLogsGameBoard@@@Z
CLinkingLogsTile::CLinkingLogsTile(const tileinfo &refInfo, CSBLinkingLogsGameBoard *pBoard)
    : m_pGameBoard(pBoard), m_pPolyClickable(0), m_pAnim(0), m_pActor(0), m_bHasMoved(true),
      m_bStateLocked(false)
{
    IMBUnknown *iUnknown = 0;
    if (!m_bSequenceMapInit) {
        // the first sequence of each tile type in its animation
        m_mapSequenceRoot[kPath] = kInvalidSeq;
        m_mapSequenceRoot[kDoor] = kDoorRoot;
        m_mapSequenceRoot[kSwitch] = kDoorSwitchRoot;
        m_mapSequenceRoot[kTrapdoor] = kTrapdoorRoot;
        m_mapSequenceRoot[kOneWayDoor] = kDoorRoot;
        m_mapSequenceRoot[kConveyer] = kkConveyerRoot;
        m_mapSequenceRoot[kSteam] = kSteamRoot;
        m_mapSequenceRoot[kInvalidType] = kInvalidSeq;
        m_bSequenceMapInit = true;
    }
    m_objProperties = refInfo;
    InitPolyInterface();
    if (m_objProperties.m_strAnimName.length() && SteamboatNode::AddAnimToWorld(m_objProperties.m_strAnimName, m_pAnim) == 0) {
        m_pAnim->Play();
        SetCurrentSequence(0, true);
        m_rectAnim = m_pAnim->GetBounds();
    }
    m_pPolyClickable->SetBroadcastChannel(400);
    m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    m_pPolyClickable->SetClickMessage(410, iUnknown);
    SteamboatNode::AddToWorld(iUnknown);
    SetCurrentSequence(0, true);
    if (iUnknown)
        iUnknown->Release();
}

// 0x100305c0 ??0CLinkingLogsTile@@QAE@ABV0@@Z
CLinkingLogsTile::CLinkingLogsTile(const CLinkingLogsTile &refObjToCopy)
    : m_pGameBoard(refObjToCopy.m_pGameBoard), m_pPolyClickable(refObjToCopy.m_pPolyClickable),
      m_pAnim(refObjToCopy.m_pAnim), m_pActor(refObjToCopy.m_pActor),
      m_objProperties(refObjToCopy.m_objProperties), m_bHasMoved(true),
      m_bStateLocked(refObjToCopy.m_bStateLocked)
{
    if (m_pPolyClickable)
        m_pPolyClickable->AddRef();
    if (m_pAnim)
        m_pAnim->AddRef();
}

// 0x10030720 ??1CLinkingLogsTile@@UAE@XZ
CLinkingLogsTile::~CLinkingLogsTile()
{
    // the last references of the hot spot and the animation take them out of the world
    if (m_pPolyClickable && m_pPolyClickable->Release() == 2) {
        IMBUnknown *iUnknown;
        m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
    if (m_pAnim && m_pAnim->Release() == 1) {
        IMBUnknown *iUnknown;
        m_pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown)
            iUnknown->Release();
    }
}

// 0x10030880 ??4CLinkingLogsTile@@QAEABV0@ABV0@@Z
const CLinkingLogsTile &CLinkingLogsTile::operator=(const CLinkingLogsTile &refObjToCopy)
{
    // the hot spot is shared, the animation cloned
    IMBScreenPoly *pOldPoly = m_pPolyClickable;
    m_pPolyClickable = 0;
    if (refObjToCopy.m_pPolyClickable) {
        m_pPolyClickable = refObjToCopy.m_pPolyClickable;
        m_pPolyClickable->AddRef();
    }
    if (pOldPoly)
        pOldPoly->Release();
    IMBAnim *pOldAnim = m_pAnim;
    m_pAnim = 0;
    if (refObjToCopy.m_pAnim)
        refObjToCopy.m_pAnim->Clone(&m_pAnim);
    if (pOldAnim)
        pOldAnim->Release();
    m_pGameBoard = refObjToCopy.m_pGameBoard;
    m_objProperties = refObjToCopy.m_objProperties;
    m_bHasMoved = true;
    m_bStateLocked = refObjToCopy.m_bStateLocked;
    m_pActor = refObjToCopy.m_pActor;
    Place();
    return *this;
}

// Template and inline instances this unit also emits:

// EQUIV 0x10030a90 ?SetOrigin@CLinkingLogsTile@@QAEXABVMBPoint3@@@Z: reviewed: same stores, the original keeps the
// height in esi and top in edi (size and per-axis forms tried)
void CLinkingLogsTile::SetOrigin(const MBPoint3 &ptNewOrigin)
{
    MBRect &rect = m_objProperties.m_recTile;
    long nHeight = rect.Height();
    rect.top = ptNewOrigin.y;
    rect.bottom = rect.top + nHeight;
    long nWidth = rect.Width();
    rect.left = ptNewOrigin.x;
    rect.right = rect.left + nWidth;
    m_bHasMoved = true;
}

// 0x10030ae0 ?GetClickableInterface@CLinkingLogsTile@@QAEPAVIMBUnknown@@XZ
IMBUnknown *CLinkingLogsTile::GetClickableInterface()
{
    IMBUnknown *iUnknown = 0;
    m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    return iUnknown;
}

// 0x10030b10 ?SetCurrentSequence@CLinkingLogsTile@@IAE_NJ_N@Z
bool CLinkingLogsTile::SetCurrentSequence(long nSequenceOffset, bool bForce)
{
    // the offsets count from the root sequence of the tile type
    bool bRet = true;
    if (m_pAnim) {
        long nSequence = m_mapSequenceRoot[GetType(*this)] + nSequenceOffset;
        if (bForce || (m_pAnim->IsStalled() && nSequence != m_pAnim->GetCurrentSequenceIndex())) {
            bRet = m_pAnim->SetCurrentSequence(nSequence);
            m_bHasMoved = true;
        }
    }
    return bRet;
}

// 0x10030be0 ?AddToMessageMap@CLinkingLogsTile@@QAEXAAV?$MBMap@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@@@@Z
void CLinkingLogsTile::AddToMessageMap(iunknownmessagemap &refMessageMap)
{
    // clicks arrive with the hot spot, animation events with the animation
    IMBUnknown *iUnknown = 0;
    m_pPolyClickable->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    refMessageMap.erase(iUnknown);
    refMessageMap.insert(iunknownmessagemap::value_type(iUnknown, this));
    if (iUnknown) {
        iUnknown->Release();
        iUnknown = 0;
    }
    refMessageMap.erase((IMBUnknown *)m_pAnim);
    refMessageMap.insert(iunknownmessagemap::value_type((IMBUnknown *)m_pAnim, this));
}

// 0x10030d60 ?InitPolyInterface@CLinkingLogsTile@@AAEXXZ
void CLinkingLogsTile::InitPolyInterface()
{
    IMBUnknown *iUnknown;
    MBComCreateInstance(0, MBIID_CMBScreenPoly, &iUnknown);
    iUnknown->QueryInterface(MBIID_IMBScreenPoly, (void **)&m_pPolyClickable);
    if (iUnknown)
        iUnknown->Release();
}

// 0x10030db0 ?Place@CLinkingLogsTile@@QAEXXZ
void CLinkingLogsTile::Place()
{
    if (m_bHasMoved) {
        m_pPolyClickable->SetRect(MBRect(m_objProperties.m_recTile), 5);
        if (m_pAnim) {
            // the animation is centred on the tile, layered by the tile type
            MBPoint3 ptOrigin = GetCenter() - MBPoint3(m_rectAnim.Width() / 2, m_rectAnim.Height() / 2, 150);
            ptOrigin.z = GetZOffset() + 155;
            m_pAnim->Place(ptOrigin);
        }
        m_bHasMoved = false;
    }
}

// 0x10030330 ??4tileinfo@CLinkingLogsTile@@QAEABU01@ABU01@@Z

// Template and inline instances this unit also emits:
// 0x1002fe30 ??1?$MBMap@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@@@QAE@XZ
// 0x1002fed0 ??0?$MBMap@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@@@QAE@XZ
// 0x100304f0 ??_GCLinkingLogsTile@@UAEPAXI@Z
// 0x10030530 ??1tileinfo@CLinkingLogsTile@@QAE@XZ
// 0x10030ed0 ?insert@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@_N@2@ABU?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@2@@Z
// 0x10030f10 ?erase@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x10031090 ??0?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@QAE@ABW4tiletype@CLinkingLogsTile@@ABW4anim_sequence@3@@Z
// 0x100310c0 ?insert@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@_N@2@ABU?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@2@@Z
// 0x10031240 ?insert@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@_N@2@ABU?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@2@@Z
// 0x100313c0 ?erase@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10031a00 ?_Erase@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10031c40 ?_Insert@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@2@@Z
// 0x10031fd0 ?_Insert@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@2@@Z
// 0x100329a0 ?lower_bound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@ABQAVIMBUnknown@@@Z
// 0x100329d0 ?upper_bound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAE?AViterator@12@ABQAVIMBUnknown@@@Z
// 0x10032ac0 ?_Dec@const_iterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@QAEXXZ
// 0x10032bb0 ?_Dec@const_iterator@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAEXXZ
// 0x10032ca0 ?_Inc@const_iterator@?$_Tree@W4tiletype@CLinkingLogsTile@@U?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@std@@U_Kfn@?$map@W4tiletype@CLinkingLogsTile@@W4anim_sequence@2@U?$less@W4tiletype@CLinkingLogsTile@@@std@@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@4@U?$less@W4tiletype@CLinkingLogsTile@@@4@V?$MBAllocator@W4anim_sequence@CLinkingLogsTile@@@@@std@@QAEXXZ
// 0x10032df0 ?_Ubound@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@std@@IBEPAU_Node@12@ABQAVIMBUnknown@@@Z
// 0x10032f40 ?_Distance@std@@YAXViterator@?$_Tree@PAVIMBUnknown@@U?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@std@@U_Kfn@?$map@PAVIMBUnknown@@PAVCLinkingLogsTile@@U?$less@PAVIMBUnknown@@@std@@V?$MBAllocator@PAVCLinkingLogsTile@@@@@3@U?$less@PAVIMBUnknown@@@3@V?$MBAllocator@PAVCLinkingLogsTile@@@@@1@0AAI@Z
// 0x10032f80 ?_Construct@std@@YAXPAU?$pair@QAVIMBUnknown@@PAVCLinkingLogsTile@@@1@ABU21@@Z
// 0x10032fb0 ?_Construct@std@@YAXPAU?$pair@$$CBW4tiletype@CLinkingLogsTile@@W4anim_sequence@2@@1@ABU21@@Z
// 0x10033060 ??0MBRect@@QAE@JJJJ@Z
// 0x10033090 ??4MBRect@@QAEAAV0@ABV0@@Z

// 0x1002f760 _$E22
// 0x1002f780 _$E19
// 0x1002fd50 _$E21
// 0x1002fd70 _$E20
// 0x1002fda0 _$E60
// 0x1002fdc0 _$E56
// 0x1002fde0 _$E59
// 0x1002fe00 _$E57
// 0x10032fe0 _$E73
// 0x10033020 _$E69
// 0x10033040 _$E72
