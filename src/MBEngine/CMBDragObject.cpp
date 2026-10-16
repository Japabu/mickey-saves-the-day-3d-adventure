// CMBDragObject.cpp - MBEngine.dll CMBDragObject.obj
#include "CMBDragObject.h"

// 0x10021ee0 ?MakeCMBDragObject@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBDragObject(void)
{
    IMBUnknown *p = (IMBDragObject *)new CMBDragObject;
    if (p)
        p->AddRef();
    return p;
}

// 0x10021f10 ??0CMBDragObject@@QAE@XZ
CMBDragObject::CMBDragObject(void)
    : mParent(0), mOffset(0, 0, 0), mIsTrackingClick(false)
{
    _mRefCnt = 0;
    mBroadcastChannel = 0;
    mPickUpEvent = -1;
    mDropEvent = -1;
    mBoundingBox = MBRect(-10000, -10000, 10000, 10000);
    mPixelTest = false;
    mZAdjust = 1;
    mIsActive = true;
}

// 0x10021fd0 ?QueryInterface@CMBDragObject@@UAEJABUMBGUID@@PAPAX@Z
long CMBDragObject::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBDragObject *)this;
    if (iid == MBIID_IMBDragObject)
        p = (IMBDragObject *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBDragObject *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (iid == MBIID_IMBObjectGroup)
        p = (IMBObjectGroup *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10022160 ?AddRef@CMBDragObject@@UAEKXZ
unsigned long CMBDragObject::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x10022170 ?Release@CMBDragObject@@UAEKXZ
unsigned long CMBDragObject::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}

// 0x100221a0 ?IsRolledOver@CMBDragObject@@UBE_NABVMBPoint@@@Z
bool CMBDragObject::IsRolledOver(const MBPoint &world_mouse_pos) const
{
    return false;
}

// 0x100221b0 ?EnterRollover@CMBDragObject@@UAEXABVMBPoint@@@Z
void CMBDragObject::EnterRollover(const MBPoint &world_mouse_pos)
{
}

// 0x100221c0 ?WithinRollover@CMBDragObject@@UAEXABVMBPoint@@@Z
void CMBDragObject::WithinRollover(const MBPoint &world_mouse_pos)
{
}

// 0x100221d0 ?LeaveRollover@CMBDragObject@@UAEXXZ
void CMBDragObject::LeaveRollover(void)
{
}

// 0x100221e0 ?IsActive@CMBDragObject@@UBE_NXZ
bool CMBDragObject::IsActive(void) const
{
    return mIsActive;
}

// 0x100221f0 ?Activate@CMBDragObject@@UAEXXZ
void CMBDragObject::Activate(void)
{
    mIsActive = true;
}

// 0x10022200 ?Deactivate@CMBDragObject@@UAEXXZ
void CMBDragObject::Deactivate(void)
{
    mIsActive = false;
}

// 0x10022210 ?SetPixelTest@CMBDragObject@@UAEX_N@Z
void CMBDragObject::SetPixelTest(bool set)
{
    mPixelTest = set;
}

// 0x10022220 ?GetPixelTest@CMBDragObject@@UAE_NXZ
bool CMBDragObject::GetPixelTest(void)
{
    return mPixelTest;
}

// 0x10022230 ?SetBroadcastChannel@CMBDragObject@@UAEXJ@Z
void CMBDragObject::SetBroadcastChannel(long channel)
{
    mBroadcastChannel = channel;
}

// 0x10022240 ?GetBroadcastChannel@CMBDragObject@@UAEJXZ
long CMBDragObject::GetBroadcastChannel(void)
{
    return mBroadcastChannel;
}

// 0x10022250 ?SetPickUpMessage@CMBDragObject@@UAEXJ@Z
void CMBDragObject::SetPickUpMessage(long message)
{
    mPickUpEvent = message;
}

// 0x10022260 ?GetPickUpMessage@CMBDragObject@@UAEJXZ
long CMBDragObject::GetPickUpMessage(void)
{
    return mPickUpEvent;
}

// 0x10022270 ?SetDropMessage@CMBDragObject@@UAEXJ@Z
void CMBDragObject::SetDropMessage(long message)
{
    mDropEvent = message;
}

// 0x10022280 ?GetDropMessage@CMBDragObject@@UAEJXZ
long CMBDragObject::GetDropMessage(void)
{
    return mDropEvent;
}

// 0x10022290 ?SetObject@CMBDragObject@@UAEXPAVIMBObject@@@Z
void CMBDragObject::SetObject(IMBObject *object)
{
    mObject = object;
}

// 0x100222c0 ?GetObject@CMBDragObject@@UAEXPAPAVIMBObject@@@Z
void CMBDragObject::GetObject(IMBObject **object)
{
    IMBObject *o = mObject;
    *object = o;
    o->AddRef();
}

// 0x100222e0 ?SetBoundingBox@CMBDragObject@@UAEXVMBRect@@@Z
void CMBDragObject::SetBoundingBox(MBRect bb)
{
    mBoundingBox = bb;
}

// 0x10022310 ?GetBoundingBox@CMBDragObject@@UAE?AVMBRect@@XZ
MBRect CMBDragObject::GetBoundingBox(void)
{
    return mBoundingBox;
}

// 0x10022330 ?SetZAdjustment@CMBDragObject@@UAEXJ@Z
void CMBDragObject::SetZAdjustment(long dz)
{
    mZAdjust = dz;
}

// 0x10022340 ?GetZAdjustment@CMBDragObject@@UAEJXZ
long CMBDragObject::GetZAdjustment(void)
{
    return mZAdjust;
}

// 0x10022350 ??1CMBDragObject@@QAE@XZ
CMBDragObject::~CMBDragObject(void)
{
    ClearSnapToPoints();
}

// 0x100223c0 ?GetOffset@CMBDragObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBDragObject::GetOffset(void) const
{
    if (mObject)
        return mObject->GetOffset();
    return mOffset;
}

// 0x10022420 ?GetPosition@CMBDragObject@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBDragObject::GetPosition(void) const
{
    MBPoint3 pos = GetOffset();
    if (mParent)
        pos += mParent->GetPosition();
    return pos;
}

// 0x10022480 ?Place@CMBDragObject@@UAEXABVMBPoint3@@@Z
void CMBDragObject::Place(const MBPoint3 &point)
{
    if (mObject)
        mObject->Place(point);
    mOffset = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x100224d0 ?Move@CMBDragObject@@UAEXABVMBPoint3@@@Z
void CMBDragObject::Move(const MBPoint3 &point)
{
    if (mObject)
        mObject->Move(point);
    mOffset += point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10022530 ?GetBounds@CMBDragObject@@UBE?AVMBRect@@XZ
MBRect CMBDragObject::GetBounds(void) const
{
    if (mObject)
        return mObject->GetBounds();
    return MBRect();
}

// 0x10022580 ?IsVisible@CMBDragObject@@UBE_NXZ
bool CMBDragObject::IsVisible(void) const
{
    if (mObject)
        return mObject->IsVisible();
    return false;
}

// 0x10022590 ?Show@CMBDragObject@@UAEJXZ
long CMBDragObject::Show(void)
{
    if (mObject)
        return mObject->Show();
    return 0;
}

// 0x100225a0 ?Hide@CMBDragObject@@UAEJXZ
long CMBDragObject::Hide(void)
{
    if (mObject)
        return mObject->Hide();
    return 0;
}

// 0x100225b0 ?IsDirty@CMBDragObject@@UBE_NXZ
bool CMBDragObject::IsDirty(void) const
{
    if (mObject)
        return mObject->IsDirty();
    return false;
}

// 0x100225c0 ?SetDirty@CMBDragObject@@UAEXXZ
void CMBDragObject::SetDirty(void)
{
    if (mObject)
        mObject->SetDirty();
}

// 0x100225d0 ?ClearDirty@CMBDragObject@@UAEXXZ
void CMBDragObject::ClearDirty(void)
{
    if (mObject)
        mObject->ClearDirty();
}

// 0x100225e0 ?Draw@CMBDragObject@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBDragObject::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    if (mObject)
        return mObject->Draw(dest, srcRect, destRect);
    return 0;
}

// 0x10022610 ?GetParent@CMBDragObject@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBDragObject::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x10022630 ?SetParent@CMBDragObject@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBDragObject::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject(this);
    mParent = newParent;
    return oldParent;
}

// 0x10022670 ?GetBackground@CMBDragObject@@UBEXPAPAVIMBObject@@@Z
void CMBDragObject::GetBackground(IMBObject **background) const
{
    *background = 0;
}

// 0x10022680 ?SetBackground@CMBDragObject@@UAEXPAVIMBObject@@@Z
void CMBDragObject::SetBackground(IMBObject *newBg)
{
}

// 0x10022690 ?AddObject@CMBDragObject@@UAEXPAVIMBObject@@@Z
void CMBDragObject::AddObject(IMBObject *obj)
{
}

// 0x100226a0 ?RemoveObject@CMBDragObject@@UAEXPAVIMBObject@@@Z
void CMBDragObject::RemoveObject(IMBObject *obj)
{
}

// 0x100226b0 ?ObjectMoved@CMBDragObject@@UAEXPAVIMBObject@@@Z
void CMBDragObject::ObjectMoved(IMBObject *obj)
{
}

// 0x100226c0 ?IsClicked@CMBDragObject@@UBE_NABVMBPoint@@F@Z
bool CMBDragObject::IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const
{
    MBRect bounds;
    if (!mObject || !IsActive() || !IsVisible())
        return false;
    bounds = GetBounds();
    if (!bounds.Contains(world_mouse_pos))
        return false;
    if (!mPixelTest)
        return true;
    MBComPtr<IMBSurfaceObject> so(mObject);
    MBComPtr<IMBAnim> anim(mObject);
    MBComPtr<IMBSurface> surface;
    long x, y;
    if (so) {
        IMBSurface *tempSurface;
        so->GetSurface(&tempSurface);
        surface.Attach(tempSurface);
        x = world_mouse_pos.x - GetPosition().x;
        y = world_mouse_pos.y - GetPosition().y;
    } else if (anim) {
        long sequence = anim->GetCurrentSequenceIndex();
        long frame = anim->GetCurrentFrameIndex();
        long cast = anim->GetCastID(sequence, frame);
        IMBSurface *tempSurface;
        anim->GetCastMemberSurface(cast, &tempSurface);
        surface.Attach(tempSurface);
        x = world_mouse_pos.x - bounds.left;
        y = world_mouse_pos.y - bounds.top;
    }
    if (surface) {
        MBColor pixelColor = surface->GetPixelColor(x, y);
        MBColor transparentColor = surface->GetTransparentColor();
        return pixelColor != transparentColor;
    }
    return false;
}

// 0x10022940 ?IsTrackingClick@CMBDragObject@@UBE_NXZ
bool CMBDragObject::IsTrackingClick(void) const
{
    return mIsTrackingClick;
}

// 0x10022950 ?StartTrackingClick@CMBDragObject@@UAEXABVMBPoint@@F@Z
void CMBDragObject::StartTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    if (IsActive() && mObject && IsVisible()) {
        MBComPtr<IMBEventManager> broadcaster;
        broadcaster.CreateObject(MBIID_IMBEventManager);
        if (broadcaster && mBroadcastChannel && mPickUpEvent != -1)
            broadcaster->BroadcastEvent(mBroadcastChannel, mPickUpEvent, this);
        mDragStartMousePos = world_mouse_pos;
        mDragStartSurfacePos = GetOffset();
        mIsTrackingClick = true;
    }
}

