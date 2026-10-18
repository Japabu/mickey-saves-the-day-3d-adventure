#include "CSBInventory.h"
#include "SteamboatNode.h"
#include "MBRuntime.h"
#include "SBInventoryItem.h"
#include "SBRecycleItem.h"
#include "MBColor.h"
#include <string.h>

// 0x1006a240 ?AddRef@CSBInventory@@UAEKXZ
unsigned long CSBInventory::AddRef()
{
    return ++_mRefCnt;
}

// 0x1006a260 ?Release@CSBInventory@@UAEKXZ
unsigned long CSBInventory::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1006ae60 ?RemoveAllItems@CSBInventory@@UAEXXZ
void CSBInventory::RemoveAllItems()
{
    ClearInventory();
}

// 0x1006aef0 ?GetRecycleItemCount@CSBInventory@@UAEHXZ
int CSBInventory::GetRecycleItemCount()
{
    return m_listRecycleItemNames.size();
}

// The original reports a failing step by throwing its source line, so the functions using
// THROW_ON_ERROR sit on the lines they had there; the gaps held code not reconstructed yet.
#define THROW_ON_ERROR(expr) if ((expr) != 0) throw (long)__LINE__


// 0x10069a20 ?MakeCSBInventory@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBInventory()
{
    static IMBUnknown *instance = MBComUtilAddSingleton((ISBInventory *)new CSBInventory);
    if (instance)
        instance->AddRef();
    return instance;
}

// the GUI animations and where they are placed
const MBString CSBInventory::m_strAnimNames[4] = {
    "GDInventoryGUIRecycleBin", "GDInventoryGUIMap", "GDInventoryGUIPants", "GDInventoryGUI"
};
const MBPoint3 CSBInventory::m_arrayAnimPositions[4] = {
    MBPoint3(0, 0, 200), MBPoint3(0, 0, 200), MBPoint3(0, 0, 200), MBPoint3(0, 0, 190)
};
static const MBPoint3 kGUIBarPosition(0, 0, 190);

// 0x10069e00 ??0CSBInventory@@QAE@XZ
CSBInventory::CSBInventory()
    : m_eCurrentHighlightAnim(noanim), m_eCurrentClickedAnim(noanim), m_eCurrentHighlightState(highlight),
      m_eBarState(hidden), m_pParent(0), m_bActive(false), m_bPixelTest(false), m_bIsTrackingClick(false),
      m_bIsEnabled(true)
{
    _mRefCnt = 0;
    memset(m_pInventoryGUIAnims, 0, sizeof(m_pInventoryGUIAnims));
    m_vecItemsInInventory.resize(10);
    m_timerGUIHide.SetBroadcastChannel(600);
    m_timerGUIHide.SetBroadcastEvent(609);
}

