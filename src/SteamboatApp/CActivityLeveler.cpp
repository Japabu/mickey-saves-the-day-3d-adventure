#include "CActivityLeveler.h"
#include "SteamboatNode.h"
#include <string.h>

// 0x100072e0 ??0CActivityLeveler@@QAE@JJ@Z
CActivityLeveler::CActivityLeveler(long nChannel, long nEvent)
    : m_nCurrentLevel(1), m_nLastLevel(1), m_nMinLevel(1), m_nMaxLevel(10), m_eState(kIdle),
      m_pLevelUpButton(0), m_pLevelDownButton(0), m_nChannel(nChannel), m_nEvent(nEvent)
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    _mRefCnt = 0;
    memset(m_arrayLevelAnims, 0, sizeof(m_arrayLevelAnims));
    if (eventManager) {
        eventManager->AddListener(205, this);
        eventManager->AddListener(-2, this);
    }
}

// 0x10007410 ?QueryInterface@CActivityLeveler@@UAEJABUMBGUID@@PAPAX@Z
long CActivityLeveler::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100074b0 ?AddRef@CActivityLeveler@@UAEKXZ
unsigned long CActivityLeveler::AddRef()
{
    return ++_mRefCnt;
}

// 0x100074d0 ?Release@CActivityLeveler@@UAEKXZ
unsigned long CActivityLeveler::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10007500 ??_GCActivityLeveler@@UAEPAXI@Z

// Create's steps: a failing one throws its source line and (evaluated again) its error code.
#define THROW_ON_ERROR(expr) if ((expr) != 0) throw exception(__LINE__, expr)
#define SAFE_RELEASE(p) if (p) { p->Release(); p = 0; }

// 0x10007610 ?Create@CActivityLeveler@@QAEJVMBPoint3@@J@Z
long CActivityLeveler::Create(MBPoint3 ptOrigin, long nStartingLevel)
{
    char szName[32] = { 0 };
    int i;
    IMBAnim *pAnim = 0;
    IMBUnknown *iUnknown = 0;
    long hr = 0;
    try {
        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_CSBAnimButton, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_ISBAnimButton, (void **)&m_pLevelUpButton));
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(SteamboatNode::CreateAndLoadAnim("GDNextLevel", pAnim));
        m_pLevelUpButton->SetAnim(pAnim);
        m_pLevelUpButton->SetBroadcastChannel(205);
        m_pLevelUpButton->SetClickMessage(10);
        SAFE_RELEASE(pAnim);

        THROW_ON_ERROR(m_pLevelUpButton->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown));
        SteamboatNode::AddToWorld(iUnknown);
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_CSBAnimButton, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_ISBAnimButton, (void **)&m_pLevelDownButton));
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(SteamboatNode::CreateAndLoadAnim("GDPrevLevel", pAnim));
        m_pLevelDownButton->SetAnim(pAnim);
        m_pLevelDownButton->SetBroadcastChannel(205);
        m_pLevelDownButton->SetClickMessage(11);
        SAFE_RELEASE(pAnim);

        THROW_ON_ERROR(m_pLevelDownButton->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown));
        SteamboatNode::AddToWorld(iUnknown);
        SAFE_RELEASE(iUnknown);

        // the level counter: GDCounter1 .. GDCounter10, shown one at a time
        for (i = 0; i < 10; i++) {
            sprintf(szName, "GDCounter%d", i + 1);
            THROW_ON_ERROR(SteamboatNode::AddAnimToWorld(szName, m_arrayLevelAnims[i]));
            m_arrayLevelAnims[i]->Play();
            if (m_arrayLevelAnims[i]->IsVisible())
                m_arrayLevelAnims[i]->Hide();
        }
        m_nLastLevel = m_nCurrentLevel = nStartingLevel;
        Place(ptOrigin);
        OnLevelChangeComplete();
    } catch (exception e) {
        SAFE_RELEASE(m_pLevelUpButton);
        SAFE_RELEASE(m_pLevelDownButton);
        SAFE_RELEASE(iUnknown);
        SAFE_RELEASE(pAnim);
        for (i = 0; i < 10; i++)
            SAFE_RELEASE(m_arrayLevelAnims[i]);
        hr = e.nError;
    }
    return hr;
}

// 0x10007540 ??1CActivityLeveler@@UAE@XZ
CActivityLeveler::~CActivityLeveler()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(this);
}

// 0x10007e20 ?Destroy@CActivityLeveler@@QAEXXZ
void CActivityLeveler::Destroy()
{
    IMBUnknown *iUnknown = 0;
    m_pLevelUpButton->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    SteamboatNode::RemoveFromWorld(iUnknown);
    if (iUnknown) {
        iUnknown->Release();
        iUnknown = 0;
    }
    if (m_pLevelUpButton) {
        m_pLevelUpButton->Release();
        m_pLevelUpButton = 0;
    }
    m_pLevelDownButton->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
    SteamboatNode::RemoveFromWorld(iUnknown);
    if (iUnknown) {
        iUnknown->Release();
        iUnknown = 0;
    }
    if (m_pLevelDownButton) {
        m_pLevelDownButton->Release();
        m_pLevelDownButton = 0;
    }
    for (int i = 0; i < 10; i++) {
        m_arrayLevelAnims[i]->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        if (m_arrayLevelAnims[i]) {
            m_arrayLevelAnims[i]->Release();
            m_arrayLevelAnims[i] = 0;
        }
    }
}

