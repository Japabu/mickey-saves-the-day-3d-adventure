// CMBResourceStreamLocator.cpp - MBEngine.dll CMBResourceStreamLocator.obj
#include "CMBResourceStreamLocator.h"

// 0x1000f3f0 ?MakeCMBResourceStreamLocator@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBResourceStreamLocator(void)
{
    IMBUnknown *obj = new CMBResourceStreamLocator;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1000f420 ??0CMBResourceStreamLocator@@QAE@XZ
CMBResourceStreamLocator::CMBResourceStreamLocator(void)
    : mResID(0), mType(0)
{
    _mRefCnt = 0;
}

// 0x1000f440 ?QueryInterface@CMBResourceStreamLocator@@UAEJABUMBGUID@@PAPAX@Z
long CMBResourceStreamLocator::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBStreamLocator)
        p = this;
    if (iid == MBIID_IMBResourceStreamLocator)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1000f540 ?AddRef@CMBResourceStreamLocator@@UAEKXZ
unsigned long CMBResourceStreamLocator::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1000f550 ?Release@CMBResourceStreamLocator@@UAEKXZ
unsigned long CMBResourceStreamLocator::Release(void)
{
    unsigned long cnt = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return cnt;
}

// 0x1000f580 ??1CMBResourceStreamLocator@@QAE@XZ
CMBResourceStreamLocator::~CMBResourceStreamLocator(void)
{
}

// 0x1000f590 ?GetStream@CMBResourceStreamLocator@@UAEJPAPAVIMBStream@@@Z
long CMBResourceStreamLocator::GetStream(IMBStream **stream)
{
    MBComPtr<IMBResourceFileList> list(MBIID_CMBResourceMgr);
    if (list)
        list->GetStream(mResID, mType, stream);
    return 0;
}

// 0x1000f610 ?GetID@CMBResourceStreamLocator@@UAEKXZ
unsigned long CMBResourceStreamLocator::GetID(void)
{
    return mResID;
}

// 0x1000f620 ?SetResource@CMBResourceStreamLocator@@UAEJKK@Z
long CMBResourceStreamLocator::SetResource(unsigned long resID, unsigned long type)
{
    mResID = resID;
    mType = type;
    return 0;
}