// 0x1006a030 ?QueryInterface@CSBInventory@@UAEJABUMBGUID@@PAPAX@Z
long CSBInventory::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (ISBInventory *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_ISBInventory)
        p = (ISBInventory *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1006a400 ??1CSBInventory@@MAE@XZ
CSBInventory::~CSBInventory()
{
    HideAndDestroyInventoryGUI();
    HideAndDestroyInventoryItems();
    StopListening();
}

// 0x1006a540 ?Show@CSBInventory@@UAEX_N@Z
void CSBInventory::Show(bool bShow)
{
    try {
        if (bShow) {
            CreateAndShowInventoryGUI();
            CreateAndShowInventoryItems();
            StartListening();
        } else {
            HideAndDestroyInventoryGUI();
            HideAndDestroyInventoryItems();
            StopListening();
        }
        m_bActive = bShow;
    } catch (long nLine) {
        MBDebugStr("%s(%d) : ERROR! Failed to show Inventory GUI \n", "D:\\tugboat\\code\\steamboat\\SBInventory.cpp", nLine);
        m_bActive = false;
    }
}

// TODO 0x1006a610 ?GetBounds@CSBInventory@@UBE?AVMBRect@@XZ: (84% similar) loop layout and register choice differ (the original places the search loop's back edge last and counts the second loop down in ebx)
MBRect CSBInventory::GetBounds() const
{
    MBRect rectBounds;
    if (IsActive()) {
        int nAnim = bar;
        do {
            if (m_pInventoryGUIAnims[nAnim] && m_pInventoryGUIAnims[nAnim]->IsVisible())
                break;
        } while (nAnim--);
        if (nAnim != noanim) {
            rectBounds = m_pInventoryGUIAnims[firstanim]->GetBounds();
            while (nAnim != noanim && nAnim--) {
                if (m_pInventoryGUIAnims[nAnim] && m_pInventoryGUIAnims[nAnim]->IsVisible()) {
                    MBRect rect = m_pInventoryGUIAnims[nAnim]->GetBounds();
                    if (rectBounds.left > rect.left) rectBounds.left = rect.left;
                    if (rectBounds.top > rect.top) rectBounds.top = rect.top;
                    if (rectBounds.right < rect.right) rectBounds.right = rect.right;
                    if (rectBounds.bottom < rect.bottom) rectBounds.bottom = rect.bottom;
                }
            }
        }
    }
    return rectBounds;
}

// EQUIV 0x1006a770 ?AddItem@CSBInventory@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N@Z: reviewed: MBComPtr inlining: the original calls gs's CreateObject out of line, VC6 inlines it
void CSBInventory::AddItem(const MBString &strInstanceName, bool bPopUp)
{
    MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
    SBItem *pItem = gs->GetItem(strInstanceName);
    MBString strClassName = strInstanceName.substr(0, strInstanceName.find_last_of("0123456789"));
    if (pItem) {
        gs->SetProperty((strClassName + "InInventory").c_str(), 1);
        pItem->SetInScene(false);
        pItem->SetOnCharacter(true);
        if (dynamic_cast<SBInventoryItem *>(pItem)) {
            int nSlot = AddInventoryItem(strInstanceName, bPopUp);
            if (nSlot != -1 && IsActive() && m_eBarState == visible)
                CreateAndShowInventoryItem(nSlot);
        } else if (dynamic_cast<SBRecycleItem *>(pItem))
            AddRecycleItem(strInstanceName, bPopUp);
        if (bPopUp) {
            // pop the bar up for a while
            if (m_eBarState == hiding)
                NextState();
            else if (m_eBarState == hidden)
                SetGUIAnimSequence(bar, NextState());
            if (!m_timerGUIHide.IsArmed())
                m_timerGUIHide.Arm(2000);
        }
    }
}

// 0x1006aa20 ?AddInventoryItem@CSBInventory@@IAEHABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N@Z
int CSBInventory::AddInventoryItem(const MBString &strInstanceName, bool bPopUp)
{
    int nSlot = 0;
    try {
        for (nSlot = 0; nSlot < 10; nSlot++) {
            if (m_vecItemsInInventory[nSlot].strInstanceName.size() == 0) {
                m_vecItemsInInventory[nSlot].strInstanceName = strInstanceName;
                break;
            }
        }
    } catch (long nLine) {
        MBDebugStr("%s(%d) : ERROR! Failed to show Inventory GUI \n", "D:\\tugboat\\code\\steamboat\\SBInventory.cpp", nLine);
        m_bActive = false;
    }
    return nSlot;
}

// 0x1006ac50 ?RemoveItem@CSBInventory@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CSBInventory::RemoveItem(const MBString &strInstanceName)
{
    for (int nSlot = 0; nSlot < 10; nSlot++) {
        if (strInstanceName == m_vecItemsInInventory[nSlot].strInstanceName) {
            HideAndDestroyInventoryItem(nSlot);
            m_vecItemsInInventory[nSlot].strInstanceName.erase();
            return;
        }
    }
}

// 0x1006ae80 ?AddRecycleItem@CSBInventory@@IAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@_N@Z
void CSBInventory::AddRecycleItem(const MBString &strInstanceName, bool bPopUp)
{
    m_listRecycleItemNames.push_back(strInstanceName);
}

// 0x1006af10 ?ClearInventory@CSBInventory@@IAEXXZ
void CSBInventory::ClearInventory()
{
    HideAndDestroyInventoryItems();
    for (int nSlot = 0; nSlot < 10; nSlot++)
        m_vecItemsInInventory[nSlot].strInstanceName.erase();
    m_listRecycleItemNames.clear();
}








































































































// 0x1006b080 ?CreateInventoryAnim@CSBInventory@@AAEXW4gui_anims@1@J@Z
void CSBInventory::CreateInventoryAnim(gui_anims eAnim, long nState)
{
    if (!m_pInventoryGUIAnims[eAnim]) {
        THROW_ON_ERROR(SteamboatNode::AddAnimToWorld(m_strAnimNames[eAnim].c_str(), m_pInventoryGUIAnims[eAnim]));
        m_pInventoryGUIAnims[eAnim]->Place(m_arrayAnimPositions[eAnim]);
        SetGUIAnimSequence(eAnim, nState);
        m_pInventoryGUIAnims[eAnim]->Play();
        m_pInventoryGUIAnims[eAnim]->Hide();
    }
}








// 0x1006b250 ?DestroyInventoryAnim@CSBInventory@@AAEXW4gui_anims@1@@Z
void CSBInventory::DestroyInventoryAnim(gui_anims eAnim)
{
    IMBAnim *pAnim = m_pInventoryGUIAnims[eAnim];
    if (pAnim) {
        IMBUnknown *pUnknown;
        THROW_ON_ERROR(pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&pUnknown));
        SteamboatNode::RemoveFromWorld(pUnknown);
        if (pUnknown) {
            pUnknown->Release();
            pUnknown = 0;
        }
        pAnim->Release();
        m_pInventoryGUIAnims[eAnim] = 0;
    }
}

// 0x1006b2f0 ?CreateAndShowInventoryGUI@CSBInventory@@AAEXXZ
void CSBInventory::CreateAndShowInventoryGUI()
{
    CreateInventoryAnim(bar, 0);
    CreateInventoryAnim(recyclebin, 1);
    CreateInventoryAnim(map, 1);
    CreateInventoryAnim(pants, 1);
    m_pInventoryGUIAnims[bar]->Show();
}

