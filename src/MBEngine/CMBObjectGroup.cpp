// CMBObjectGroup.cpp - MBEngine.dll CMBObjectGroup.obj
#include "CMBObjectGroup.h"

// 0x10025250 ?MakeCMBObjectGroup@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBObjectGroup(void)
{
    IMBUnknown *p = new CMBObjectGroup;
    if (p)
        p->AddRef();
    return p;
}

// 0x10025280 ??0CMBObjectGroup@@QAE@XZ
CMBObjectGroup::CMBObjectGroup(void)
    : mParent(0), mBackground(0), mCachedBoundsValid(false), mOffset(0, 0, 0),
      mHideCount(0), mDirty(true)
{
    _mRefCnt = 0;
    GetBounds();
}

// 0x10025360 ?QueryInterface@CMBObjectGroup@@UAEJABUMBGUID@@PAPAX@Z
long CMBObjectGroup::QueryInterface(const MBGUID &iid, void **obj)
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

// 0x10025430 ?AddRef@CMBObjectGroup@@UAEKXZ
unsigned long CMBObjectGroup::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10025440 ?Release@CMBObjectGroup@@UAEKXZ
unsigned long CMBObjectGroup::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10025470 ??1CMBObjectGroup@@QAE@XZ
CMBObjectGroup::~CMBObjectGroup(void)
{
    SetBackground(0);
    while (mObjects.begin() != mObjects.end())
        RemoveObject((*mObjects.begin()).second);
}

// 0x10025580 ?GetOffset@CMBObjectGroup@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBObjectGroup::GetOffset(void) const
{
    return mOffset;
}

// 0x100255a0 ?GetPosition@CMBObjectGroup@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBObjectGroup::GetPosition(void) const
{
    MBPoint3 pos = mOffset;
    if (mParent)
        pos += mParent->GetPosition();
    return pos;
}

// 0x100255f0 ?Place@CMBObjectGroup@@UAEXABVMBPoint3@@@Z
void CMBObjectGroup::Place(const MBPoint3 &point)
{
    SetDirty();
    mOffset = point;
}

// 0x10025620 ?Move@CMBObjectGroup@@UAEXABVMBPoint3@@@Z
void CMBObjectGroup::Move(const MBPoint3 &point)
{
    SetDirty();
    mOffset += point;
}

// 0x10025650 ?IsVisible@CMBObjectGroup@@UBE_NXZ
bool CMBObjectGroup::IsVisible(void) const
{
    return mHideCount == 0;
}

// 0x10025660 ?Show@CMBObjectGroup@@UAEJXZ
long CMBObjectGroup::Show(void)
{
    bool wasVisible = IsVisible();
    mHideCount--;
    if (wasVisible != IsVisible())
        SetDirty();
    return mHideCount;
}

// 0x10025690 ?Hide@CMBObjectGroup@@UAEJXZ
long CMBObjectGroup::Hide(void)
{
    bool wasVisible = IsVisible();
    mHideCount++;
    if (wasVisible != IsVisible())
        SetDirty();
    return mHideCount;
}

// 0x100256c0 ?GetParent@CMBObjectGroup@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBObjectGroup::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100256e0 ?SetParent@CMBObjectGroup@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBObjectGroup::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject(this);
    if (newParent)
        mParent = newParent;
    return oldParent;
}

// 0x10025710 ?IsDirty@CMBObjectGroup@@UBE_NXZ
bool CMBObjectGroup::IsDirty(void) const
{
    return mDirty;
}

// 0x10025720 ?SetDirty@CMBObjectGroup@@UAEXXZ
void CMBObjectGroup::SetDirty(void)
{
    mDirty = true;
}

// 0x10025730 ?ClearDirty@CMBObjectGroup@@UAEXXZ
void CMBObjectGroup::ClearDirty(void)
{
    mDirty = false;
}

// EQUIV 0x10025740 ?GetBounds@CMBObjectGroup@@UBE?AVMBRect@@XZ: reviewed: register choice and the order of the independent result stores only
MBRect CMBObjectGroup::GetBounds(void) const
{
    if (mCachedBoundsValid)
        return mCachedBounds;
    long count = mObjects.size();
    if (mBackground)
        count++;
    if (count == 0)
        return MBRect();
    MBRect bounds(0x7fffffff, 0x7fffffff, 0x80000000, 0x80000000);
    if (mBackground)
        bounds.Union(mBackground->GetBounds());
    for (MBObjectMap::const_iterator it = mObjects.begin(); it != mObjects.end(); ++it)
        bounds.Union((*it).second->GetBounds());
    if (&mCachedBounds != &bounds) // evidence: the original compares the addresses before the copy
        mCachedBounds = bounds;
    mCachedBoundsValid = true;
    return bounds;
}

// TODO 0x10025910 ?ClipRectsToObject@CMBObjectGroup@@ABE_NPAVIMBObject@@AAVMBRect@@1@Z: ClipRectsToObject: dest/src rect update order and 3 extra spill slots not reproduced; 5 compiles (53% similar)
bool CMBObjectGroup::ClipRectsToObject(IMBObject *obj, MBRect &srcRect, MBRect &destRect) const
{
    MBRect objRect = obj->GetBounds();
    MBRect groupBounds = GetBounds();
    objRect.Offset(-groupBounds.left, -groupBounds.top);
    MBRect clip;
    if (srcRect.Intersect(objRect, clip)) {
        destRect.top += clip.top - srcRect.top;
        destRect.left += clip.left - srcRect.left;
        destRect.SetSize(clip.GetSize());
        clip.Offset(-objRect.left, -objRect.top);
        srcRect = clip;
        return true;
    }
    return false;
}

