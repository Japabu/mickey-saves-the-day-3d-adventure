// CMBWorld.cpp - MBEngine.dll CMBWorld.obj
#include "CMBWorld.h"

// 0x1002c900 ?MakeCMBWorld@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBWorld(void)
{
    IMBUnknown *obj = new CMBWorld;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1002c930 ??0CMBWorld@@QAE@XZ
CMBWorld::CMBWorld(void)
    : mRoot(0)
{
    _mRefCnt = 0;
}

// 0x1002c950 ?QueryInterface@CMBWorld@@UAEJABUMBGUID@@PAPAX@Z
long CMBWorld::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBWorld)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002c9d0 ?AddRef@CMBWorld@@UAEKXZ
unsigned long CMBWorld::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002c9e0 ?Release@CMBWorld@@UAEKXZ
unsigned long CMBWorld::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002ca10 ??1CMBWorld@@QAE@XZ
CMBWorld::~CMBWorld(void)
{
    if (mRoot)
        mRoot->Release();
}

// 0x1002ca30 ?GetBounds@CMBWorld@@UBE?AVMBRect@@XZ
MBRect CMBWorld::GetBounds(void) const
{
    return mBounds;
}

// 0x1002ca50 ?SetBounds@CMBWorld@@UAEXABVMBRect@@@Z
void CMBWorld::SetBounds(const MBRect &bounds)
{
    mBounds = bounds;
}

// TODO 0x1002ca80 ?Draw@CMBWorld@@UAEJPAVIMBSurface@@AAVMBRect@@1@Z: Draw: inline MBRect::Intersect/SetSize shape right, register allocation differs (worldRect ptr/rb.bottom swap), 343 vs 345 bytes (85% similar)
long CMBWorld::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect)
{
    MBRect rootBounds = GetRoot().GetBounds();
    MBRect rootBlitRect;
    if (!worldRect.Intersect(rootBounds, rootBlitRect))
        return 0;
    destRect.Offset(rootBlitRect.left - worldRect.left, rootBlitRect.top - worldRect.top);
    destRect.SetSize(rootBlitRect.GetSize());
    rootBlitRect.Offset(-rootBounds.left, -rootBounds.top);
    return mRoot->Draw(dest, rootBlitRect, destRect);
}

// 0x1002cbe0 ?GetRoot@CMBWorld@@UAEAAVIMBObjectGroup@@XZ
IMBObjectGroup &CMBWorld::GetRoot(void)
{
    if (!mRoot) {
        IMBUnknown *unknown;
        MBComCreateInstance(0, MBIID_CMBObjectGroup, &unknown);
        if (unknown) {
            unknown->QueryInterface(MBIID_IMBObjectGroup, (void **)&mRoot);
            unknown->Release();
        }
    }
    return *mRoot;
}

// 0x1002cc20 ?GetConstRoot@CMBWorld@@UBEABVIMBObjectGroup@@XZ
const IMBObjectGroup &CMBWorld::GetConstRoot(void) const
{
    return ((CMBWorld *)this)->GetRoot();
}
