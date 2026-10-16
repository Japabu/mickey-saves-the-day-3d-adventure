#include "ConfirmationBox.h"
#include "SteamboatNode.h"

// 0x10017550 ??0ConfirmationBox@@QAE@XZ
ConfirmationBox::ConfirmationBox()
    : mMyMouseMonitor(MBIID_CMBMouseMonitor), mBackground(0), mYes(0), mNo(0), mActive(false)
{
    _mRefCnt = 0;
    mMyMouseMonitor->Deactivate();
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->AddListener(210, this);
}

// 0x100176e0 ?QueryInterface@ConfirmationBox@@UAEJABUMBGUID@@PAPAX@Z
long ConfirmationBox::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100177a0 ?Release@ConfirmationBox@@UAEKXZ
unsigned long ConfirmationBox::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100177d0 ?SetBackgroundName@ConfirmationBox@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void ConfirmationBox::SetBackgroundName(MBString bgname)
{
    mBackgroundName = bgname;
}

// 0x10017990 ?SetButtonNames@ConfirmationBox@@UAEXV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@0@Z
void ConfirmationBox::SetButtonNames(MBString yesname, MBString noname)
{
    mYesName = yesname;
    mNoName = noname;
}

// Create's steps: a failing one throws its source line and (evaluated again) its error code.
#define THROW_ON_ERROR(expr) if ((expr) != 0) throw exception(__LINE__, expr)
#define SAFE_RELEASE(p) if (p) { p->Release(); p = 0; }

// 0x10017da0 ?Create@ConfirmationBox@@UAEJXZ
long ConfirmationBox::Create()
{
    IMBAnim *pAnim = 0;
    IMBUnknown *iUnknown = 0;
    long hr = 0;
    // take the mouse from the scene
    mNodeMouseMonitor = SteamboatNode::GetActiveMouseMonitor();
    mNodeMouseMonitor->Deactivate();
    mMyMouseMonitor->Activate();
    SteamboatNode::SetActiveMouseMonitor(mMyMouseMonitor);
    try {
        THROW_ON_ERROR(SteamboatNode::AddAnimToWorld(mBackgroundName.c_str(), mBackground));
        if (mBackground) {
            mBackground->Place(MBPoint3(0, 0, 9000));
            mBackground->Play();
        }

        // the Yes button (the original has one more line here, without code)

        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_CSBAnimButton, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_ISBAnimButton, (void **)&mYes));
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(SteamboatNode::CreateAndLoadAnim(mYesName.c_str(), pAnim));
        mYes->SetAnim(pAnim);
        mYes->SetBroadcastChannel(210);
        mYes->SetClickMessage(211);
        // in front of the background
        mYes->Place(MBPoint3(0, 0, 9010));
        SAFE_RELEASE(pAnim);

        THROW_ON_ERROR(mYes->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown));
        SteamboatNode::AddToWorld(iUnknown);
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(MBComCreateInstance(0, MBIID_CSBAnimButton, &iUnknown));
        THROW_ON_ERROR(iUnknown->QueryInterface(MBIID_ISBAnimButton, (void **)&mNo));
        SAFE_RELEASE(iUnknown);

        THROW_ON_ERROR(SteamboatNode::CreateAndLoadAnim(mNoName.c_str(), pAnim));
        mNo->SetAnim(pAnim);
        mNo->SetBroadcastChannel(210);
        mNo->SetClickMessage(212);
        // in front of the background
        mNo->Place(MBPoint3(0, 0, 9010));
        SAFE_RELEASE(pAnim);

        THROW_ON_ERROR(mNo->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown));
        SteamboatNode::AddToWorld(iUnknown);
        SAFE_RELEASE(iUnknown);
        mActive = true;
    } catch (exception e) {
        SAFE_RELEASE(mBackground);
        SAFE_RELEASE(mYes);
        SAFE_RELEASE(mNo);
        SAFE_RELEASE(iUnknown);
        SAFE_RELEASE(pAnim);
        hr = e.nError;
    }
    return hr;
}

// 0x10017780 ?AddRef@ConfirmationBox@@UAEKXZ
unsigned long ConfirmationBox::AddRef()
{
    return ++_mRefCnt;
}

// 0x10017b50 ?IsActive@ConfirmationBox@@UAE_NXZ
bool ConfirmationBox::IsActive()
{
    return mActive;
}

// 0x10017b70 ??_GConfirmationBox@@UAEPAXI@Z

// 0x10017bb0 ??1ConfirmationBox@@UAE@XZ
ConfirmationBox::~ConfirmationBox()
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager)
        eventManager->RemoveListener(this);
}

// 0x10017d80 ?ListenToEvent@ConfirmationBox@@MAEXJJPAX@Z
void ConfirmationBox::ListenToEvent(long channel, long event, void *data)
{
}

// 0x100186d0 ?Destroy@ConfirmationBox@@UAEXXZ
void ConfirmationBox::Destroy()
{
    // take the three animations out of the world and give the mouse back to the scene
    IMBUnknown *iUnknown = 0;
    if (mBackground) {
        mBackground->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        mBackground->Release();
        mBackground = 0;
    }
    if (mYes) {
        mYes->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        mYes->Release();
        mYes = 0;
    }
    if (mNo) {
        mNo->QueryInterface(MBIID_IMBUnknown, (void **)&iUnknown);
        SteamboatNode::RemoveFromWorld(iUnknown);
        if (iUnknown) {
            iUnknown->Release();
            iUnknown = 0;
        }
        mNo->Release();
        mNo = 0;
    }
    mMyMouseMonitor->Deactivate();
    if (mNodeMouseMonitor) {
        mNodeMouseMonitor->Activate();
        SteamboatNode::SetActiveMouseMonitor(mNodeMouseMonitor);
        mNodeMouseMonitor = 0;
    }
    mActive = false;
}

// Template and inline instances this unit also emits:

// 0x10018ae0 ?Release@?$MBComPtr@VIMBMouseMonitor@@@@QAEXXZ

// 0x10018db0 ?_QueryInterface@?$MBComPtr@VIMBMouseMonitor@@@@AAEJABQAVIMBUnknown@@@Z

// the unit includes these after its code (the line numbers above are the original ones)
#include "SBCursorItems.h" // every node unit of the original has the cursor item strings
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers

// 0x10016f10 _$E49
// 0x10016f30 _$E46
// 0x10017500 _$E48
// 0x10017520 _$E47
// 0x10018e50 _$E61
// 0x10018e90 _$E57
// 0x10018eb0 _$E60
