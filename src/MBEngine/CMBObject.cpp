// CMBObject.cpp - MBEngine.dll CMBObject.obj
#include "CMBObject.h"

// 0x10025010 ??0CMBObject@@QAE@XZ
CMBObject::CMBObject(void)
    : mParent(0), mPosition(0, 0, 100), mHideCount(0), mDirty(true)
{
}

// 0x10025040 ??1CMBObject@@QAE@XZ
CMBObject::~CMBObject(void)
{
}

// 0x10025050 ?GetOffset@CMBObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBObject::GetOffset(void) const
{
    return mPosition;
}

// 0x10025070 ?GetPosition@CMBObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBObject::GetPosition(void) const
{
    if (mParent)
        return mPosition + mParent->GetPosition();
    return mPosition;
}

// 0x100250e0 ?Place@CMBObject@@UAEXABVMBPoint3@@@Z
void CMBObject::Place(const MBPoint3 &point)
{
    mPosition = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10025120 ?Move@CMBObject@@UAEXABVMBPoint3@@@Z
void CMBObject::Move(const MBPoint3 &point)
{
    mPosition.x += point.x;
    mPosition.y += point.y;
    mPosition.z += point.z;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10025160 ?IsVisible@CMBObject@@UBE_NXZ
bool CMBObject::IsVisible(void) const
{
    return mHideCount == 0;
}

// 0x10025170 ?Show@CMBObject@@UAEJXZ
long CMBObject::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x100251a0 ?Hide@CMBObject@@UAEJXZ
long CMBObject::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x100251c0 ?IsDirty@CMBObject@@UBE_NXZ
bool CMBObject::IsDirty(void) const
{
    return mDirty;
}

// 0x100251d0 ?SetDirty@CMBObject@@UAEXXZ
void CMBObject::SetDirty(void)
{
    mDirty = true;
}

// 0x100251e0 ?ClearDirty@CMBObject@@UAEXXZ
void CMBObject::ClearDirty(void)
{
    mDirty = false;
}

// 0x100251f0 ?GetParent@CMBObject@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBObject::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x10025210 ?SetParent@CMBObject@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBObject::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}