// 0x1006b340 ?HideAndDestroyInventoryGUI@CSBInventory@@AAEXXZ
void CSBInventory::HideAndDestroyInventoryGUI()
{
    m_eBarState = hidden;
    DestroyInventoryAnim(bar);
    DestroyInventoryAnim(recyclebin);
    DestroyInventoryAnim(map);
    DestroyInventoryAnim(pants);
}

// 0x1006b380 ?CreateAndShowInventoryItems@CSBInventory@@AAEXXZ
void CSBInventory::CreateAndShowInventoryItems()
{
    for (int i = 0; i < 10; i++)
        CreateAndShowInventoryItem(i);
}

// 0x1006b3b0 ?HideAndDestroyInventoryItems@CSBInventory@@AAEXXZ
void CSBInventory::HideAndDestroyInventoryItems()
{
    for (int i = 0; i < 10; i++)
        HideAndDestroyInventoryItem(i);
}




























// EQUIV 0x1006b3e0 ?CreateAndShowInventoryItem@CSBInventory@@AAEXH@Z: reviewed: matched until the headers of this
// round changed (not MBComPtr.h or MBPoint.h); now nSlot and nGUISeque sit in swapped registers
void CSBInventory::CreateAndShowInventoryItem(int nSlot)
{
    int nGUIFrame = m_pInventoryGUIAnims[bar]->GetCurrentFrameIndex();
    int nGUISeque = m_pInventoryGUIAnims[bar]->GetCurrentSequenceIndex();
    if (!m_vecItemsInInventory[nSlot].pAnim && m_vecItemsInInventory[nSlot].strInstanceName.length()) {
        // the item's animation is named after the instance without its last character
        MBString strInstanceName = m_vecItemsInInventory[nSlot].strInstanceName;
        MBString strAnimName = strInstanceName.substr(0, strInstanceName.length() - 1);
        THROW_ON_ERROR(SteamboatNode::AddAnimToWorld(strAnimName.c_str(), m_vecItemsInInventory[nSlot].pAnim));
    }
    if (m_vecItemsInInventory[nSlot].pAnim) {
        m_vecItemsInInventory[nSlot].pAnim->Play();
        m_vecItemsInInventory[nSlot].pAnim->SetCurrentSequence(0);
        if (m_eBarState == visible)
            UpdateInventoryItem(nSlot, nGUISeque, 12);
        else
            UpdateInventoryItem(nSlot, nGUIFrame, nGUISeque);
        m_vecItemsInInventory[nSlot].pAnim->Show();
    }
}












// 0x1006b650 ?HideAndDestroyInventoryItem@CSBInventory@@AAEXH@Z
void CSBInventory::HideAndDestroyInventoryItem(int nSlot)
{
    if (m_vecItemsInInventory[nSlot].pAnim) {
        IMBUnknown *pUnknown;
        THROW_ON_ERROR(m_vecItemsInInventory[nSlot].pAnim->QueryInterface(MBIID_IMBUnknown, (void **)&pUnknown));
        SteamboatNode::RemoveFromWorld(pUnknown);
        if (pUnknown) {
            pUnknown->Release();
            pUnknown = 0;
        }
        if (m_vecItemsInInventory[nSlot].pAnim) {
            m_vecItemsInInventory[nSlot].pAnim->Release();
            m_vecItemsInInventory[nSlot].pAnim = 0;
        }
    }
}

// Where an item sits in each frame of the bar's animations: the first four frames are those of the
// hiding (and visible) bar, the other nine those of the bar popping up.
static const MBPointStruct kItemPositions[10][13] = {
    { { 98, 435 }, { 98, 425 }, { 98, 465 }, { 98, -100 }, { 98, -100 }, { 98, -100 }, { 98, -100 }, { 98, -100 }, { 98, 455 }, { 98, 425 }, { 98, 440 }, { 98, 430 }, { 98, 435 } },
    { { 151, 435 }, { 151, 425 }, { 151, 465 }, { 151, -100 }, { 151, -100 }, { 151, -100 }, { 151, -100 }, { 151, 455 }, { 151, 425 }, { 151, 440 }, { 151, 430 }, { 151, 435 }, { 151, 435 } },
    { { 204, 435 }, { 204, 425 }, { 204, 465 }, { 204, -100 }, { 204, -100 }, { 204, -100 }, { 204, 455 }, { 204, 425 }, { 204, 440 }, { 204, 430 }, { 204, 435 }, { 204, 435 }, { 204, 435 } },
    { { 258, 435 }, { 258, 425 }, { 258, 465 }, { 258, -100 }, { 258, -100 }, { 258, -100 }, { 258, -100 }, { 258, 455 }, { 258, 425 }, { 258, 440 }, { 258, 430 }, { 258, 435 }, { 258, 435 } },
    { { 312, 435 }, { 312, 425 }, { 312, 465 }, { 312, -100 }, { 312, -100 }, { 312, -100 }, { 312, 455 }, { 312, 425 }, { 312, 440 }, { 312, 430 }, { 312, 435 }, { 312, 435 }, { 312, 435 } },
    { { 365, 435 }, { 365, 425 }, { 365, 465 }, { 365, -100 }, { 365, -100 }, { 365, -100 }, { 365, -100 }, { 365, -100 }, { 365, 455 }, { 365, 425 }, { 365, 440 }, { 365, 430 }, { 365, 435 } },
    { { 420, 435 }, { 420, 420 }, { 420, 465 }, { 420, -100 }, { 420, -100 }, { 420, -100 }, { 420, -100 }, { 420, 455 }, { 420, 420 }, { 420, 440 }, { 420, 430 }, { 420, 435 }, { 420, 435 } },
    { { 472, 435 }, { 472, 425 }, { 472, 465 }, { 472, -100 }, { 472, -100 }, { 472, -100 }, { 472, 455 }, { 472, 425 }, { 472, 440 }, { 472, 430 }, { 472, 435 }, { 472, 435 }, { 472, 435 } },
    { { 528, 435 }, { 528, 425 }, { 528, 465 }, { 528, -100 }, { 528, -100 }, { 528, -100 }, { 528, -100 }, { 528, 455 }, { 528, 425 }, { 528, 440 }, { 528, 430 }, { 528, 435 }, { 528, 435 } },
    { { 581, 435 }, { 581, 425 }, { 581, 465 }, { 581, -100 }, { 581, -100 }, { 581, -100 }, { 581, 455 }, { 581, 425 }, { 581, 440 }, { 581, 430 }, { 581, 435 }, { 581, 435 }, { 581, 435 } },
};