// 0x10007f40 ?ListenToEvent@CActivityLeveler@@MAEXJJPAX@Z
void CActivityLeveler::ListenToEvent(long channel, long event, void *data)
{
    if (channel == 205) {
        if (event == 10)
            OnNextLevel();
        else if (event == 11)
            OnPrevLevel();
    } else if (channel == -2) {
        if ((event == 6 || event == 4) && IsActivityLevelerAnim(((MBAnimEventStruct *)data)->anim)) {
            switch (m_eState) {
            case kStartChange:
                OnLevelChange();
                break;
            case kEndChange:
                OnLevelChangeComplete();
                break;
            }
        }
    }
}

// 0x10007fe0 ?Place@CActivityLeveler@@QAEXABVMBPoint3@@@Z
void CActivityLeveler::Place(const MBPoint3 &ptOrigin)
{
    m_ptOrigin = ptOrigin;
    Place();
}

// 0x10008020 ?Place@CActivityLeveler@@IAEXXZ
void CActivityLeveler::Place()
{
    m_pLevelUpButton->Place(m_ptOrigin);
    m_pLevelDownButton->Place(m_ptOrigin);
    for (int i = 0; i < 10; i++)
        m_arrayLevelAnims[i]->Place(m_ptOrigin);
}

// 0x10008070 ?ShowButtons@CActivityLeveler@@IAEX_N@Z
void CActivityLeveler::ShowButtons(bool bShow)
{
    if (bShow) {
        m_pLevelUpButton->Show();
        m_pLevelUpButton->Enable();
        m_pLevelDownButton->Show();
        m_pLevelDownButton->Enable();
    } else {
        if (m_pLevelUpButton->IsVisible())
            m_pLevelUpButton->Hide();
        if (m_pLevelDownButton->IsVisible())
            m_pLevelDownButton->Hide();
        m_pLevelDownButton->Disable();
        m_pLevelUpButton->Disable();
    }
}

// 0x10008110 ?OnNextLevel@CActivityLeveler@@IAEXXZ
void CActivityLeveler::OnNextLevel()
{
    if (m_nMaxLevel > m_nCurrentLevel) {
        ShowButtons(false);
        ChangeState(kStartChange);
        m_nLastLevel = m_nCurrentLevel;
        m_nCurrentLevel++;
    }
}

// 0x10008150 ?OnPrevLevel@CActivityLeveler@@IAEXXZ
void CActivityLeveler::OnPrevLevel()
{
    if (m_nMinLevel < m_nCurrentLevel) {
        ShowButtons(false);
        ChangeState(kStartChange);
        m_nLastLevel = m_nCurrentLevel;
        m_nCurrentLevel--;
    }
}

// 0x10008190 ?OnLevelChange@CActivityLeveler@@IAEXXZ
void CActivityLeveler::OnLevelChange()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    eventManager->BroadcastEvent(m_nChannel, m_nEvent, (void *)m_nCurrentLevel);
    ChangeState(kEndChange);
}

// 0x100082a0 ?OnLevelChangeComplete@CActivityLeveler@@IAEXXZ
void CActivityLeveler::OnLevelChangeComplete()
{
    ShowButtons(true);
    ChangeState(kIdle);
}

// 0x100082d0 ?IsActivityLevelerAnim@CActivityLeveler@@IAE_NPAVIMBAnim@@@Z
bool CActivityLeveler::IsActivityLevelerAnim(IMBAnim *pAnim)
{
    bool bFound = false;
    for (int i = 0; i < 10 && !bFound; i++)
        bFound = m_arrayLevelAnims[i] == pAnim;
    return bFound;
}

// 0x10008310 ?ChangeState@CActivityLeveler@@IAEXW4state@1@@Z
void CActivityLeveler::ChangeState(state eState)
{
    m_eState = eState;
    IMBAnim *pAnim = m_arrayLevelAnims[m_nLastLevel - 1];
    if (pAnim->IsVisible())
        pAnim->Hide();
    pAnim = m_arrayLevelAnims[m_nCurrentLevel - 1];
    pAnim->SetCurrentSequence(eState);
    pAnim->Show();
}

// 0x10008380 ?SetLevel@CActivityLeveler@@QAEXJ@Z
void CActivityLeveler::SetLevel(long nLevel)
{
    m_nLastLevel = m_nCurrentLevel;
    m_nCurrentLevel = nLevel;
    OnLevelChangeComplete();
}

// 0x100083b0 ?GetLevel@CActivityLeveler@@QAEJXZ
long CActivityLeveler::GetLevel()
{
    return m_nCurrentLevel;
}

// 0x100083d0 ?SetLevelRange@CActivityLeveler@@QAEXJJ@Z
void CActivityLeveler::SetLevelRange(long nMinLevel, long nMaxLevel)
{
    m_nMinLevel = nMinLevel;
    m_nMaxLevel = nMaxLevel;
}

// Template instances and initializers emitted by the code above:
// 0x10008560 ??1?$MBComPtr@VIMBEventManager@@@@QAE@XZ
// 0x10008600 ?Release@?$MBComPtr@VIMBEventManager@@@@QAEXXZ
// 0x10008960 ?_QueryInterface@?$MBComPtr@VIMBEventManager@@@@AAEJABQAVIMBUnknown@@@Z
// 0x10008b30 ?id@?$ctype@G@std@@$E
