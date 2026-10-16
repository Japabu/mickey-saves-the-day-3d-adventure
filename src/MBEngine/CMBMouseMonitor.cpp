// CMBMouseMonitor.cpp - MBEngine.dll CMBMouseMonitor.obj
#include "CMBMouseMonitor.h"

// 0x1004b650 ?MakeCMBMouseMonitor@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBMouseMonitor(void)
{
    IMBUnknown *p = (IMBMouseMonitor *)new CMBMouseMonitor;
    if (p)
        p->AddRef();
    return p;
}

// 0x1004b680 ??0CMBMouseMonitor@@QAE@XZ
CMBMouseMonitor::CMBMouseMonitor(void)
    : mCurrentRollover(0), mCurrentClick(0)
{
    _mRefCnt = 0;
    mDoZTest = true;
    Activate();
    MBComPtr<IMBEventManager> broadcaster;
    broadcaster.CreateObject(MBIID_IMBEventManager);
    broadcaster->AddListener(-1, this);
    broadcaster->AddListener(-3, this);
}

// 0x1004b740 ?QueryInterface@CMBMouseMonitor@@UAEJABUMBGUID@@PAPAX@Z
long CMBMouseMonitor::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBMouseMonitor *)this;
    if (iid == MBIID_IMBMouseMonitor)
        p = (IMBMouseMonitor *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1004b850 ?AddRef@CMBMouseMonitor@@UAEKXZ
unsigned long CMBMouseMonitor::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1004b860 ?Release@CMBMouseMonitor@@UAEKXZ
unsigned long CMBMouseMonitor::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x1004b890 ??1CMBMouseMonitor@@QAE@XZ
CMBMouseMonitor::~CMBMouseMonitor(void)
{
    Deactivate();
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster) {
        broadcaster->RemoveListener(-1, this);
        broadcaster->RemoveListener(-3, this);
    }
    for (MBMouseableList::iterator it = mMouseables.begin(); it != mMouseables.end(); ++it) {
        (*it)->Release();
        *it = 0;
    }
}

// 0x1004b970 ?AddMouseable@CMBMouseMonitor@@UAEXPAVIMBMouseable@@@Z
void CMBMouseMonitor::AddMouseable(IMBMouseable *mouseable)
{
    if (std::find(mMouseables.begin(), mMouseables.end(), mouseable) == mMouseables.end()) {
        mouseable->AddRef();
        mMouseables.push_back(mouseable);
    }
}

// 0x1004bb20 ?RemoveMouseable@CMBMouseMonitor@@UAEXPAVIMBMouseable@@@Z
void CMBMouseMonitor::RemoveMouseable(IMBMouseable *mouseable)
{
    MBMouseableList::iterator it = std::find(mMouseables.begin(), mMouseables.end(), mouseable);
    if (it != mMouseables.end()) {
        (*it)->Release();
        *it = 0;
        mMouseables.erase(it);
    }
    if (mouseable == mCurrentRollover)
        mCurrentRollover = 0;
    if (mouseable == mCurrentClick)
        mCurrentClick = 0;
}

// 0x1004bba0 ?Activate@CMBMouseMonitor@@UAEXXZ
void CMBMouseMonitor::Activate(void)
{
    mIsActive = true;
}

// 0x1004bbb0 ?Deactivate@CMBMouseMonitor@@UAEXXZ
void CMBMouseMonitor::Deactivate(void)
{
    mIsActive = false;
}

// 0x1004bbc0 ?IsActive@CMBMouseMonitor@@UAE_NXZ
bool CMBMouseMonitor::IsActive(void)
{
    return mIsActive;
}

// 0x1004bbd0 ?ListenToEvent@CMBMouseMonitor@@UAEXJJPAX@Z
void CMBMouseMonitor::ListenToEvent(long channel, long event, void *data)
{
    if (IsActive()) {
        if (channel == -1) {
            if (event == 5) {
                MBMouseEvent *mouseEvent = (MBMouseEvent *)data;
                MBPoint pos = mouseEvent->mMousePos;
                HandleMouseDown(pos, mouseEvent->mScreen, mouseEvent->mButton);
            } else if (event == 6) {
                MBMouseEvent *mouseEvent = (MBMouseEvent *)data;
                MBPoint pos = mouseEvent->mMousePos;
                HandleMouseUp(pos, mouseEvent->mButton);
            }
        } else if (channel == -3) {
            if (event == 0) {
                CheckRollovers();
                CheckClicks();
            }
        }
    }
}