// 0x10022a30 ?TrackClick@CMBDragObject@@UAEXABVMBPoint@@@Z
void CMBDragObject::TrackClick(const MBPoint &world_mouse_pos)
{
    if (IsActive() && mObject && IsVisible()) {
        MBPoint3 newpos = mDragStartSurfacePos;
        newpos.x += world_mouse_pos.x - mDragStartMousePos.x;
        newpos.y += world_mouse_pos.y - mDragStartMousePos.y;
        newpos.z += mZAdjust;
        CheckForSnap(newpos, false);
        Place(newpos);
    }
}

// 0x10022ab0 ?StopTrackingClick@CMBDragObject@@UAEXABVMBPoint@@F@Z
void CMBDragObject::StopTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = false;
    if (IsActive() && mObject && IsVisible()) {
        MBPoint3 newpos = mDragStartSurfacePos;
        newpos.x += world_mouse_pos.x - mDragStartMousePos.x;
        newpos.y += world_mouse_pos.y - mDragStartMousePos.y;
        CheckForSnap(newpos, true);
        Place(newpos);
        MBComPtr<IMBEventManager> broadcaster;
        broadcaster.CreateObject(MBIID_IMBEventManager);
        if (broadcaster && mBroadcastChannel && mDropEvent != -1)
            broadcaster->BroadcastEvent(mBroadcastChannel, mDropEvent, this);
    }
}