// EQUIV 0x1006b6f0 ?UpdateInventoryItem@CSBInventory@@IAEXHHH@Z: reviewed: the original keeps the z of 210 in esi before storing it (member-wise and constructor forms tried)
void CSBInventory::UpdateInventoryItem(int nSlot, int nGUISeque, int nGUIFrame)
{
    if (m_vecItemsInInventory[nSlot].pAnim) {
        switch (nGUISeque) {
        case hidden:
            m_vecItemsInInventory[nSlot].pAnim->Place(MBPoint3(-100, -100, 0));
            break;
        case showing: {
            MBPoint3 ptPosition(kItemPositions[nSlot][nGUIFrame + 4].x, kItemPositions[nSlot][nGUIFrame + 4].y, 210);
            m_vecItemsInInventory[nSlot].pAnim->Place(ptPosition);
            break;
        }
        case visible:
        case hiding: {
            MBPoint3 ptPosition(kItemPositions[nSlot][nGUIFrame].x, kItemPositions[nSlot][nGUIFrame].y, 210);
            m_vecItemsInInventory[nSlot].pAnim->Place(ptPosition);
            break;
        }
        }
    }
}

// 0x1006b820 ?StartListening@CSBInventory@@IAEXXZ
void CSBInventory::StartListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->AddListener(-2, this);
    broadcaster->AddListener(600, this);
    IMBUnknown *pUnknown;
    QueryInterface(MBIID_IMBUnknown, (void **)&pUnknown);
    SteamboatNode::AddToWorld(pUnknown);
    if (pUnknown) {
        pUnknown->Release();
        pUnknown = 0;
    }
}

// 0x1006b950 ?StopListening@CSBInventory@@IAEXXZ
void CSBInventory::StopListening()
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-2, this);
        broadcaster->RemoveListener(600, this);
        IMBUnknown *pUnknown;
        QueryInterface(MBIID_IMBUnknown, (void **)&pUnknown);
        SteamboatNode::RemoveFromWorld(pUnknown);
        if (pUnknown) {
            pUnknown->Release();
            pUnknown = 0;
        }
    }
}

// 0x1006ba80 ?ListenToEvent@CSBInventory@@MAEXJJPAX@Z
void CSBInventory::ListenToEvent(long channel, long event, void *data)
{
    switch (channel) {
    case -2: {
        // animation events of the bar
        MBAnimEventStruct *pEvent = (MBAnimEventStruct *)data;
        switch (event) {
        case 2: {
            for (int nSlot = 0; nSlot < 10; nSlot++)
                UpdateInventoryItem(nSlot, pEvent->sequence, pEvent->frame);
            break;
        }
        case 5:
            switch (m_eBarState) {
            case hidden:
            case showing:
            case hiding:
                if (m_pInventoryGUIAnims[recyclebin]->IsVisible())
                    m_pInventoryGUIAnims[recyclebin]->Hide();
                if (m_pInventoryGUIAnims[map]->IsVisible())
                    m_pInventoryGUIAnims[map]->Hide();
                if (m_pInventoryGUIAnims[pants]->IsVisible())
                    m_pInventoryGUIAnims[pants]->Hide();
                break;
            case visible:
                m_pInventoryGUIAnims[recyclebin]->Show();
                m_pInventoryGUIAnims[map]->Show();
                m_pInventoryGUIAnims[pants]->Show();
                break;
            }
            break;
        case 6:
            if (m_pInventoryGUIAnims[bar] == pEvent->anim) {
                if (pEvent->sequence == showing || pEvent->sequence == hiding)
                    SetGUIAnimSequence(bar, NextState());
                else if (pEvent->sequence == visible)
                    CreateAndShowInventoryItems();
            }
            break;
        }
    }
    case 600:
        // the hide timer
        if (event == 609 && m_eBarState == visible)
            SetGUIAnimSequence(bar, NextState());
        break;
    }
}