// EQUIV 0x1004bc80 ?HandleMouseDown@CMBMouseMonitor@@IAEXABVMBPoint@@PAVIMBScreen@@F@Z: reviewed: register and slot choices only (the original spills &mCurrentClick and compares the byte member directly; VC6 loads and compares). Was: register allocation: original spills &mCurrentClick (ebp) to a stack slot and uses ebp for the GetPosition result; ours reuses ebx (frame 0x40 vs 0x44) (82% similar)
void CMBMouseMonitor::HandleMouseDown(const MBPoint &screenPoint, IMBScreen *screen, short button)
{
    MBPoint worldPoint = screenPoint;
    if (mCurrentRollover && !mCurrentRollover->IsActive())
        mCurrentRollover = 0;
    if (mCurrentClick && !mCurrentClick->IsActive())
        mCurrentClick = 0;
    if (!mCurrentClick) {
        std::sort(mMouseables.begin(), mMouseables.end(), std::ptr_fun(CompareMouseablesGreaterThan));
        MBMouseableList::iterator it;
        for (it = mMouseables.begin(); it != mMouseables.end(); ++it) {
            MBComPtr<IMBMouseable> mouse(*it);
            MBComPtr<IMBObject> objtest(mouse);
            MBComPtr<IMBObject> objclick(mCurrentClick);
            if (objtest)
                objtest->GetPosition();
            if (objclick)
                objclick->GetPosition();
            if ((*it)->IsActive()) {
                bool doClickTest = true;
                if (mCurrentClick && objtest && objclick)
                    doClickTest = objtest->GetPosition().z > objclick->GetPosition().z && mDoZTest;
                if (doClickTest && (*it)->IsClicked(worldPoint, button)) {
                    mCurrentClick = *it;
                    if (mCurrentRollover && mCurrentRollover != mCurrentClick) {
                        ClearCurrentRollover();
                        SetCurrentRollover(mCurrentClick, worldPoint);
                    }
                    break;
                }
            }
        }
        if (mCurrentClick)
            mCurrentClick->StartTrackingClick(worldPoint, button);
    }
}

static inline long GetMouseableZ(IMBMouseable *mouseable)
{
    MBComPtr<IMBObject> obj(mouseable);
    return obj ? obj->GetPosition().z : 0x7fffffff;
}

// 0x1004be70 ?CompareMouseablesGreaterThan@@YA_NPAVIMBMouseable@@0@Z
bool CompareMouseablesGreaterThan(IMBMouseable *arg1, IMBMouseable *arg2)
{
    return GetMouseableZ(arg1) > GetMouseableZ(arg2);
}

// 0x1004bf70 ?HandleMouseUp@CMBMouseMonitor@@IAEXABVMBPoint@@F@Z
void CMBMouseMonitor::HandleMouseUp(const MBPoint &globalPoint, short button)
{
    MBPoint worldPoint = globalPoint;
    if (mCurrentRollover && !mCurrentRollover->IsActive())
        mCurrentRollover = 0;
    if (mCurrentClick && !mCurrentClick->IsActive())
        mCurrentClick = 0;
    if (mCurrentClick) {
        mCurrentClick->StopTrackingClick(worldPoint, button);
        mCurrentClick = 0;
    }
}

// 0x1004bfe0 ?CheckRollovers@CMBMouseMonitor@@IAEXXZ
void CMBMouseMonitor::CheckRollovers(void)
{
    MBPoint worldPoint;
    IMBWorld *tempworld;
    if (mCurrentRollover && !mCurrentRollover->IsActive())
        mCurrentRollover = 0;
    if (mCurrentClick && !mCurrentClick->IsActive())
        mCurrentClick = 0;
    if (!MBGetWorldMouseLocation(worldPoint, &tempworld)) {
        ClearCurrentRollover();
        return;
    }
    MBComPtr<IMBWorld> world(tempworld, false);
    if (mCurrentClick) {
        if (mCurrentClick->IsRolledOver(worldPoint)) {
            if (mCurrentRollover)
                mCurrentClick->WithinRollover(worldPoint);
            else
                SetCurrentRollover(mCurrentClick, worldPoint);
        } else {
            ClearCurrentRollover();
        }
        return;
    }
    {
        std::sort(mMouseables.begin(), mMouseables.end(), std::ptr_fun(CompareMouseablesGreaterThan));
        for (unsigned int i = 0; i < mMouseables.size(); i++) {
            MBComPtr<IMBObject> obj(mMouseables[i]);
            obj->GetPosition();
        }
        MBMouseableList::iterator it;
        for (it = mMouseables.begin(); it != mMouseables.end(); ++it) {
            if ((*it)->IsRolledOver(worldPoint)) {
                if (mCurrentRollover == *it) {
                    mCurrentRollover->WithinRollover(worldPoint);
                } else {
                    ClearCurrentRollover();
                    SetCurrentRollover(*it, worldPoint);
                }
                break;
            }
        }
        if (it == mMouseables.end())
            ClearCurrentRollover();
    }
}