// 0x10022ba0 ?AddSnapToPoint@CMBDragObject@@UAEXVMBPoint@@J_N@Z
void CMBDragObject::AddSnapToPoint(MBPoint point, long range, bool on_release_only)
{
    SnapToPoint *snap = new SnapToPoint;
    snap->point = point;
    snap->range = range;
    snap->onreleaseonly = on_release_only;
    mSnapToPoints.push_back(snap);
}

// 0x10022c30 ?RemoveSnapToPoint@CMBDragObject@@UAEXVMBPoint@@@Z
void CMBDragObject::RemoveSnapToPoint(MBPoint point)
{
    for (MBList<SnapToPoint *>::iterator i = mSnapToPoints.begin(); i != mSnapToPoints.end(); ++i) {
        if ((*i)->point == point) {
            delete *i;
            mSnapToPoints.erase(i);
            return;
        }
    }
}

// 0x10022c90 ?ClearSnapToPoints@CMBDragObject@@UAEXXZ
void CMBDragObject::ClearSnapToPoints(void)
{
    while (mSnapToPoints.size() > 0) {
        delete mSnapToPoints.front();
        mSnapToPoints.pop_front();
    }
}

// EQUIV 0x10022cf0 ?CheckForSnap@CMBDragObject@@IAEXAAVMBPoint3@@_N@Z: reviewed: register choice and the order of the summed terms only; same clamps, loop and snap
void CMBDragObject::CheckForSnap(MBPoint3 &objectpos, bool release)
{
    MBList<SnapToPoint *>::iterator i = mSnapToPoints.begin();
    MBRect bounds = GetBounds();
    if (objectpos.x + (bounds.right - bounds.left) > mBoundingBox.right)
        objectpos.x = mBoundingBox.right - (bounds.right - bounds.left);
    if (objectpos.y + (bounds.bottom - bounds.top) > mBoundingBox.bottom)
        objectpos.y = mBoundingBox.bottom - (bounds.bottom - bounds.top);
    if (objectpos.x < mBoundingBox.left)
        objectpos.x = mBoundingBox.left;
    if (objectpos.y < mBoundingBox.top)
        objectpos.y = mBoundingBox.top;
    for (; i != mSnapToPoints.end(); ++i) {
        if ((release && (*i)->onreleaseonly) || !(*i)->onreleaseonly) {
            long dx = objectpos.x - (*i)->point.x;
            if (dx < 0)
                dx = -dx;
            long dy = objectpos.y - (*i)->point.y;
            if (dy < 0)
                dy = -dy;
            long distance = dx < dy ? dy : dx;
            if (distance <= (*i)->range) {
                objectpos.x = (*i)->point.x;
                objectpos.y = (*i)->point.y;
                return;
            }
        }
    }
}