// 0x1006bc30 ?NextState@CSBInventory@@AAE?AW4gui_state@1@XZ
CSBInventory::gui_state CSBInventory::NextState()
{
    gui_state eNewState = (gui_state)((m_eBarState + 1) % (lastguistate + 1));
    MBDebugStr("Inventory::NextState (state change: %d --> %d)\n", m_eBarState, eNewState);
    m_eBarState = eNewState;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->BroadcastEvent(600, 610, &m_eBarState);
    return m_eBarState;
}

// 0x1006bd50 ?PopUpMenu@CSBInventory@@IAEXXZ
void CSBInventory::PopUpMenu()
{
}

// 0x1006bd70 ?SetGUIAnimSequence@CSBInventory@@AAEXW4gui_anims@1@H@Z
void CSBInventory::SetGUIAnimSequence(gui_anims eAnim, int nState)
{
    // the recycle bin always shows how many recycle items are collected
    if (eAnim != recyclebin) {
        if (m_pInventoryGUIAnims[recyclebin])
            m_pInventoryGUIAnims[recyclebin]->SetCurrentSequence(m_listRecycleItemNames.size());
        m_pInventoryGUIAnims[eAnim]->SetCurrentSequence(nState);
    } else {
        m_pInventoryGUIAnims[recyclebin]->SetCurrentSequence(m_listRecycleItemNames.size());
    }
}

// 0x1006bde0 ?Load@CSBInventory@@UAEXPAVIMBStream@@PAVIMBPref@@PAVMfAiActor@@@Z
void CSBInventory::Load(IMBStream *pPrefStream, IMBPref *pPref, MfAiActor *pMfActor)
{
    MBString strItemName;
    ClearInventory();
    char pszKey[32];
    int nIndex;
    for (nIndex = 0; nIndex < 10; nIndex++) {
        sprintf(pszKey, "inventory item %d", nIndex);
        bool bFound = pPref->GetString(pPrefStream, MBString("inventory"), MBString(pszKey), strItemName, MBString("")) > 0;
        if (bFound)
            AddItem(strItemName, false);
    }
    nIndex = 0;
    sprintf(pszKey, "recycle item %d", nIndex++);
    for (;;) {
        bool bFound = pPref->GetString(pPrefStream, MBString("inventory"), MBString(pszKey), strItemName, MBString("")) > 0;
        if (!bFound)
            break;
        AddItem(strItemName, false);
        sprintf(pszKey, "recycle item %d", nIndex++);
    }
}

// 0x1006c300 ?Save@CSBInventory@@UAEXPAVIMBStream@@PAVIMBPref@@_N@Z
void CSBInventory::Save(IMBStream *pPrefStream, IMBPref *pPref, bool bExit)
{
    char pszKey[32];
    int nIndex;
    for (nIndex = 0; nIndex < 10; nIndex++) {
        sprintf(pszKey, "inventory item %d", nIndex);
        pPref->SetString(pPrefStream, MBString("inventory"), MBString(pszKey), m_vecItemsInInventory[nIndex].strInstanceName);
    }
    nIndex = 0;
    for (recycle_item_list::iterator it = m_listRecycleItemNames.begin(); it != m_listRecycleItemNames.end(); it++, nIndex++) {
        sprintf(pszKey, "recycle item %d", nIndex);
        pPref->SetString(pPrefStream, MBString("inventory"), MBString(pszKey), *it);
    }
}

// EQUIV 0x1006c6b0 ?IsPointOnAnim@CSBInventory@@IBE_NABVMBPoint@@PBVIMBAnim@@@Z: reviewed: the original stores a false bool on the stack first and keeps the comparison result in a second stack byte
bool CSBInventory::IsPointOnAnim(const MBPoint &ptTestPoint, const IMBAnim *pTestAnim) const
{
    // on the anim's rectangle and not on a transparent pixel
    bool bOnAnim = false;
    IMBAnim *pAnim = (IMBAnim *)pTestAnim;
    MBRect rectBounds = pAnim->GetBounds();
    if (rectBounds.right > 0)
        rectBounds.right--;
    if (rectBounds.bottom > 0)
        rectBounds.bottom--;
    if (pAnim->IsVisible() && rectBounds.ContainsPoint(ptTestPoint)) {
        IMBSurface *pSurface;
        long nCastID = pAnim->GetCastID(pAnim->GetCurrentSequenceIndex(), pAnim->GetCurrentFrameIndex());
        pAnim->GetCastMemberSurface(nCastID, &pSurface);
        if (pSurface) {
            MBColor colorPixel = pSurface->GetPixelColor(ptTestPoint.x - rectBounds.left, ptTestPoint.y - rectBounds.top);
            MBColor colorTransparent = pSurface->GetTransparentColor();
            bOnAnim = colorPixel != colorTransparent;
            pSurface->Release();
        }
    }
    return bOnAnim;
}

