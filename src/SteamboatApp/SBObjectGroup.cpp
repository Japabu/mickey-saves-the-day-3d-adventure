#include "SBObjectGroup.h"
#include <limits.h>

// 0x1002c060 ??0SBObjectGroup@@QAE@XZ
SBObjectGroup::SBObjectGroup()
    : mParent(0), mBackground(0), mCachedBoundsValid(false), mHideCount(0), mDirty(true)
{
    _mRefCnt = 0;
    GetBounds();
}

// 0x1002c1a0 ?QueryInterface@SBObjectGroup@@UAEJABUMBGUID@@PAPAX@Z
long SBObjectGroup::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBObject)
        p = this;
    if (iid == MBIID_IMBObjectGroup)
        p = this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1002c2b0 ?AddRef@SBObjectGroup@@UAEKXZ
unsigned long SBObjectGroup::AddRef()
{
    return ++_mRefCnt;
}

// 0x1002c5e0 ?IsVisible@SBObjectGroup@@UBE_NXZ
bool SBObjectGroup::IsVisible() const
{
    return mHideCount == 0;
}

// 0x1002c6f0 ?IsDirty@SBObjectGroup@@UBE_NXZ
bool SBObjectGroup::IsDirty() const
{
    return mDirty;
}

// 0x1002c710 ?SetDirty@SBObjectGroup@@UAEXXZ
void SBObjectGroup::SetDirty()
{
    mDirty = true;
}

// 0x1002c730 ?ClearDirty@SBObjectGroup@@UAEXXZ
void SBObjectGroup::ClearDirty()
{
    mDirty = false;
}

// 0x1002c2d0 ?Release@SBObjectGroup@@UAEKXZ
unsigned long SBObjectGroup::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1002c3b0 ??1SBObjectGroup@@QAE@XZ
SBObjectGroup::~SBObjectGroup()
{
    SetBackground(0);
    ObjectMap::iterator it;
    while ((it = mObjects.begin()) != mObjects.end())
        RemoveObject((*it).second);
}

// 0x1002c4c0 ?GetOffset@SBObjectGroup@@UBE?AVMBPoint3@@XZ
MBPoint3 SBObjectGroup::GetOffset() const
{
    return mOffset;
}

// 0x1002c4f0 ?GetPosition@SBObjectGroup@@UBE?AVMBPoint3@@XZ
MBPoint3 SBObjectGroup::GetPosition() const
{
    MBPoint3 position = mOffset;
    if (mParent)
        position += mParent->GetPosition();
    return position;
}

// 0x1002c560 ?Place@SBObjectGroup@@UAEXABVMBPoint3@@@Z
void SBObjectGroup::Place(const MBPoint3 &point)
{
    SetDirty();
    mOffset = point;
}

// 0x1002c5a0 ?Move@SBObjectGroup@@UAEXABVMBPoint3@@@Z
void SBObjectGroup::Move(const MBPoint3 &point)
{
    SetDirty();
    mOffset += point;
}

// 0x1002c600 ?Show@SBObjectGroup@@UAEJXZ
long SBObjectGroup::Show()
{
    bool wasVisible = IsVisible();
    --mHideCount;
    if (wasVisible != IsVisible())
        SetDirty();
    return mHideCount;
}

// 0x1002c640 ?Hide@SBObjectGroup@@UAEJXZ
long SBObjectGroup::Hide()
{
    bool wasVisible = IsVisible();
    ++mHideCount;
    if (wasVisible != IsVisible())
        SetDirty();
    return mHideCount;
}

// 0x1002c680 ?GetParent@SBObjectGroup@@UBEXPAPAVIMBObjectGroup@@@Z
void SBObjectGroup::GetParent(IMBObjectGroup **parent) const
{
    *parent = mParent;
    if (mParent)
        mParent->AddRef();
}

// 0x1002c6b0 ?SetParent@SBObjectGroup@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *SBObjectGroup::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject(this);
    if (newParent)
        mParent = newParent;
    return oldParent;
}

// 0x1002cc80 ?GetBackground@SBObjectGroup@@UBEXPAPAVIMBObject@@@Z
void SBObjectGroup::GetBackground(IMBObject **background) const
{
    *background = mBackground;
    if (mBackground)
        mBackground->AddRef();
}

// 0x1002ccb0 ?SetBackground@SBObjectGroup@@UAEXPAVIMBObject@@@Z
void SBObjectGroup::SetBackground(IMBObject *newBg)
{
    if (mBackground) {
        IMBObject *oldBg = mBackground;
        mBackground = 0;
        oldBg->SetParent(0);
        oldBg->Release();
    }
    if (newBg) {
        mBackground = newBg;
        newBg->AddRef();
        newBg->SetParent(this);
    }
}

// 0x1002c750 ?GetBounds@SBObjectGroup@@UBE?AVMBRect@@XZ
MBRect SBObjectGroup::GetBounds() const
{
    if (mCachedBoundsValid)
        return mCachedBounds;
    unsigned long count = mObjects.size();
    if (mBackground)
        count++;
    if (count == 0)
        return MBRect();
    MBRect rect(LONG_MAX, LONG_MAX, LONG_MIN, LONG_MIN);
    if (mBackground)
        rect.Grow(mBackground->GetBounds());
    for (ObjectMap::const_iterator i = mObjects.begin(); i != mObjects.end(); ++i)
        rect.Grow((*i).second->GetBounds());
    ((SBObjectGroup *)this)->mCachedBounds = rect;
    ((SBObjectGroup *)this)->mCachedBoundsValid = true;
    return rect;
}

