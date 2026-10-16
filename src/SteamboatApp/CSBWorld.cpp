#include "CSBWorld.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBNew.h"

// 0x1002baa0 ?MakeCSBWorld@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBWorld()
{
    IMBUnknown *obj = (IMBWorld *)new CSBWorld;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1002bb30 ??0CSBWorld@@QAE@XZ
CSBWorld::CSBWorld() : mRoot(0)
{
    _mRefCnt = 0;
    mRoot = new SBObjectGroup;
    mRoot->AddRef();
}

// 0x1002bbf0 ?QueryInterface@CSBWorld@@UAEJABUMBGUID@@PAPAX@Z
long CSBWorld::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBWorld)
        p = (IMBWorld *)this;
    if (iid == MBIID_ISBWorld)
        p = (ISBWorld *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002bd00 ?AddRef@CSBWorld@@UAEKXZ
unsigned long CSBWorld::AddRef()
{
    return ++_mRefCnt;
}

// 0x1002bd20 ?Release@CSBWorld@@UAEKXZ
unsigned long CSBWorld::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002bd60 ??1CSBWorld@@QAE@XZ
CSBWorld::~CSBWorld()
{
    if (mRoot)
        mRoot->Release();
}

// 0x1002bd90 ?GetBounds@CSBWorld@@UBE?AVMBRect@@XZ
MBRect CSBWorld::GetBounds() const
{
    return mBounds;
}

// 0x1002bdc0 ?SetBounds@CSBWorld@@UAEXABVMBRect@@@Z
void CSBWorld::SetBounds(const MBRect &bounds)
{
    mBounds = bounds;
}

// EQUIV 0x1002be00 ?Draw@CSBWorld@@UAEJPAVIMBSurface@@AAVMBRect@@1@Z: reviewed: same operations (as MBEngine's
// CMBWorld::Draw, also open); the original keeps rootBounds.right/bottom in registers and loads
// worldRect first
long CSBWorld::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect)
{
    MBRect rootBounds = GetRoot().GetBounds();
    MBRect rootBlitRect;
    if (!worldRect.Intersect(rootBounds, rootBlitRect))
        return 0;
    destRect.left += rootBlitRect.left - worldRect.left;
    destRect.top += rootBlitRect.top - worldRect.top;
    destRect.SetSize(rootBlitRect.GetSize());
    rootBlitRect.Offset(-rootBounds.TopLeft());
    return mRoot->Draw(dest, rootBlitRect, destRect);
}

// 0x1002bfc0 ?GetRoot@CSBWorld@@UAEAAVIMBObjectGroup@@XZ
IMBObjectGroup &CSBWorld::GetRoot()
{
    return *mRoot;
}

// 0x1002bfe0 ?GetConstRoot@CSBWorld@@UBEABVIMBObjectGroup@@XZ
const IMBObjectGroup &CSBWorld::GetConstRoot() const
{
    return ((CSBWorld *)this)->GetRoot();
}

// 0x1002c000 ?GetNObjects@CSBWorld@@UAEKXZ
unsigned long CSBWorld::GetNObjects()
{
    return mRoot->GetNObjects();
}

// 0x1002c020 ?GetObjectA@CSBWorld@@UAE?AV?$MBComPtr@VIMBObject@@@@K@Z
MBComPtr<IMBObject> CSBWorld::GetObjectA(unsigned long index)
{
    return mRoot->GetObjectA(index);
}

// Adjustor thunks (emitted with the vftables):
// 0x1002e2b0 ?QueryInterface@CSBWorld@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1002e2c0 ?AddRef@CSBWorld@@W3AEKXZ
// 0x1002e2d0 ?Release@CSBWorld@@W3AEKXZ

// 0x1002e230 _$E27
// 0x1002e270 _$E23
// 0x1002e290 _$E26