// 0x1006c860 ?GetAnimFromPoint@CSBInventory@@IBE?AW4gui_anims@1@ABVMBPoint@@@Z
CSBInventory::gui_anims CSBInventory::GetAnimFromPoint(const MBPoint &ptTestPoint) const
{
    gui_anims eAnim = noanim;
    if (IsActive()) {
        int i;
        for (i = 0; i < 10 && eAnim == noanim; i++) {
            if (m_vecItemsInInventory[i].pAnim && IsPointOnAnim(ptTestPoint, m_vecItemsInInventory[i].pAnim))
                eAnim = (gui_anims)(item1 + i);
        }
        for (i = 0; i < numguianims && eAnim == noanim; i++) {
            if (IsPointOnAnim(ptTestPoint, m_pInventoryGUIAnims[i]))
                eAnim = (gui_anims)i;
        }
    }
    return eAnim;
}

// 0x1006c900 ?GetIDFromPoint@CSBInventory@@IBE?AW4gui_anims@1@ABVMBPoint@@@Z
CSBInventory::gui_anims CSBInventory::GetIDFromPoint(const MBPoint &ptTestPoint) const
{
    gui_anims eAnim = GetAnimFromPoint(ptTestPoint);
    if (eAnim == bar)
        return noanim;
    return eAnim;
}

// 0x1006c930 ?IsPointOnMenu@CSBInventory@@UBE_NABVMBPoint@@@Z
bool CSBInventory::IsPointOnMenu(const MBPoint &ptTestPoint) const
{
    return IsPointOnGUI(ptTestPoint) || IsPointOnItem(ptTestPoint);
}

// EQUIV 0x1006c970 ?IsPointOnGUI@CSBInventory@@IBE_NABVMBPoint@@@Z: reviewed: the original keeps a false bool in bl for
// the inactive case and the point in ebx; the forms tried fold or move it
bool CSBInventory::IsPointOnGUI(const MBPoint &ptTestPoint) const
{
    bool bOnGUI = false;
    if (IsActive()) {
        for (int i = firstanim; i < numguianims; i++) {
            bOnGUI = IsPointOnAnim(ptTestPoint, m_pInventoryGUIAnims[i]);
            if (bOnGUI)
                return true;
        }
    }
    return bOnGUI;
}

// EQUIV 0x1006c9e0 ?IsPointOnItem@CSBInventory@@IBE_NABVMBPoint@@@Z: reviewed: as IsPointOnGUI
bool CSBInventory::IsPointOnItem(const MBPoint &ptTestPoint) const
{
    bool bOnItem = false;
    if (IsActive()) {
        for (int i = 0; i < 10; i++) {
            IMBAnim *pAnim = m_vecItemsInInventory[i].pAnim;
            if (pAnim) {
                bOnItem = IsPointOnAnim(ptTestPoint, pAnim);
                if (bOnItem)
                    return true;
            }
        }
    }
    return bOnItem;
}

// 0x1006ca50 ?StartTrackingClick@CSBInventory@@MAEXABVMBPoint@@F@Z
void CSBInventory::StartTrackingClick(const MBPoint &worldMousePosition, short mouseButton)
{
    if (IsActive()) {
        if (m_eBarState == visible) {
            gui_anims eAnim = GetIDFromPoint(worldMousePosition);
            if (m_eCurrentClickedAnim != noanim && m_pInventoryGUIAnims[eAnim] != m_pInventoryGUIAnims[m_eCurrentClickedAnim]) {
                SetGUIAnimSequence(m_eCurrentClickedAnim, idle);
                m_eCurrentClickedAnim = noanim;
            }
            if (eAnim != noanim && eAnim < numguianims) {
                m_eCurrentHighlightState = down;
                SetGUIAnimSequence(eAnim, down);
                m_eCurrentClickedAnim = eAnim;
            } else if (eAnim > numguianims)
                m_eCurrentClickedAnim = eAnim;
        }
        m_bIsTrackingClick = true;
    }
}

// EQUIV 0x1006cb10 ?StopTrackingClick@CSBInventory@@MAEXABVMBPoint@@F@Z: reviewed: same statements; the original computes the object pointer (this - 4) up front, spills it and keeps the point in ebp
void CSBInventory::StopTrackingClick(const MBPoint &worldMousePosition, short mouseBbutton)
{
    m_bIsTrackingClick = false;
    m_eCurrentHighlightState = highlight;
    m_eCurrentClickedAnim = noanim;
    if (IsActive() && m_eBarState != hiding && m_eBarState != showing) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        gui_anims eAnim = GetAnimFromPoint(worldMousePosition);
        WithinRollover(worldMousePosition);
        switch (eAnim) {
        case recyclebin:
            broadcaster->BroadcastEvent(600, 605, 0);
            break;
        case map: {
            // the map works once the three parts of the boat are back
            MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
            if (gs->GetProperty("PRbathtubRecovered", 0) && gs->GetProperty("PRbicycleRecovered", 0) &&
                gs->GetProperty("PRpropellerRecovered", 0)) {
                MBComPtr<ISBGameState> gameState(MBIID_CSBGameState);
                if (!gameState->GetProperty("HCMickeyCaptive", 0))
                    SteamboatNode::GetActiveNode()->StartSound("gd02xmc0010");
                else
                    SteamboatNode::GetActiveNode()->StartSound("gd02xmi0010");
            } else
                broadcaster->BroadcastEvent(600, 604, 0);
            break;
        }
        case pants:
            broadcaster->BroadcastEvent(600, 606, 0);
            break;
        case bar:
            SetGUIAnimSequence(bar, NextState());
            break;
        case item1:
        case item2:
        case item3:
        case item4:
        case item5:
        case item6:
        case item7:
        case item8:
        case item9:
        case item10: {
            // an item can be used where it belongs
            MBComPtr<ISBGameState> gs(MBIID_CSBGameState);
            SBItem *pItem = gs->GetItem(m_vecItemsInInventory[eAnim - item1].strInstanceName);
            bool bActive = pItem->IsItemActiveInSpace(SteamboatNode::GetSpaceCode());
            if (bActive) {
                SetGUIAnimSequence(bar, NextState());
                broadcaster->BroadcastEvent(600, 603, pItem);
            }
            break;
        }
        }
    }
}