// 0x1002cd20 ?FindObject@SBObjectGroup@@AAE?AViterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@PAVIMBObject@@@Z
SBObjectGroup::ObjectMap::iterator SBObjectGroup::FindObject(IMBObject *obj)
{
    // Look among the objects at the object's depth first.
    ObjectMap::iterator iter = mObjects.lower_bound(obj->GetPosition().z);
    ObjectMap::iterator last = mObjects.upper_bound(obj->GetPosition().z);
    for (; iter != last; ++iter)
        if ((*iter).second == obj)
            return iter;
    // The object moved without telling us: find it anywhere and file it under its new depth.
    for (iter = mObjects.begin(); iter != mObjects.end(); ++iter)
        if ((*iter).second == obj) {
            mObjects.erase(iter);
            return mObjects.insert(ObjectMap::value_type(obj->GetPosition().z, obj));
        }
    return mObjects.end();
}

// TODO 0x1002c9a0 ?ClipRectsToObject@SBObjectGroup@@ABE_NPAVIMBObject@@AAVMBRect@@1@Z: same operations; the
// original keeps objBlitRect in registers and schedules the Offset with x first (Offset(point) or
// (dx, dy), += or Offset for destRect tried)
bool SBObjectGroup::ClipRectsToObject(IMBObject *obj, MBRect &srcRect, MBRect &destRect) const
{
    MBRect objBounds = obj->GetBounds();
    objBounds.Offset(-GetBounds().TopLeft());
    MBRect objBlitRect;
    if (!srcRect.Intersect(objBounds, objBlitRect))
        return false;
    destRect.left += objBlitRect.left - srcRect.left;
    destRect.top += objBlitRect.top - srcRect.top;
    destRect.SetSize(objBlitRect.GetSize());
    srcRect.SetRect(objBlitRect);
    srcRect.Offset(-objBounds.TopLeft());
    return true;
}

// 0x1002cb60 ?Draw@SBObjectGroup@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long SBObjectGroup::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    MBRect safeSrcRect = srcRect;
    MBRect safeDestRect = destRect;
    ObjectMap::const_iterator i = mObjects.begin();
    IMBObject *obj = mBackground;
    long err = 0;
    do {
        if (obj && ClipRectsToObject(obj, srcRect, destRect)) {
            err = obj->Draw(dest, srcRect, destRect);
            srcRect.SetRect(safeSrcRect);
            destRect.SetRect(safeDestRect);
        }
        if (i == mObjects.end())
            break;
        obj = (*i++).second;
    } while (err == 0);
    return err;
}

// 0x1002ce80 ?ObjectMoved@SBObjectGroup@@UAEXPAVIMBObject@@@Z
void SBObjectGroup::ObjectMoved(IMBObject *obj)
{
    ObjectMap::iterator it = FindObject(obj);
    if (obj) {
        if (it != mObjects.end())
            mObjects.erase(it);
        mObjects.insert(ObjectMap::value_type(obj->GetPosition().z, obj));
        mCachedBoundsValid = false;
    }
}

// 0x1002cf10 ?AddObject@SBObjectGroup@@UAEXPAVIMBObject@@@Z
void SBObjectGroup::AddObject(IMBObject *obj)
{
    if (obj) {
        mObjects.insert(ObjectMap::value_type(obj->GetPosition().z, obj));
        obj->AddRef();
        obj->SetParent(this);
        mCachedBoundsValid = false;
    }
}

// 0x1002cf90 ?RemoveObject@SBObjectGroup@@UAEXPAVIMBObject@@@Z
void SBObjectGroup::RemoveObject(IMBObject *obj)
{
    if (obj) {
        if (obj == mBackground)
            SetBackground(0);
        ObjectMap::iterator it = FindObject(obj);
        if (it != mObjects.end()) {
            mObjects.erase(it);
            obj->SetParent(0);
            obj->Release();
            mCachedBoundsValid = false;
        }
    }
}

// 0x1002d030 ?GetObjectA@SBObjectGroup@@UAE?AV?$MBComPtr@VIMBObject@@@@K@Z
MBComPtr<IMBObject> SBObjectGroup::GetObjectA(unsigned long index)
{
    if (index < GetNObjects()) {
        if (index == 0 && mBackground)
            return MBComPtr<IMBObject>(mBackground);
        unsigned long n = index;
        if (mBackground)
            n = index - 1;
        ObjectMap::iterator i = mObjects.begin();
        while (n--)
            ++i;
        return MBComPtr<IMBObject>((*i).second);
    }
    return MBComPtr<IMBObject>();
}

// 0x1002d010 ?GetNObjects@SBObjectGroup@@UAEKXZ
unsigned long SBObjectGroup::GetNObjects()
{
    return mObjects.size() + (mBackground != 0);
}

// The object list's template instances (emitted by the code above):
// 0x1002c310 ??1?$MBMultimap@JPAVIMBObject@@U?$less@J@std@@@@QAE@XZ
// 0x1002d340 ?erase@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x1002d980 ?erase@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAE?AViterator@12@V312@0@Z
// 0x1002db00 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAEXXZ
// 0x1002dca0 ?_Insert@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJPAVIMBObject@@@2@@Z
// 0x1002e110 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAEXXZ
// 0x1002d1c0 ?insert@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@_N@2@ABU?$pair@$$CBJPAVIMBObject@@@2@@Z
// 0x1002dbe0 ?_Erase@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAEXPAU_Node@12@@Z