// Matched by this code:
// 0x10022df0 _$E2
// 0x10022e00 _$E1
// 0x10022ed0 _$E5
// 0x10022ee0 _$E4
// 0x10022fb0 _$E8
// 0x10022fc0 _$E7
// 0x10023090 _$E11
// 0x100230a0 _$E10
// 0x10023160 _$E14
// 0x10023170 _$E13
// 0x10023230 _$E17
// 0x10023240 _$E16
// 0x100232f0 _$E20
// 0x10023300 _$E19
// 0x100233b0 _$E23
// 0x100233c0 _$E22
// 0x10023470 _$E26
// 0x10023480 _$E25
// 0x10023540 _$E29
// 0x10023550 _$E28
// 0x10023610 _$E32
// 0x10023620 _$E31
// 0x100236d0 _$E35
// 0x100236e0 _$E34
// 0x10023790 _$E38
// 0x100237a0 _$E37
// 0x10023850 _$E41
// 0x10023860 _$E40
// 0x10023910 _$E44
// 0x10023920 _$E43
// 0x100239d0 _$E47
// 0x100239e0 _$E46
// 0x10023a90 ?erase@?$list@PAUSnapToPoint@CMBDragObject@@V?$MBAllocator@PAUSnapToPoint@CMBDragObject@@@@@std@@QAE?AViterator@12@V312@@Z
// 0x10023ad0 ?_QueryInterface@?$MBComPtr@VIMBSurfaceObject@@@@AAEJABV?$MBComPtr@VIMBObject@@@@@Z
// 0x10023b50 ?_QueryInterface@?$MBComPtr@VIMBAnim@@@@AAEJABV?$MBComPtr@VIMBObject@@@@@Z
// 0x10023bd0 _$E68
// 0x10023bf0 _$E67
// 0x10023c00 ?QueryInterface@CMBDragObject@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10023c10 ?AddRef@CMBDragObject@@W3AEKXZ
// 0x10023c20 ?Release@CMBDragObject@@W3AEKXZ