// 0x1006cf20 ?EnterRollover@CSBInventory@@EAEXABVMBPoint@@@Z
void CSBInventory::EnterRollover(const MBPoint &worldMousePosition)
{
    WithinRollover(worldMousePosition);
    MBDebugStr("Inventory::EnterRollover  (current state = %d)\n", m_eBarState);
    if (m_eBarState == visible)
        SteamboatNode::GetActiveCursor()->SetCurrentSequence(10);
}

// 0x1006cf70 ?WithinRollover@CSBInventory@@MAEXABVMBPoint@@@Z
void CSBInventory::WithinRollover(const MBPoint &worldMousePosition)
{
    if (IsActive() && m_eBarState == visible) {
        gui_anims eAnim = GetIDFromPoint(worldMousePosition);
        if (m_eCurrentHighlightAnim != noanim && eAnim != m_eCurrentHighlightAnim) {
            if (m_eCurrentHighlightAnim < numguianims)
                SetGUIAnimSequence(m_eCurrentHighlightAnim, idle);
            m_eCurrentHighlightAnim = noanim;
        }
        if (eAnim != noanim && eAnim < numguianims) {
            if (m_eCurrentHighlightAnim != eAnim ||
                m_eCurrentHighlightState != m_pInventoryGUIAnims[eAnim]->GetCurrentSequenceIndex()) {
                if (m_eCurrentClickedAnim == noanim || eAnim == m_eCurrentClickedAnim) {
                    SetGUIAnimSequence(eAnim, m_eCurrentHighlightState);
                    m_eCurrentHighlightAnim = eAnim;
                }
            }
        }
    }
}

// 0x1006d030 ?LeaveRollover@CSBInventory@@MAEXXZ
void CSBInventory::LeaveRollover()
{
    SteamboatNode::GetActiveNode()->SetCursorSequence(0);
    if (m_eCurrentHighlightAnim != noanim) {
        SetGUIAnimSequence(m_eCurrentHighlightAnim, idle);
        m_eCurrentHighlightAnim = noanim;
    }
}

// 0x1006d070 ?IsRolledOver@CSBInventory@@MBE_NABVMBPoint@@@Z
bool CSBInventory::IsRolledOver(const MBPoint &worldMousePosition) const
{
    if (!m_bIsEnabled)
        return false;
    return IsPointOnMenu(worldMousePosition);
}

// 0x1006d0a0 ?IsClicked@CSBInventory@@MBE_NABVMBPoint@@F@Z
bool CSBInventory::IsClicked(const MBPoint &worldMousePosition, short mouseButton) const
{
    if (!m_bIsEnabled)
        return false;
    return IsPointOnMenu(worldMousePosition);
}

// 0x1006d0d0 ?IsTrackingClick@CSBInventory@@EBE_NXZ
bool CSBInventory::IsTrackingClick() const
{
    return m_bIsTrackingClick;
}

// 0x1006d0f0 ?IsActive@CSBInventory@@EBE_NXZ
bool CSBInventory::IsActive() const
{
    return m_bActive;
}

// 0x1006d110 ?Activate@CSBInventory@@EAEXXZ
void CSBInventory::Activate()
{
    Show(true);
}

// 0x1006d130 ?Deactivate@CSBInventory@@EAEXXZ
void CSBInventory::Deactivate()
{
    Show(false);
}

// 0x1006d150 ?SetPixelTest@CSBInventory@@EAEX_N@Z
void CSBInventory::SetPixelTest(bool set)
{
    m_bPixelTest = set;
}

// 0x1006d170 ?GetPixelTest@CSBInventory@@EAE_NXZ
bool CSBInventory::GetPixelTest()
{
    return m_bPixelTest;
}

// 0x1006d190 ?TrackClick@CSBInventory@@EAEXABVMBPoint@@@Z
void CSBInventory::TrackClick(const MBPoint &worldMousePosition)
{
}

// 0x1006d1b0 ?GetOffset@CSBInventory@@EBE?AVMBPoint3@@XZ
MBPoint3 CSBInventory::GetOffset() const
{
    return MBPoint3(0, 0, 0);
}

