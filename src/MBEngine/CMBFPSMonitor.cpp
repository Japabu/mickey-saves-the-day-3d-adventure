// MBEngine.dll .\MBEngineRelease\CMBFPSMonitor.obj
#include "CMBFPSMonitor.h"

// 0x10023c30 ?MakeCMBFPSMonitor@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBFPSMonitor(void)
{
    static IMBUnknown *instance = MBComUtilAddSingleton((IMBFPSMonitor *)new CMBFPSMonitor);
    if (instance)
        instance->AddRef();
    return instance;
}

// 0x10023ca0 ??0CMBFPSMonitor@@QAE@XZ
CMBFPSMonitor::CMBFPSMonitor(void)
    : mEventLoopSlot(0), mEventLoopFPS(0), mDrawLoopSlot(0), mDrawLoopFPS(0), mIsEnabled(false)
{
    _mRefCnt = 0;
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->AddListener(-3, this);
    int i;
    for (i = 0; i < 3; i++)
        mEventLoopHistory[i] = 0;
    for (i = 0; i < 3; i++)
        mDrawLoopHistory[i] = 0;
}

// 0x10023d50 ?QueryInterface@CMBFPSMonitor@@UAEJABUMBGUID@@PAPAX@Z
long CMBFPSMonitor::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBFPSMonitor *)this;
    if (iid == MBIID_IMBFPSMonitor)
        p = (IMBFPSMonitor *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10023e60 ?AddRef@CMBFPSMonitor@@UAEKXZ
unsigned long CMBFPSMonitor::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10023e70 ?Release@CMBFPSMonitor@@UAEKXZ
unsigned long CMBFPSMonitor::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x10023ea0 ??1CMBFPSMonitor@@QAE@XZ
CMBFPSMonitor::~CMBFPSMonitor(void)
{
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-3, this);
}

// 0x10023f30 ?SetEventLoopTime@CMBFPSMonitor@@UAEXK@Z
void CMBFPSMonitor::SetEventLoopTime(unsigned long time)
{
    mEventLoopHistory[mEventLoopSlot] = time;
    if (++mEventLoopSlot == 3)
        mEventLoopSlot = 0;
    unsigned long total = 0;
    unsigned long count = 0;
    for (int i = 0; i < 3; i++) {
        if (mEventLoopHistory[i] > 0) {
            total += mEventLoopHistory[i];
            count++;
        }
    }
    if (count > 0) {
        unsigned long average = total / count;
        if (average > 0)
            mEventLoopFPS = 1000 / average;
    }
}

// 0x10023f90 ?SetDrawLoopTime@CMBFPSMonitor@@UAEXK@Z
void CMBFPSMonitor::SetDrawLoopTime(unsigned long time)
{
    mDrawLoopHistory[mDrawLoopSlot] = time;
    if (++mDrawLoopSlot == 3)
        mDrawLoopSlot = 0;
    unsigned long total = 0;
    unsigned long count = 0;
    for (int i = 0; i < 3; i++) {
        if (mDrawLoopHistory[i] > 0) {
            total += mDrawLoopHistory[i];
            count++;
        }
    }
    if (count > 0) {
        unsigned long average = total / count;
        if (average > 0)
            mDrawLoopFPS = 1000 / average;
    }
}

// 0x10023ff0 ?GetEventLoopFPS@CMBFPSMonitor@@UBEKXZ
unsigned long CMBFPSMonitor::GetEventLoopFPS(void) const
{
    return mEventLoopFPS;
}

// 0x10024000 ?GetDrawLoopFPS@CMBFPSMonitor@@UBEKXZ
unsigned long CMBFPSMonitor::GetDrawLoopFPS(void) const
{
    return mDrawLoopFPS;
}

// 0x10024010 ?Enable@CMBFPSMonitor@@UAEX_N@Z
void CMBFPSMonitor::Enable(bool isEnabled)
{
    mIsEnabled = isEnabled;
}

// 0x10024020 ?ListenToEvent@CMBFPSMonitor@@UAEXJJPAX@Z
void CMBFPSMonitor::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -3 && event == 1 && mIsEnabled) {
        if (!mFont) {
            MakeFont();
            if (!mFont)
                return;
        }
        IMBSurface *backBuffer;
        ((IMBScreen *)data)->GetBackBuffer(&backBuffer);
        if (backBuffer) {
            char textBuffer[16];
            sprintf(textBuffer, "%d", GetEventLoopFPS());
            mFont->DrawText(backBuffer, 0, 0, textBuffer, MBstrlen(textBuffer));
            sprintf(textBuffer, "%d", GetDrawLoopFPS());
            mFont->DrawText(backBuffer, 0, 20, textBuffer, MBstrlen(textBuffer));
            backBuffer->Release();
        }
    }
}

// 0x10024120 ?MakeFont@CMBFPSMonitor@@IAEXXZ
void CMBFPSMonitor::MakeFont(void)
{
    unsigned long fontID = MBHashString("Arial_14BoldRed");
    MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
    IMBStream *stream;
    mgr->GetStream(fontID, 'qsh ', &stream);
    if (stream) {
        mFont.CreateObject(MBIID_CMBFont);
        MBComPtr<IMBBitmapFont> bitmapFont(mFont);
        bitmapFont->LoadFont(fontID, stream);
        stream->Release();
    }
    mgr->GetStream(fontID, 'cmp ', &stream);
    if (stream) {
        MBComPtr<IMBBitmapFont> bitmapFont(mFont);
        bitmapFont->LoadCharacterMap(fontID, stream);
        stream->Release();
    }
}


// Matched by this code:
// 0x100242c0 _$E2
// 0x100242d0 _$E1
// 0x100243a0 _$E5
// 0x100243b0 _$E4
// 0x10024480 _$E8
// 0x10024490 _$E7
// 0x10024560 _$E11
// 0x10024570 _$E10
// 0x10024630 _$E14
// 0x10024640 _$E13
// 0x10024700 _$E17
// 0x10024710 _$E16
// 0x100247c0 _$E20
// 0x100247d0 _$E19
// 0x10024880 _$E23
// 0x10024890 _$E22
// 0x10024940 _$E26
// 0x10024950 _$E25
// 0x10024a10 _$E29
// 0x10024a20 _$E28
// 0x10024ae0 _$E32
// 0x10024af0 _$E31
// 0x10024ba0 _$E35
// 0x10024bb0 _$E34
// 0x10024c60 _$E38
// 0x10024c70 _$E37
// 0x10024d20 _$E41
// 0x10024d30 _$E40
// 0x10024de0 _$E44
// 0x10024df0 _$E43
// 0x10024ea0 _$E47
// 0x10024eb0 _$E46
// 0x10024f60 ?Release@?$MBComPtr@VIMBBitmapFont@@@@QAEXXZ
// 0x10024f80 _$E69
// 0x10024fa0 _$E68
// 0x10024fb0 ??4?$MBComPtr@VIMBBitmapFont@@@@QAEAAV0@PAVIMBBitmapFont@@@Z
// 0x10024fe0 ?QueryInterface@CMBFPSMonitor@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10024ff0 ?AddRef@CMBFPSMonitor@@W3AEKXZ
// 0x10025000 ?Release@CMBFPSMonitor@@W3AEKXZ
