// CMBViewManager.cpp - MBEngine.dll CMBViewManager.obj
#include "CMBViewManager.h"
#include <algorithm>

// PDB ldata order in CMBViewManager.obj (0x1008b630..0x1008b6e0); MBIID_IMBUnknown comes first, from IMBUnknown.h via CMBView.h
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBView =
    { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager =
    { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFPSMonitor =
    { 0x8f082f50, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// 0x1002b440 ?MakeCMBViewManager@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBViewManager(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton((IMBViewManager *)new CMBViewManager);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x1002b4b0 ??0CMBViewManager@@QAE@XZ
CMBViewManager::CMBViewManager(void)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
}

// 0x1002b550 ?QueryInterface@CMBViewManager@@UAEJABUMBGUID@@PAPAX@Z
long CMBViewManager::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBViewManager *)this;
    if (iid == MBIID_IMBViewManager)
        p = (IMBViewManager *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002b660 ?AddRef@CMBViewManager@@UAEKXZ
unsigned long CMBViewManager::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002b670 ?Release@CMBViewManager@@UAEKXZ
unsigned long CMBViewManager::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002b6a0 ??1CMBViewManager@@QAE@XZ
CMBViewManager::~CMBViewManager(void)
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    for (MBVector<IMBView *>::iterator it = mViews.begin(); it != mViews.end(); ++it)
        (*it)->Release();
}

// 0x1002b760 ?AddView@CMBViewManager@@UAEXPAVIMBView@@@Z
void CMBViewManager::AddView(IMBView *view)
{
    if (std::find(mViews.begin(), mViews.end(), view) == mViews.end()) {
        view->AddRef();
        mViews.push_back(view);
    }
}

// 0x1002b910 ?RemoveView@CMBViewManager@@UAEXPAVIMBView@@@Z
void CMBViewManager::RemoveView(IMBView *view)
{
    MBVector<IMBView *>::iterator it = std::find(mViews.begin(), mViews.end(), view);
    if (it != mViews.end()) {
        view->Release();
        mViews.erase(it);
    }
}

// 0x1002b960 ?FindViewContaingPoint@CMBViewManager@@UAEXPAVIMBScreen@@ABVMBPoint@@PAPAVIMBView@@@Z
void CMBViewManager::FindViewContaingPoint(IMBScreen *screen, const MBPoint &point, IMBView **view)
{
    *view = 0;
    for (MBVector<IMBView *>::iterator it = mViews.begin(); it != mViews.end(); ++it) {
        MBRect bounds = (*it)->GetScreenBounds();
        if (bounds.Contains(point)) {
            *view = *it;
            (*view)->AddRef();
            return;
        }
    }
}

// 0x1002b9e0 ?Idle@CMBViewManager@@UAEXXZ
void CMBViewManager::Idle(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    MBComPtr<IMBScreenManager> screenManager;
    IMBScreenManager *tempScreenManager;
    MBGetScreenManager(&tempScreenManager);
    screenManager = tempScreenManager;
    unsigned short numScreens = screenManager->GetNumScreens();
    for (unsigned short i = 0; i < numScreens; i++) {
        MBComPtr<IMBScreen> screen;
        IMBScreen *tempScreen;
        screenManager->GetScreenByIndex(i, &tempScreen);
        screen = tempScreen;
        if (screen) {
            IMBSurface *backBuffer;
            screen->GetBackBuffer(&backBuffer);
            if (backBuffer) {
                if (broadcaster)
                    broadcaster->BroadcastEvent(-3, 0, screen);
                for (MBVector<IMBView *>::iterator it = mViews.begin(); it != mViews.end(); ++it) {
                    if ((*it)->IsAttachedToScreen(screen) && (*it)->IsDirty())
                        (*it)->Draw(backBuffer);
                }
                if (broadcaster)
                    broadcaster->BroadcastEvent(-3, 1, screen);
                screen->Flip();
                if (broadcaster)
                    broadcaster->BroadcastEvent(-3, 2, screen);
                backBuffer->Release();
            }
        }
    }
}


// Matched by this code:
// 0x1001ff80 $E68
// 0x1001ffa0 $E67
// 0x1002bb50 _$E2
// 0x1002bb60 _$E1
// 0x1002bc30 _$E5
// 0x1002bc40 _$E4
// 0x1002bd10 _$E8
// 0x1002bd20 _$E7
// 0x1002bdf0 _$E11
// 0x1002be00 _$E10
// 0x1002bec0 _$E14
// 0x1002bed0 _$E13
// 0x1002bf90 _$E17
// 0x1002bfa0 _$E16
// 0x1002c050 _$E20
// 0x1002c060 _$E19
// 0x1002c110 _$E23
// 0x1002c120 _$E22
// 0x1002c1d0 _$E26
// 0x1002c1e0 _$E25
// 0x1002c2a0 _$E29
// 0x1002c2b0 _$E28
// 0x1002c370 _$E32
// 0x1002c380 _$E31
// 0x1002c430 _$E35
// 0x1002c440 _$E34
// 0x1002c4f0 _$E38
// 0x1002c500 _$E37
// 0x1002c5b0 _$E41
// 0x1002c5c0 _$E40
// 0x1002c670 _$E44
// 0x1002c680 _$E43
// 0x1002c730 _$E47
// 0x1002c740 _$E46
// 0x1002c7f0 ?_Destroy@?$vector@PAVIMBView@@V?$MBAllocator@PAVIMBView@@@@@std@@IAEXPAPAVIMBView@@0@Z
// 0x1002c800 ?size@?$vector@PAVIMBView@@V?$MBAllocator@PAVIMBView@@@@@std@@QBEIXZ
// 0x1002c820 ?_Ucopy@?$vector@PAVIMBView@@V?$MBAllocator@PAVIMBView@@@@@std@@IAEPAPAVIMBView@@PBQAV3@0PAPAV3@@Z
// 0x1002c850 ?_Ufill@?$vector@PAVIMBView@@V?$MBAllocator@PAVIMBView@@@@@std@@IAEXPAPAVIMBView@@IABQAV3@@Z
// 0x1002c880 ?_Construct@std@@YAXPAPAVIMBView@@ABQAV2@@Z
// 0x1002c8a0 _$E68
// 0x1002c8c0 _$E67
// 0x1002c8d0 ?QueryInterface@CMBViewManager@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1002c8e0 ?AddRef@CMBViewManager@@W3AEKXZ
// 0x1002c8f0 ?Release@CMBViewManager@@W3AEKXZ
// 0x10042d00 $E68
// 0x10042d20 $E67