// 0x1006d1e0 ?GetPosition@CSBInventory@@EBE?AVMBPoint3@@XZ
MBPoint3 CSBInventory::GetPosition() const
{
    return m_arrayAnimPositions[bar];
}

// 0x1006d220 ?Place@CSBInventory@@EAEXABVMBPoint3@@@Z
void CSBInventory::Place(const MBPoint3 &point)
{
}

// 0x1006d240 ?Move@CSBInventory@@EAEXABVMBPoint3@@@Z
void CSBInventory::Move(const MBPoint3 &point)
{
}

// 0x1006d260 ?IsVisible@CSBInventory@@EBE_NXZ
bool CSBInventory::IsVisible() const
{
    return m_bActive;
}

// 0x1006d280 ?Show@CSBInventory@@EAEJXZ
long CSBInventory::Show()
{
    Show(true);
    return 0;
}

// 0x1006d2a0 ?Hide@CSBInventory@@EAEJXZ
long CSBInventory::Hide()
{
    Show(false);
    return 1;
}

// 0x1006d2d0 ?IsDirty@CSBInventory@@EBE_NXZ
bool CSBInventory::IsDirty() const
{
    return false;
}

// 0x1006d2f0 ?SetDirty@CSBInventory@@EAEXXZ
void CSBInventory::SetDirty()
{
}

// 0x1006d310 ?ClearDirty@CSBInventory@@EAEXXZ
void CSBInventory::ClearDirty()
{
}

// 0x1006d330 ?Draw@CSBInventory@@EBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CSBInventory::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    return 0;
}

// 0x1006d350 ?GetParent@CSBInventory@@EBEXPAPAVIMBObjectGroup@@@Z
void CSBInventory::GetParent(IMBObjectGroup **parent) const
{
    *parent = m_pParent;
}

// 0x1006d370 ?SetParent@CSBInventory@@EAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CSBInventory::SetParent(IMBObjectGroup *newParent)
{
    if (m_pParent)
        m_pParent->RemoveObject(this);
    return m_pParent = newParent;
}

// 0x1006d3b0 ?Enable@CSBInventory@@UAEX_N@Z
void CSBInventory::Enable(bool bEnable)
{
    if (bEnable)
        m_bIsEnabled = true;
    else
        m_bIsEnabled = false;
}

// Adjustor thunks (emitted with the vftables):
// 0x1006dbf0 ?QueryInterface@CSBInventory@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1006dc00 ?AddRef@CSBInventory@@W3AEKXZ
// 0x1006dc10 ?Release@CSBInventory@@W3AEKXZ
// 0x1006dc20 ?QueryInterface@CSBInventory@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1006dc30 ?AddRef@CSBInventory@@W7AEKXZ
// 0x1006dc40 ?Release@CSBInventory@@W7AEKXZ
// 0x1006dc50 ?GetBounds@CSBInventory@@W7BE?AVMBRect@@XZ
// 0x1006dc60 ?QueryInterface@CSBInventory@@WM@AEJABUMBGUID@@PAPAX@Z
// 0x1006dc70 ?AddRef@CSBInventory@@WM@AEKXZ
// 0x1006dc80 ?Release@CSBInventory@@WM@AEKXZ

// Template and inline instances this unit also emits:
// 0x1006a290 ??_GCSBInventory@@MAEPAXI@Z
// 0x1006a2d0 ??1?$MBVector@Uinventory_item@CSBInventory@@@@QAE@XZ
// 0x1006a390 ??1inventory_item@CSBInventory@@QAE@XZ
// 0x1006d3e0 ?find_last_of@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@QBEIPBDII@Z
// 0x1006d470 ?insert@?$vector@Uinventory_item@CSBInventory@@V?$MBAllocator@Uinventory_item@CSBInventory@@@@@std@@QAEXPAUinventory_item@CSBInventory@@IABU34@@Z
// 0x1006d830 ?erase@?$vector@Uinventory_item@CSBInventory@@V?$MBAllocator@Uinventory_item@CSBInventory@@@@@std@@QAEPAUinventory_item@CSBInventory@@PAU34@0@Z
// 0x1006d8d0 ?_Refcnt@?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@AAEAAEPBD@Z
// 0x1006d8f0 ?deallocate@?$MBAllocator@D@@QAEXPAXK@Z
// 0x1006d920 ?erase@?$list@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@V?$MBAllocator@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1006d9c0 ?_Construct@std@@YAXPAUinventory_item@CSBInventory@@ABU23@@Z

// Instances the linker took from other objects (this unit emits them too):
// 0x1005ac20 ??1?$MBList@V?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@@QAE@XZ

// the unit includes these after its code (the line numbers above are the original ones)
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x100693e0 _$E53
// 0x10069400 _$E62
// 0x100699d0 _$E52
// 0x100699f0 _$E63
// 0x10069ae0 _$E65
// 0x10069b00 _$E49
// 0x10069cb0 _$E64
// 0x10069cd0 _$E50
// 0x10069d10 _$E56
// 0x10069d30 _$E55
// 0x10069db0 _$E59
// 0x10069dd0 _$E58
// 0x1006db70 _$E77
// 0x1006dbb0 _$E73
// 0x1006dbd0 _$E76