// 0x1004c1b0 ?CheckClicks@CMBMouseMonitor@@IAEXXZ
void CMBMouseMonitor::CheckClicks(void)
{
    MBPoint worldPoint;
    IMBWorld *world;
    if (mCurrentRollover && !mCurrentRollover->IsActive())
        mCurrentRollover = 0;
    if (mCurrentClick && !mCurrentClick->IsActive())
        mCurrentClick = 0;
    if (mCurrentClick && MBGetWorldMouseLocation(worldPoint, &world)) {
        if (world)
            world->Release();
        mCurrentClick->TrackClick(worldPoint);
    }
}

// 0x1004c230 ?SetCurrentRollover@CMBMouseMonitor@@IAEXPAVIMBMouseable@@ABVMBPoint@@@Z
void CMBMouseMonitor::SetCurrentRollover(IMBMouseable *mouseable, const MBPoint &worldPoint)
{
    mCurrentRollover = mouseable;
    mouseable->EnterRollover(worldPoint);
}

// 0x1004c250 ?ClearCurrentRollover@CMBMouseMonitor@@IAEXXZ
void CMBMouseMonitor::ClearCurrentRollover(void)
{
    if (mCurrentRollover) {
        mCurrentRollover->LeaveRollover();
        mCurrentRollover = 0;
    }
}


// Matched by this code:
// 0x1004c270 _$E2
// 0x1004c280 _$E1
// 0x1004c350 _$E5
// 0x1004c360 _$E4
// 0x1004c430 _$E8
// 0x1004c440 _$E7
// 0x1004c510 _$E11
// 0x1004c520 _$E10
// 0x1004c5e0 _$E14
// 0x1004c5f0 _$E13
// 0x1004c6b0 _$E17
// 0x1004c6c0 _$E16
// 0x1004c770 _$E20
// 0x1004c780 _$E19
// 0x1004c830 _$E23
// 0x1004c840 _$E22
// 0x1004c8f0 _$E26
// 0x1004c900 _$E25
// 0x1004c9c0 _$E29
// 0x1004c9d0 _$E28
// 0x1004ca90 _$E32
// 0x1004caa0 _$E31
// 0x1004cb50 _$E35
// 0x1004cb60 _$E34
// 0x1004cc10 _$E38
// 0x1004cc20 _$E37
// 0x1004ccd0 _$E41
// 0x1004cce0 _$E40
// 0x1004cd90 _$E44
// 0x1004cda0 _$E43
// 0x1004ce50 _$E47
// 0x1004ce60 _$E46
// 0x1004cf10 ?size@?$vector@PAVIMBMouseable@@V?$MBAllocator@PAVIMBMouseable@@@@@std@@QBEIXZ
// 0x1004cf30 ?_Destroy@?$vector@PAVIMBMouseable@@V?$MBAllocator@PAVIMBMouseable@@@@@std@@IAEXPAPAVIMBMouseable@@0@Z
// 0x1004cf40 ?Release@?$MBComPtr@VIMBObject@@@@QAEXXZ
// 0x1004cf60 ?_Ucopy@?$vector@PAVIMBMouseable@@V?$MBAllocator@PAVIMBMouseable@@@@@std@@IAEPAPAVIMBMouseable@@PBQAV3@0PAPAV3@@Z
// 0x1004cf90 ?_Ufill@?$vector@PAVIMBMouseable@@V?$MBAllocator@PAVIMBMouseable@@@@@std@@IAEXPAPAVIMBMouseable@@IABQAV3@@Z
// 0x1004cfc0 ?_Construct@std@@YAXPAPAVIMBMouseable@@ABQAV2@@Z
// 0x1004cfe0 ?_QueryInterface@?$MBComPtr@VIMBObject@@@@AAEJABQAVIMBMouseable@@@Z
// 0x1004d060 ?_Sort_0@std@@YAXPAPAVIMBMouseable@@0V?$pointer_to_binary_function@PAVIMBMouseable@@PAV1@_N@1@0@Z
// 0x1004d300 ?_QueryInterface@?$MBComPtr@VIMBObject@@@@AAEJABV?$MBComPtr@VIMBMouseable@@@@@Z
// 0x1004d380 _$E67
// 0x1004d3a0 _$E66
// 0x1004d3b0 ??4?$MBComPtr@VIMBObject@@@@QAEAAV0@PAVIMBObject@@@Z
// 0x1004d3e0 ?_Sort@std@@YAXPAPAVIMBMouseable@@0V?$pointer_to_binary_function@PAVIMBMouseable@@PAV1@_N@1@0@Z
// 0x1004d510 ?QueryInterface@CMBMouseMonitor@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1004d520 ?AddRef@CMBMouseMonitor@@W3AEKXZ
// 0x1004d530 ?Release@CMBMouseMonitor@@W3AEKXZ
// steamboatapp.dll 0x10087270 ?Release@?$MBComPtr@VIMBObject@@@@QAEXXZ
// steamboatapp.dll 0x10087700 ??4?$MBComPtr@VIMBObject@@@@QAEAAV0@PAVIMBObject@@@Z