// EQUIV 0x10025a70 ?Draw@CMBObjectGroup@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z: reviewed: register and slot choices only (the post-increment temporary in a register, the result in another slot). Was: Draw: post-increment temp kept in a register and result slot [esp+0x10] not reproduced; 6 compiles (69% similar)
long CMBObjectGroup::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    MBRect saveSrc = srcRect;
    MBRect saveDest = destRect;
    IMBObject *obj = mBackground;
    MBObjectMap::const_iterator it = mObjects.begin();
    long result = 0;
    do {
        if (obj && ClipRectsToObject(obj, srcRect, destRect)) {
            result = obj->Draw(dest, srcRect, destRect);
            srcRect = saveSrc;
            destRect = saveDest;
        }
        if (it == mObjects.end())
            break;
        obj = (*it).second;
        ++it;
    } while (result == 0);
    return result;
}

// 0x10025bc0 ?GetBackground@CMBObjectGroup@@UBEXPAPAVIMBObject@@@Z
void CMBObjectGroup::GetBackground(IMBObject **background) const
{
    IMBObject *p = mBackground;
    *background = p;
    if (p)
        p->AddRef();
}

// 0x10025be0 ?SetBackground@CMBObjectGroup@@UAEXPAVIMBObject@@@Z
void CMBObjectGroup::SetBackground(IMBObject *newBg)
{
    IMBObject *old = mBackground;
    if (old) {
        mBackground = 0;
        old->SetParent(0);
        old->Release();
    }
    if (newBg) {
        mBackground = newBg;
        newBg->AddRef();
        mBackground->SetParent(this);
    }
}

// TODO 0x10025c20 ?FindObject@CMBObjectGroup@@AAE?AViterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@PAVIMBObject@@@Z: FindObject: original inlines _Inc in the second (begin..end) loop, ours calls it in both; 6 compiles (71% similar)
MBObjectMap::iterator CMBObjectGroup::FindObject(IMBObject *obj)
{
    MBObjectMap::iterator it = mObjects.lower_bound(obj->GetPosition().z);
    MBObjectMap::iterator last = mObjects.upper_bound(obj->GetPosition().z);
    for (; it != last; ++it)
        if ((*it).second == obj)
            return it;
    for (it = mObjects.begin(); it != mObjects.end(); ++it) {
        if ((*it).second == obj) {
            mObjects.erase(it);
            return mObjects.insert(MBObjectMap::value_type(obj->GetPosition().z, obj));
        }
    }
    return mObjects.end();
}

// 0x10025d90 ?ObjectMoved@CMBObjectGroup@@UAEXPAVIMBObject@@@Z
void CMBObjectGroup::ObjectMoved(IMBObject *obj)
{
    MBObjectMap::iterator it = FindObject(obj);
    if (obj) {
        if (it != mObjects.end())
            mObjects.erase(it);
        mObjects.insert(MBObjectMap::value_type(obj->GetPosition().z, obj));
        mCachedBoundsValid = false;
    }
}

// 0x10025ea0 ?AddObject@CMBObjectGroup@@UAEXPAVIMBObject@@@Z
void CMBObjectGroup::AddObject(IMBObject *obj)
{
    if (obj) {
        mObjects.insert(MBObjectMap::value_type(obj->GetPosition().z, obj));
        obj->AddRef();
        obj->SetParent(this);
        mCachedBoundsValid = false;
    }
}

// 0x10025fb0 ?RemoveObject@CMBObjectGroup@@UAEXPAVIMBObject@@@Z
void CMBObjectGroup::RemoveObject(IMBObject *obj)
{
    if (obj) {
        if (obj == mBackground)
            SetBackground(0);
        MBObjectMap::iterator it = FindObject(obj);
        if (it != mObjects.end()) {
            mObjects.erase(it);
            obj->SetParent(0);
            obj->Release();
            mCachedBoundsValid = false;
        }
    }
}


// Matched by this code:
// 0x10026010 ?insert@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAE?AU?$pair@Viterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_4ad55c47
// 0x100262b0 ?erase@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x100267a0 ?_Erase@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10026800 ?_Insert@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAE?AViterator@12@PAU_Node@12@0ABU?$pair@$$CBJPAVIMBObject@@@2@@Z
// 0x10026ae0 ?_Lbound@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10026b30 ?_Lrotate@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10026b90 ?_Rrotate@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAEXPAU_Node@12@@Z
// 0x10026bf0 ?_Ubound@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IBEPAU_Node@12@ABJ@Z
// 0x10026c40 ?_Inc@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAEXXZ
// 0x10026cd0 ?_Buynode@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IAEPAU_Node@12@PAU312@W4_Redbl@12@@Z
// 0x10026cf0 ?_Dec@const_iterator@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@QAEXXZ
// 0x10026d90 ?_Construct@std@@YAXPAU?$pair@$$CBJPAVIMBObject@@@1@ABU21@@Z
// 0x10026db0 $E21
// 0x10026dd0 $E20
// steamboatapp.dll 0x1002e030 ?_Lbound@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IBEPAU_Node@12@ABJ@Z
// steamboatapp.dll 0x1002e0a0 ?_Ubound@?$_Tree@JU?$pair@$$CBJPAVIMBObject@@@std@@U_Kfn@?$multimap@JPAVIMBObject@@U?$less@J@std@@V?$MBAllocator@PAVIMBObject@@@@@2@U?$less@J@2@V?$MBAllocator@PAVIMBObject@@@@@std@@IBEPAU_Node@12@ABJ@Z
// steamboatapp.dll 0x1002e200 ?_Construct@std@@YAXPAU?$pair@$$CBJPAVIMBObject@@@1@ABU21@@Z
