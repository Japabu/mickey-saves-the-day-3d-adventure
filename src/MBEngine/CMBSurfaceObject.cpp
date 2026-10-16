// CMBSurfaceObject.cpp - MBEngine.dll CMBSurfaceObject.obj
#include "CMBSurfaceObject.h"

// 0x10026de0 ?MakeCMBSurfaceObject@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBSurfaceObject(void)
{
    IMBUnknown *p = new CMBSurfaceObject;
    if (p)
        p->AddRef();
    return p;
}

// 0x10026e10 ??0CMBSurfaceObject@@QAE@XZ
CMBSurfaceObject::CMBSurfaceObject(void)
    : mParent(0), mHideCount(0), mDirty(true)
{
    _mRefCnt = 0;
}

// 0x10026e40 ?QueryInterface@CMBSurfaceObject@@UAEJABUMBGUID@@PAPAX@Z
long CMBSurfaceObject::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBObject)
        p = this;
    if (iid == MBIID_IMBSurfaceObject)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10026f40 ?AddRef@CMBSurfaceObject@@UAEKXZ
unsigned long CMBSurfaceObject::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10026f50 ?Release@CMBSurfaceObject@@UAEKXZ
unsigned long CMBSurfaceObject::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10026f80 ?GetOffset@CMBSurfaceObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBSurfaceObject::GetOffset(void) const
{
    return mPos;
}

// 0x10026fa0 ?GetPosition@CMBSurfaceObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBSurfaceObject::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x10027010 ?Place@CMBSurfaceObject@@UAEXABVMBPoint3@@@Z
void CMBSurfaceObject::Place(const MBPoint3 &point)
{
    mPos = point;
    SetDirty();
    if (mParent)
        mParent->ObjectMoved(this);
}

// 0x10027050 ?Move@CMBSurfaceObject@@UAEXABVMBPoint3@@@Z
void CMBSurfaceObject::Move(const MBPoint3 &point)
{
    mPos += point;
    SetDirty();
    if (mParent)
        mParent->ObjectMoved(this);
}

// 0x10027090 ?IsVisible@CMBSurfaceObject@@UBE_NXZ
bool CMBSurfaceObject::IsVisible(void) const
{
    return mHideCount == 0;
}

// 0x100270a0 ?IsDirty@CMBSurfaceObject@@UBE_NXZ
bool CMBSurfaceObject::IsDirty(void) const
{
    return mDirty;
}

// 0x100270b0 ?SetDirty@CMBSurfaceObject@@UAEXXZ
void CMBSurfaceObject::SetDirty(void)
{
    mDirty = true;
}

// 0x100270c0 ?ClearDirty@CMBSurfaceObject@@UAEXXZ
void CMBSurfaceObject::ClearDirty(void)
{
    mDirty = false;
}

// 0x100270d0 ??1CMBSurfaceObject@@IAE@XZ
CMBSurfaceObject::~CMBSurfaceObject(void)
{
}

// 0x100270f0 ?Hide@CMBSurfaceObject@@UAEJXZ
long CMBSurfaceObject::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x10027110 ?Show@CMBSurfaceObject@@UAEJXZ
long CMBSurfaceObject::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x10027140 ?GetParent@CMBSurfaceObject@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBSurfaceObject::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x10027160 ?SetParent@CMBSurfaceObject@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBSurfaceObject::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject(this);
    if (newParent)
        mParent = newParent;
    return oldParent;
}

// 0x10027190 ?GetSurface@CMBSurfaceObject@@UAEXPAPAVIMBSurface@@@Z
void CMBSurfaceObject::GetSurface(IMBSurface **surface)
{
    IMBSurface *p = mSurface;
    *surface = p;
    if (p)
        p->AddRef();
}

// 0x100271b0 ?SetSurface@CMBSurfaceObject@@UAEXPAVIMBSurface@@@Z
void CMBSurfaceObject::SetSurface(IMBSurface *surface)
{
    if (mSurface)
        mSurface.Release();
    mSurface = surface;
}

// 0x10027200 ?GetBounds@CMBSurfaceObject@@UBE?AVMBRect@@XZ
MBRect CMBSurfaceObject::GetBounds(void) const
{
    MBPoint3 pos = GetPosition();
    long width, height;
    if (mSurface) {
        width = mSurface->GetWidth();
        height = mSurface->GetHeight();
    } else {
        width = 0;
        height = 0;
    }
    return MBRect(pos, MBSize(width, height));
}

// 0x10027260 ?Draw@CMBSurfaceObject@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBSurfaceObject::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const
{
    if (IsVisible() && mSurface)
        return mSurface->Blit(dest, &worldRect, &destRect, 1);
    return 0;
}


// Matched by this code:
// 0x100272a0 _$E2
// 0x100272b0 _$E1
// 0x10027380 _$E5
// 0x10027390 _$E4
// 0x10027460 _$E8
// 0x10027470 _$E7
// 0x10027540 _$E11
// 0x10027550 _$E10
// 0x10027610 _$E14
// 0x10027620 _$E13
// 0x100276e0 _$E17
// 0x100276f0 _$E16
// 0x100277a0 _$E20
// 0x100277b0 _$E19
// 0x10027860 _$E23
// 0x10027870 _$E22
// 0x10027920 _$E26
// 0x10027930 _$E25
// 0x100279f0 _$E29
// 0x10027a00 _$E28
// 0x10027ac0 _$E32
// 0x10027ad0 _$E31
// 0x10027b80 _$E35
// 0x10027b90 _$E34
// 0x10027c40 _$E38
// 0x10027c50 _$E37
// 0x10027d00 _$E41
// 0x10027d10 _$E40
// 0x10027dc0 _$E44
// 0x10027dd0 _$E43
// 0x10027e80 _$E47
// 0x10027e90 _$E46
