// CMBGameEngine.cpp - MBEngine.dll CMBGameEngine.obj
#include "CMBGameEngine.h"

// 0x10009ff0 ?MakeCMBGameEngine@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBGameEngine(void)
{
    IMBUnknown *p = new CMBGameEngine;
    if (p)
        p->AddRef();
    return p;
}

// 0x1000a020 ??0CMBGameEngine@@QAE@XZ
CMBGameEngine::CMBGameEngine(void)
    : mfOnMBEngineStartup(0), mfOnMBEngineShutdown(0), mOnMBEngineCreateScreen(0), mIsQuitting(false)
{
    _mRefCnt = 0;
    mNodeMgr.CreateObject(MBIID_CMBNodeManager);
    mStringTable.CreateObject(MBIID_CMBStringTable);
}

// 0x1000a130 ?QueryInterface@CMBGameEngine@@UAEJABUMBGUID@@PAPAX@Z
long CMBGameEngine::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBGameEngine)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000a200 ?AddRef@CMBGameEngine@@UAEKXZ
unsigned long CMBGameEngine::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000a210 ?Release@CMBGameEngine@@UAEKXZ
unsigned long CMBGameEngine::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1000a240 ??1CMBGameEngine@@QAE@XZ
CMBGameEngine::~CMBGameEngine(void)
{
}

// 0x1000a280 ?SetOnEngineStartupFunction@CMBGameEngine@@UAEXP6AXPAVIMBGameEngine@@@Z@Z
void CMBGameEngine::SetOnEngineStartupFunction(void (*func)(IMBGameEngine *))
{
    mfOnMBEngineStartup = func;
}

// 0x1000a290 ?SetOnEngineShutdownFunction@CMBGameEngine@@UAEXP6AXPAVIMBGameEngine@@@Z@Z
void CMBGameEngine::SetOnEngineShutdownFunction(void (*func)(IMBGameEngine *))
{
    mfOnMBEngineShutdown = func;
}

// 0x1000a2a0 ?SetOnEngineCreateScreenFunction@CMBGameEngine@@UAEXP6AJPAVIMBGameEngine@@PAVIMBScreenManager@@@Z@Z
void CMBGameEngine::SetOnEngineCreateScreenFunction(long (*func)(IMBGameEngine *, IMBScreenManager *))
{
    mOnMBEngineCreateScreen = func;
}

// EQUIV 0x1000a2b0 ?CreateScreen@CMBGameEngine@@UAEJPAVIMBScreenManager@@@Z: reviewed: branch layout only (the -2 result is selected with a jump instead of loaded before the test)
long CMBGameEngine::CreateScreen(IMBScreenManager *screenManager)
{
    long result = 0;
    if (mOnMBEngineCreateScreen)
        return mOnMBEngineCreateScreen(this, screenManager);
    MBScreenAttributes screenAttributes;
    screenAttributes.width = 640;
    screenAttributes.height = 480;
    screenAttributes.depth = 16;
    if (!screenManager->RequestScreen(1, screenAttributes))
        result = -2;
    return result;
}

// 0x1000a310 ?Init@CMBGameEngine@@UAEXXZ
void CMBGameEngine::Init(void)
{
}

// 0x1000a320 ?ShutDown@CMBGameEngine@@UAEXXZ
void CMBGameEngine::ShutDown(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->BroadcastEvent(-3, 5, 0);
    if (mfOnMBEngineShutdown)
        mfOnMBEngineShutdown(this);
}

// 0x1000a3a0 ?GetNodeMgr@CMBGameEngine@@UBE?AV?$MBComPtr@VIMBNodeManager@@@@XZ
MBComPtr<IMBNodeManager> CMBGameEngine::GetNodeMgr(void) const
{
    return mNodeMgr;
}

// 0x1000a3c0 ?Start@CMBGameEngine@@UAEXXZ
void CMBGameEngine::Start(void)
{
    if (mfOnMBEngineStartup)
        mfOnMBEngineStartup(this);
}

// 0x1000a3d0 ?Idle@CMBGameEngine@@UAEXXZ
void CMBGameEngine::Idle(void)
{
    MBComPtr<IMBPeriodicalManager> periodicalManager(MBIID_IMBPeriodicalManager);
    if (periodicalManager)
        periodicalManager->DevoteTimeToPeriodicals();
}

// 0x1000a430 ?UpdateScreen@CMBGameEngine@@UAEXABVMBRect@@@Z
void CMBGameEngine::UpdateScreen(const MBRect &bounds)
{
    MBComPtr<IMBViewManager> viewManager(MBIID_IMBViewManager);
    if (viewManager) {
        MBComPtr<IMBPeriodical> periodical(viewManager);
        if (periodical)
            periodical->Idle();
    }
}

// 0x1000a4e0 ?IsQuitting@CMBGameEngine@@UBE_NXZ
bool CMBGameEngine::IsQuitting(void) const
{
    return mIsQuitting;
}

// 0x1000a4f0 ?Quit@CMBGameEngine@@UAEXXZ
void CMBGameEngine::Quit(void)
{
    mIsQuitting = true;
}

// 0x1000a500 ?GetStringTable@CMBGameEngine@@UAEXPAPAVIMBStringTable@@@Z
void CMBGameEngine::GetStringTable(IMBStringTable **table)
{
    if (table) {
        *table = mStringTable;
        if (mStringTable)
            mStringTable->AddRef();
    }
}

// 0x1000a520 ?Release@?$MBComPtr@VIMBPeriodical@@@@QAEXXZ (MBComPtr template, out of line in UpdateScreen)
// 0x1000a540..0x1000b130 $E16..$E62: colour statics from CMBView.h
// 0x1000b1e0 _$E71 (original $E71,) 0x1000b200 $E70: std::ctype<unsigned short>::id from <string>


// Matched by this code:
// 0x1000a540 _$E17
// 0x1000a550 _$E16
// 0x1000a620 _$E20
// 0x1000a630 _$E19
// 0x1000a700 _$E23
// 0x1000a710 _$E22
// 0x1000a7e0 _$E26
// 0x1000a7f0 _$E25
// 0x1000a8b0 _$E29
// 0x1000a8c0 _$E28
// 0x1000a980 _$E32
// 0x1000a990 _$E31
// 0x1000aa40 _$E35
// 0x1000aa50 _$E34
// 0x1000ab00 _$E38
// 0x1000ab10 _$E37
// 0x1000abc0 _$E41
// 0x1000abd0 _$E40
// 0x1000ac90 _$E44
// 0x1000aca0 _$E43
// 0x1000ad60 _$E47
// 0x1000ad70 _$E46
// 0x1000ae20 _$E50
// 0x1000ae30 _$E49
// 0x1000aee0 _$E53
// 0x1000aef0 _$E52
// 0x1000afa0 _$E56
// 0x1000afb0 _$E55
// 0x1000b060 _$E59
// 0x1000b070 _$E58
// 0x1000b120 _$E62
// 0x1000b130 _$E61
// 0x1000b200 _$E70
