// MBEngine.dll .\MBEngineRelease\CMBSoundDescription.obj
#include "CMBSoundDescription.h"

// 0x10046f30 ?MakeCMBSoundDescription@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBSoundDescription(void)
{
    IMBUnknown *p = new CMBSoundDescription;
    if (p)
        p->AddRef();
    return p;
}

// 0x10046f60 ??0CMBSoundDescription@@QAE@XZ
CMBSoundDescription::CMBSoundDescription(void)
    : mStreamLocator(0), mChannel(-1), mLoopCount(1), mStreaming(false), mRateMultiplier(1.0f)
{
    _mRefCnt = 0;
}

// 0x10046f90 ?QueryInterface@CMBSoundDescription@@UAEJABUMBGUID@@PAPAX@Z
long CMBSoundDescription::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBUnknown *)this;
    if (iid == MBIID_IMBSoundDescription)
        p = (IMBSoundDescription *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10047060 ?AddRef@CMBSoundDescription@@UAEKXZ
unsigned long CMBSoundDescription::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10047070 ?Release@CMBSoundDescription@@UAEKXZ
unsigned long CMBSoundDescription::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x100470a0 ??1CMBSoundDescription@@QAE@XZ
CMBSoundDescription::~CMBSoundDescription(void)
{
    if (mStreamLocator)
        mStreamLocator->Release();
}

// 0x100470c0 ?SetStreamLocator@CMBSoundDescription@@UAEXPAVIMBStreamLocator@@@Z
void CMBSoundDescription::SetStreamLocator(IMBStreamLocator *streamLocator)
{
    IMBStreamLocator *old = mStreamLocator;
    mStreamLocator = streamLocator;
    if (streamLocator)
        streamLocator->AddRef();
    if (old)
        old->Release();
}

// 0x100470f0 ?GetStreamLocator@CMBSoundDescription@@UAEXPAPAVIMBStreamLocator@@@Z
void CMBSoundDescription::GetStreamLocator(IMBStreamLocator **streamLocator)
{
    *streamLocator = mStreamLocator;
    if (mStreamLocator)
        mStreamLocator->AddRef();
}

// 0x10047110 ?GetSoundID@CMBSoundDescription@@UBEKXZ
unsigned long CMBSoundDescription::GetSoundID(void) const
{
    unsigned long id = 0;
    if (mStreamLocator)
        id = mStreamLocator->GetID();
    return id;
}

// 0x10047120 ?SetChannel@CMBSoundDescription@@UAEXJ@Z
void CMBSoundDescription::SetChannel(long channel)
{
    mChannel = channel;
}

// 0x10047130 ?GetChannel@CMBSoundDescription@@UBEJXZ
long CMBSoundDescription::GetChannel(void) const
{
    return mChannel;
}

// 0x10047140 ?SetLoopCount@CMBSoundDescription@@UAEXJ@Z
void CMBSoundDescription::SetLoopCount(long loopCount)
{
    mLoopCount = loopCount;
}

// 0x10047150 ?GetLoopCount@CMBSoundDescription@@UBEJXZ
long CMBSoundDescription::GetLoopCount(void) const
{
    return mLoopCount;
}

// 0x10047160 ?SetStreaming@CMBSoundDescription@@UAEX_N@Z
void CMBSoundDescription::SetStreaming(bool isStreaming)
{
    mStreaming = isStreaming;
}

// 0x10047170 ?IsStreaming@CMBSoundDescription@@UBE_NXZ
bool CMBSoundDescription::IsStreaming(void) const
{
    return mStreaming;
}

// 0x10047180 ?SetRateMultiplier@CMBSoundDescription@@UAEXM@Z
void CMBSoundDescription::SetRateMultiplier(float mult)
{
    mRateMultiplier = mult;
}

// 0x10047190 ?GetRateMultiplier@CMBSoundDescription@@UBEMXZ
float CMBSoundDescription::GetRateMultiplier(void) const
{
    return mRateMultiplier;
}
