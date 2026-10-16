// CMBColorBackgroundObject.cpp - MBEngine.dll CMBColorBackgroundObject.obj
#include "CMBColorBackgroundObject.h"

// 0x10020dc0 ?MakeCMBColorBackgroundObject@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBColorBackgroundObject(void)
{
    IMBUnknown *p = new CMBColorBackgroundObject;
    if (p)
        p->AddRef();
    return p;
}

// 0x10020df0 ??0CMBColorBackgroundObject@@QAE@XZ
CMBColorBackgroundObject::CMBColorBackgroundObject(void)
    : mParent(0), mPos(0, 0, 100), mHideCount(0), mDirty(true), mColor(kBlack)
{
    _mRefCnt = 0;
}

// 0x10020e30 ?QueryInterface@CMBColorBackgroundObject@@UAEJABUMBGUID@@PAPAX@Z
long CMBColorBackgroundObject::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (iid == MBIID_IMBObject)
        p = this;
    if (iid == MBIID_IMBColorBackgroundObject)
        p = this;
    if (iid == MBIID_IMBUnknown)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10020f70 ?AddRef@CMBColorBackgroundObject@@UAEKXZ
unsigned long CMBColorBackgroundObject::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10020f80 ?Release@CMBColorBackgroundObject@@UAEKXZ
unsigned long CMBColorBackgroundObject::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10020fb0 ?GetOffset@CMBColorBackgroundObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBColorBackgroundObject::GetOffset(void) const
{
    return mPos;
}

// 0x10020fd0 ?GetPosition@CMBColorBackgroundObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBColorBackgroundObject::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x10021040 ?Place@CMBColorBackgroundObject@@UAEXABVMBPoint3@@@Z
void CMBColorBackgroundObject::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10021080 ?Move@CMBColorBackgroundObject@@UAEXABVMBPoint3@@@Z
void CMBColorBackgroundObject::Move(const MBPoint3 &point)
{
    mPos += point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x100210c0 ?IsVisible@CMBColorBackgroundObject@@UBE_NXZ
bool CMBColorBackgroundObject::IsVisible(void) const
{
    return mHideCount == 0;
}

// 0x100210d0 ?IsDirty@CMBColorBackgroundObject@@UBE_NXZ
bool CMBColorBackgroundObject::IsDirty(void) const
{
    return mDirty;
}

// 0x100210e0 ?SetDirty@CMBColorBackgroundObject@@UAEXXZ
void CMBColorBackgroundObject::SetDirty(void)
{
    mDirty = true;
}

// 0x100210f0 ?ClearDirty@CMBColorBackgroundObject@@UAEXXZ
void CMBColorBackgroundObject::ClearDirty(void)
{
    mDirty = false;
}

// 0x10021100 ??1CMBColorBackgroundObject@@IAE@XZ
CMBColorBackgroundObject::~CMBColorBackgroundObject(void)
{
}

// 0x10021110 ?GetBounds@CMBColorBackgroundObject@@UBE?AVMBRect@@XZ
MBRect CMBColorBackgroundObject::GetBounds(void) const
{
    return MBRect();
}

// 0x10021130 ?Hide@CMBColorBackgroundObject@@UAEJXZ
long CMBColorBackgroundObject::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x10021150 ?Show@CMBColorBackgroundObject@@UAEJXZ
long CMBColorBackgroundObject::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x10021180 ?GetParent@CMBColorBackgroundObject@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBColorBackgroundObject::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100211a0 ?SetParent@CMBColorBackgroundObject@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBColorBackgroundObject::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x100211e0 ?Draw@CMBColorBackgroundObject@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBColorBackgroundObject::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const
{
    return dest->ColorFill(mColor, 0);
}

// 0x10021200 ?SetColor@CMBColorBackgroundObject@@UAEXABVMBColor@@@Z
void CMBColorBackgroundObject::SetColor(const MBColor &color)
{
    mColor = color;
}

// 0x10021220 ?GetColor@CMBColorBackgroundObject@@UAEXAAVMBColor@@@Z
void CMBColorBackgroundObject::GetColor(MBColor &color)
{
    color = mColor;
}


// Matched by this code:
// 0x10021240 _$E2
// 0x10021250 _$E1
// 0x10021320 _$E5
// 0x10021330 _$E4
// 0x10021400 _$E8
// 0x10021410 _$E7
// 0x100214e0 _$E11
// 0x100214f0 _$E10
// 0x100215b0 _$E14
// 0x100215c0 _$E13
// 0x10021680 _$E17
// 0x10021690 _$E16
// 0x10021740 _$E20
// 0x10021750 _$E19
// 0x10021800 _$E23
// 0x10021810 _$E22
// 0x100218c0 _$E26
// 0x100218d0 _$E25
// 0x10021990 _$E29
// 0x100219a0 _$E28
// 0x10021a60 _$E32
// 0x10021a70 _$E31
// 0x10021b20 _$E35
// 0x10021b30 _$E34
// 0x10021be0 _$E38
// 0x10021bf0 _$E37
// 0x10021ca0 _$E41
// 0x10021cb0 _$E40
// 0x10021d60 _$E44
// 0x10021d70 _$E43
// 0x10021e20 _$E47
// 0x10021e30 _$E46
