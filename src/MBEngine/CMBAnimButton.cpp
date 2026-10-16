// CMBAnimButton.cpp - MBEngine.dll CMBAnimButton.obj
#include "CMBAnimButton.h"

// 0x1001ef70 ?MakeCMBAnimButton@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBAnimButton(void)
{
    IMBUnknown *p = (IMBAnimButton *)new CMBAnimButton;
    if (p)
        p->AddRef();
    return p;
}

// 0x1001efa0 ??0CMBAnimButton@@QAE@XZ
CMBAnimButton::CMBAnimButton(void)
    : mParent(0), mUserData(0), mBroadcastChannel(-1), mClickMessage(-1), mRolloverMessage(-1),
      mLeaveRolloverMessage(-1), mBehavior(kPushButtonBehavior), mButtonState(kButtonUp), mOffset(0, 0, 0),
      mIsActive(true), mPixelTest(false), mIsTrackingClick(false), mIsEnabled(true), mIsOn(false)
{
    _mRefCnt = 0;
    mDrawStateSequences[kDrawUp] = -1;
    mDrawStateSequences[kDrawDown] = -1;
    mDrawStateSequences[kDrawRollover] = -1;
    mDrawStateSequences[kDrawDisabledUp] = -1;
    mDrawStateSequences[kDrawDisabledDown] = -1;
}

// 0x1001f020 ?QueryInterface@CMBAnimButton@@UAEJABUMBGUID@@PAPAX@Z
long CMBAnimButton::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBAnimButton *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBAnimButton *)this;
    if (iid == MBIID_IMBAnimButton)
        p = (IMBAnimButton *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBObjectGroup)
        p = (IMBObjectGroup *)this;
    if (iid == MBIID_IMBUserData)
        p = (IMBUserData *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1001f200 ?AddRef@CMBAnimButton@@UAEKXZ
unsigned long CMBAnimButton::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1001f210 ?Release@CMBAnimButton@@UAEKXZ
unsigned long CMBAnimButton::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1001f240 ?SetPixelTest@CMBAnimButton@@UAEX_N@Z
void CMBAnimButton::SetPixelTest(bool set)
{
    mPixelTest = set;
}

// 0x1001f250 ?GetPixelTest@CMBAnimButton@@UAE_NXZ
bool CMBAnimButton::GetPixelTest(void)
{
    return mPixelTest;
}

// 0x1001f260 ??1CMBAnimButton@@QAE@XZ
CMBAnimButton::~CMBAnimButton(void)
{
    if (mUserData)
        mUserData->Release();
}

// 0x1001f2a0 ?GetOffset@CMBAnimButton@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBAnimButton::GetOffset(void) const
{
    return mOffset;
}

// 0x1001f2c0 ?GetPosition@CMBAnimButton@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBAnimButton::GetPosition(void) const
{
    MBPoint3 pos = mOffset;
    if (mParent)
        pos += mParent->GetPosition();
    return pos;
}

// 0x1001f310 ?Place@CMBAnimButton@@UAEXABVMBPoint3@@@Z
void CMBAnimButton::Place(const MBPoint3 &point)
{
    mOffset = point;
    if (mParent)
        mParent->ObjectMoved((IMBAnimButton *)this);
    SetDirty();
}

// 0x1001f350 ?Move@CMBAnimButton@@UAEXABVMBPoint3@@@Z
void CMBAnimButton::Move(const MBPoint3 &point)
{
    mOffset += point;
    if (mParent)
        mParent->ObjectMoved((IMBAnimButton *)this);
    SetDirty();
}

// 0x1001f390 ?GetBounds@CMBAnimButton@@UBE?AVMBRect@@XZ
MBRect CMBAnimButton::GetBounds(void) const
{
    return mAnimation->GetBounds();
}

// 0x1001f3d0 ?IsVisible@CMBAnimButton@@UBE_NXZ
bool CMBAnimButton::IsVisible(void) const
{
    return mAnimation->IsVisible();
}

// 0x1001f3e0 ?Show@CMBAnimButton@@UAEJXZ
long CMBAnimButton::Show(void)
{
    return mAnimation->Show();
}

// 0x1001f3f0 ?Hide@CMBAnimButton@@UAEJXZ
long CMBAnimButton::Hide(void)
{
    return mAnimation->Hide();
}

// 0x1001f400 ?IsDirty@CMBAnimButton@@UBE_NXZ
bool CMBAnimButton::IsDirty(void) const
{
    return mAnimation->IsDirty();
}

// 0x1001f410 ?SetDirty@CMBAnimButton@@UAEXXZ
void CMBAnimButton::SetDirty(void)
{
    mAnimation->SetDirty();
}

// 0x1001f420 ?ClearDirty@CMBAnimButton@@UAEXXZ
void CMBAnimButton::ClearDirty(void)
{
    mAnimation->ClearDirty();
}

// 0x1001f430 ?Draw@CMBAnimButton@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CMBAnimButton::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    return mAnimation->Draw(dest, srcRect, destRect);
}

// 0x1001f450 ?GetParent@CMBAnimButton@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBAnimButton::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x1001f470 ?SetParent@CMBAnimButton@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBAnimButton::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject((IMBAnimButton *)this);
    mParent = newParent;
    return oldParent;
}

// 0x1001f4b0 ?GetBackground@CMBAnimButton@@UBEXPAPAVIMBObject@@@Z
void CMBAnimButton::GetBackground(IMBObject **background) const
{
    *background = 0;
}

// 0x1001f4c0 ?SetBackground@CMBAnimButton@@UAEXPAVIMBObject@@@Z
void CMBAnimButton::SetBackground(IMBObject *newBg)
{
}

// 0x1001f4d0 ?AddObject@CMBAnimButton@@UAEXPAVIMBObject@@@Z
void CMBAnimButton::AddObject(IMBObject *obj)
{
}

// 0x1001f4e0 ?RemoveObject@CMBAnimButton@@UAEXPAVIMBObject@@@Z
void CMBAnimButton::RemoveObject(IMBObject *obj)
{
}

// 0x1001f4f0 ?ObjectMoved@CMBAnimButton@@UAEXPAVIMBObject@@@Z
void CMBAnimButton::ObjectMoved(IMBObject *obj)
{
}

// EQUIV 0x1001f500 ?SetAnim@CMBAnimButton@@UAEXPBD@Z: reviewed: the original keeps mgr.CreateObject out of line, VC6 inlines it; the rest is registers and the order of independent loads. Was: inline budget: original keeps mgr.CreateObject out of line (mAnimation's inlined); ours inlines both (guid ctor variant also inlines) (70% similar)
void CMBAnimButton::SetAnim(const char *animName)
{
    unsigned long id = MBHashString(animName);
    MBComPtr<IMBResourceFileList> mgr;
    mgr.CreateObject(MBIID_CMBResourceMgr);
    IMBStream *tempStream;
    mgr->GetStream(id, 'anm ', &tempStream);
    if (tempStream) {
        mAnimation.CreateObject(MBIID_CMBAnim);
        if (mAnimation) {
            mAnimation->SetParent(this);
            mAnimation->Load(id, tempStream, false);
            mAnimation->SetCurrentSequence(0);
            mAnimation->Play();
            long numSequences = mAnimation->GetNumSequences();
            if (numSequences >= 1)
                mDrawStateSequences[kDrawUp] = 0;
            if (numSequences >= 2)
                mDrawStateSequences[kDrawDown] = 1;
            if (numSequences >= 3)
                mDrawStateSequences[kDrawRollover] = 2;
            if (numSequences >= 4)
                mDrawStateSequences[kDrawDisabledUp] = 3;
            if (numSequences >= 5)
                mDrawStateSequences[kDrawDisabledDown] = 4;
        }
        tempStream->Release();
    }
}

// 0x1001f640 ?GetAnim@CMBAnimButton@@UAEXPAPAVIMBAnim@@@Z
void CMBAnimButton::GetAnim(IMBAnim **anim)
{
    IMBAnim *p = mAnimation;
    *anim = p;
    if (p)
        p->AddRef();
}

// 0x1001f660 ?SetBroadcastChannel@CMBAnimButton@@UAEXJ@Z
void CMBAnimButton::SetBroadcastChannel(long channel)
{
    mBroadcastChannel = channel;
}

// 0x1001f670 ?SetClickMessage@CMBAnimButton@@UAEXJ@Z
void CMBAnimButton::SetClickMessage(long message)
{
    mClickMessage = message;
}

// 0x1001f680 ?SetRolloverMessage@CMBAnimButton@@UAEXJ@Z
void CMBAnimButton::SetRolloverMessage(long message)
{
    mRolloverMessage = message;
}

// 0x1001f690 ?SetLeaveRolloverMessage@CMBAnimButton@@UAEXJ@Z
void CMBAnimButton::SetLeaveRolloverMessage(long message)
{
    mLeaveRolloverMessage = message;
}

// 0x1001f6a0 ?GetBroadcastChannel@CMBAnimButton@@UBEJXZ
long CMBAnimButton::GetBroadcastChannel(void) const
{
    return mBroadcastChannel;
}

// 0x1001f6b0 ?GetClickMessage@CMBAnimButton@@UBEJXZ
long CMBAnimButton::GetClickMessage(void) const
{
    return mClickMessage;
}

// 0x1001f6c0 ?GetRolloverMessage@CMBAnimButton@@UBEJXZ
long CMBAnimButton::GetRolloverMessage(void) const
{
    return mRolloverMessage;
}

// 0x1001f6d0 ?GetLeaveRolloverMessage@CMBAnimButton@@UBEJXZ
long CMBAnimButton::GetLeaveRolloverMessage(void) const
{
    return mLeaveRolloverMessage;
}

// 0x1001f6e0 ?IsEnabled@CMBAnimButton@@UBE_NXZ
bool CMBAnimButton::IsEnabled(void) const
{
    return mIsEnabled;
}

// 0x1001f6f0 ?Enable@CMBAnimButton@@UAEXXZ
void CMBAnimButton::Enable(void)
{
    if (IsEnabled())
        return;
    mIsEnabled = true;
    long currentIndex = mAnimation->GetCurrentSequenceIndex();
    long newIndex;
    if (mButtonState == kButtonUp)
        newIndex = GetUpSequenceIndex();
    else if (mButtonState == kButtonDown)
        newIndex = GetDownSequenceIndex();
    else
        return;
    if (newIndex != -1 && newIndex != currentIndex)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x1001f750 ?Disable@CMBAnimButton@@UAEXXZ
void CMBAnimButton::Disable(void)
{
    if (!IsEnabled())
        return;
    mIsEnabled = false;
    long currentIndex = mAnimation->GetCurrentSequenceIndex();
    long newIndex;
    if (mButtonState == kButtonUp)
        newIndex = GetUpSequenceIndex();
    else
        newIndex = GetDownSequenceIndex();
    if (newIndex != -1 && newIndex != currentIndex)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x1001f7a0 ?SetBehavior@CMBAnimButton@@UAEXW4MBButtonBehavior@@@Z
void CMBAnimButton::SetBehavior(MBButtonBehavior behavior)
{
    mBehavior = behavior;
}

// 0x1001f7b0 ?GetBehavior@CMBAnimButton@@UBE?AW4MBButtonBehavior@@XZ
MBButtonBehavior CMBAnimButton::GetBehavior(void) const
{
    return mBehavior;
}

// 0x1001f7c0 ?GetButtonState@CMBAnimButton@@UBE?AW4MBButtonState@@XZ
MBButtonState CMBAnimButton::GetButtonState(void) const
{
    return mButtonState;
}

// 0x1001f7d0 ?SetButtonState@CMBAnimButton@@UAEXW4MBButtonState@@@Z
void CMBAnimButton::SetButtonState(MBButtonState buttonState)
{
    mButtonState = buttonState;
    long currentIndex = mAnimation->GetCurrentSequenceIndex();
    long newIndex;
    if (mButtonState == kButtonUp)
        newIndex = GetUpSequenceIndex();
    else if (mButtonState == kButtonDown)
        newIndex = GetDownSequenceIndex();
    else
        return;
    if (newIndex != -1 && newIndex != currentIndex)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x1001f820 ?IsRolledOver@CMBAnimButton@@UBE_NABVMBPoint@@@Z
bool CMBAnimButton::IsRolledOver(const MBPoint &world_mouse_pos) const
{
    return IsActive() && IsVisible() && IsWorldPointInAnim(world_mouse_pos);
}

// 0x1001f860 ?EnterRollover@CMBAnimButton@@UAEXABVMBPoint@@@Z
void CMBAnimButton::EnterRollover(const MBPoint &world_mouse_pos)
{
    if (!IsEnabled())
        return;
    if (mIsTrackingClick) {
        long newSequenceIndex;
        switch (mBehavior) {
        case kPushButtonBehavior:
            newSequenceIndex = mDrawStateSequences[kDrawDown];
            break;
        case kCheckBoxBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kDrawDown];
            else if (mButtonState == kButtonDown)
                newSequenceIndex = mDrawStateSequences[kDrawUp];
            break;
        case kRadioButtonBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kDrawDown];
            break;
        }
        if (newSequenceIndex != -1)
            mAnimation->SetCurrentSequence(newSequenceIndex);
    } else {
        if (mDrawStateSequences[kDrawRollover] != -1)
            mAnimation->SetCurrentSequence(mDrawStateSequences[kDrawRollover]);
        if (mBroadcastChannel != -1 && mRolloverMessage != -1) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(mBroadcastChannel, mRolloverMessage, this);
        }
    }
}

// 0x1001f990 ?WithinRollover@CMBAnimButton@@UAEXABVMBPoint@@@Z
void CMBAnimButton::WithinRollover(const MBPoint &world_mouse_pos)
{
}

// 0x1001f9a0 ?LeaveRollover@CMBAnimButton@@UAEXXZ
void CMBAnimButton::LeaveRollover(void)
{
    if (!IsEnabled())
        return;
    long newSequenceIndex = -1;
    switch (mBehavior) {
    case kPushButtonBehavior:
        newSequenceIndex = mDrawStateSequences[kDrawUp];
        break;
    case kCheckBoxBehavior:
        if (mButtonState == kButtonUp)
            newSequenceIndex = mDrawStateSequences[kDrawUp];
        else if (mButtonState == kButtonDown)
            newSequenceIndex = mDrawStateSequences[kDrawDown];
        break;
    case kRadioButtonBehavior:
        if (mButtonState == kButtonUp)
            newSequenceIndex = mDrawStateSequences[kDrawUp];
        else if (mButtonState == kButtonDown)
            newSequenceIndex = mDrawStateSequences[kDrawDown];
        break;
    }
    if (newSequenceIndex != -1)
        mAnimation->SetCurrentSequence(newSequenceIndex);
    if (mBroadcastChannel != -1 && mLeaveRolloverMessage != -1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mLeaveRolloverMessage, this);
    }
}

// 0x1001fa90 ?IsClicked@CMBAnimButton@@UBE_NABVMBPoint@@F@Z
bool CMBAnimButton::IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const
{
    bool isClicked = false;
    MBRect bounds;
    if (!mAnimation || !IsActive() || !IsVisible())
        return false;
    bounds = GetBounds();
    if (!bounds.Contains(world_mouse_pos))
        return false;
    if (!mPixelTest)
        return true;
    long sequence = mAnimation->GetCurrentSequenceIndex();
    long frame = mAnimation->GetCurrentFrameIndex();
    long cast = mAnimation->GetCastID(sequence, frame);
    IMBSurface *surface;
    mAnimation->GetCastMemberSurface(cast, &surface);
    if (surface) {
        long x = world_mouse_pos.x - bounds.left;
        long y = world_mouse_pos.y - bounds.top;
        MBColor pixelColor = surface->GetPixelColor(x, y);
        MBColor transparentColor = surface->GetTransparentColor();
        isClicked = pixelColor != transparentColor;
        surface->Release();
    }
    return isClicked;
}

// 0x1001fc10 ?IsTrackingClick@CMBAnimButton@@UBE_NXZ
bool CMBAnimButton::IsTrackingClick(void) const
{
    return mIsTrackingClick;
}

// 0x1001fc20 ?StartTrackingClick@CMBAnimButton@@UAEXABVMBPoint@@F@Z
void CMBAnimButton::StartTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    if (IsEnabled()) {
        long newSequenceIndex = -1;
        switch (mBehavior) {
        case kPushButtonBehavior:
            newSequenceIndex = mDrawStateSequences[kDrawDown];
            break;
        case kCheckBoxBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kDrawDown];
            else if (mButtonState == kButtonDown)
                newSequenceIndex = mDrawStateSequences[kDrawUp];
            break;
        case kRadioButtonBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kDrawDown];
            break;
        }
        if (newSequenceIndex != -1)
            mAnimation->SetCurrentSequence(newSequenceIndex);
    }
    mIsTrackingClick = true;
}

// 0x1001fc80 ?TrackClick@CMBAnimButton@@UAEXABVMBPoint@@@Z
void CMBAnimButton::TrackClick(const MBPoint &world_mouse_pos)
{
}

// 0x1001fc90 ?StopTrackingClick@CMBAnimButton@@UAEXABVMBPoint@@F@Z
void CMBAnimButton::StopTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = false;
    if (IsVisible() && IsRolledOver(world_mouse_pos)) {
        if (IsEnabled()) {
            long newSequenceIndex = -1;
            switch (mBehavior) {
            case kPushButtonBehavior:
                newSequenceIndex = mDrawStateSequences[kDrawRollover];
                break;
            case kCheckBoxBehavior:
                newSequenceIndex = mDrawStateSequences[kDrawRollover];
                if (mButtonState == kButtonUp)
                    mButtonState = kButtonDown;
                else if (mButtonState == kButtonDown)
                    mButtonState = kButtonUp;
                break;
            case kRadioButtonBehavior:
                newSequenceIndex = mDrawStateSequences[kDrawRollover];
                mButtonState = kButtonDown;
                break;
            }
            if (newSequenceIndex != -1)
                mAnimation->SetCurrentSequence(newSequenceIndex);
        }
        if (mBroadcastChannel != -1 && mClickMessage != -1) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(mBroadcastChannel, mClickMessage, this);
        }
    }
}

// 0x1001fdb0 ?IsActive@CMBAnimButton@@UBE_NXZ
bool CMBAnimButton::IsActive(void) const
{
    return mIsActive;
}

// 0x1001fdc0 ?Activate@CMBAnimButton@@UAEXXZ
void CMBAnimButton::Activate(void)
{
    mIsActive = true;
}

// 0x1001fdd0 ?Deactivate@CMBAnimButton@@UAEXXZ
void CMBAnimButton::Deactivate(void)
{
    mIsActive = false;
}

// 0x1001fde0 ?IsWorldPointInAnim@CMBAnimButton@@IBE_NABVMBPoint@@@Z
bool CMBAnimButton::IsWorldPointInAnim(const MBPoint &worldPoint) const
{
    bool result = false;
    if (mAnimation) {
        MBRect bounds = mAnimation->GetBounds();
        result = bounds.Contains(worldPoint);
    }
    return result;
}

// 0x1001fe40 ?GetUpSequenceIndex@CMBAnimButton@@IBEJXZ
long CMBAnimButton::GetUpSequenceIndex(void) const
{
    if (!IsEnabled() && mDrawStateSequences[kDrawDisabledUp] != -1)
        return mDrawStateSequences[kDrawDisabledUp];
    return mDrawStateSequences[kDrawUp];
}

// 0x1001fe60 ?GetDownSequenceIndex@CMBAnimButton@@IBEJXZ
long CMBAnimButton::GetDownSequenceIndex(void) const
{
    if (!IsEnabled()) {
        if (mDrawStateSequences[kDrawDisabledDown] != -1)
            return mDrawStateSequences[kDrawDisabledDown];
        if (mDrawStateSequences[kDrawDisabledUp] != -1)
            return mDrawStateSequences[kDrawDisabledUp];
    }
    return mDrawStateSequences[kDrawDown];
}

// 0x1001fe90 ?SetUserData@CMBAnimButton@@UAEXPAVIMBMemoryBlock@@@Z
void CMBAnimButton::SetUserData(IMBMemoryBlock *userData)
{
    if (mUserData)
        mUserData->Release();
    mUserData = userData;
    if (mUserData)
        mUserData->AddRef();
}

// 0x1001fec0 ?GetUserData@CMBAnimButton@@UAEXPAPAVIMBMemoryBlock@@@Z
void CMBAnimButton::GetUserData(IMBMemoryBlock **userData)
{
    IMBMemoryBlock *p = mUserData;
    *userData = p;
    if (p)
        p->AddRef();
}


// Matched by this code:
// 0x1001aa70 ?Release@?$MBComPtr@VIMBEventManager@@@@QAEXXZ
// 0x1001ff80 _$E68
// 0x1001ffa0 _$E67
// 0x1001ffb0 ?QueryInterface@CMBAnimButton@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1001ffc0 ?AddRef@CMBAnimButton@@W3AEKXZ
// 0x1001ffd0 ?Release@CMBAnimButton@@W3AEKXZ
// 0x1001ffe0 ?QueryInterface@CMBAnimButton@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1001fff0 ?AddRef@CMBAnimButton@@W7AEKXZ
// 0x10020000 ?Release@CMBAnimButton@@W7AEKXZ
// 0x10020010 ?GetOffset@CMBAnimButton@@W7BE?AVMBPoint3@@XZ
// 0x10020020 ?GetPosition@CMBAnimButton@@W7BE?AVMBPoint3@@XZ
// 0x10020030 ?Place@CMBAnimButton@@W7AEXABVMBPoint3@@@Z
// 0x10020040 ?Move@CMBAnimButton@@W7AEXABVMBPoint3@@@Z
// 0x10020050 ?GetBounds@CMBAnimButton@@W7BE?AVMBRect@@XZ
// 0x10020060 ?IsVisible@CMBAnimButton@@W7BE_NXZ
// 0x10020070 ?Show@CMBAnimButton@@W7AEJXZ
// 0x10020080 ?Hide@CMBAnimButton@@W7AEJXZ
// 0x10020090 ?IsDirty@CMBAnimButton@@W7BE_NXZ
// 0x100200a0 ?SetDirty@CMBAnimButton@@W7AEXXZ
// 0x100200b0 ?ClearDirty@CMBAnimButton@@W7AEXXZ
// 0x100200c0 ?Draw@CMBAnimButton@@W7BEJPAVIMBSurface@@AAVMBRect@@1@Z
// 0x100200d0 ?GetParent@CMBAnimButton@@W7BEXPAPAVIMBObjectGroup@@@Z
// 0x100200e0 ?SetParent@CMBAnimButton@@W7AEPAVIMBObjectGroup@@PAV2@@Z
// 0x100200f0 ?QueryInterface@CMBAnimButton@@WM@AEJABUMBGUID@@PAPAX@Z
// 0x10020100 ?AddRef@CMBAnimButton@@WM@AEKXZ
// 0x10020110 ?Release@CMBAnimButton@@WM@AEKXZ
// 0x10020120 _$E15
// 0x10020130 _$E14
// 0x10020200 _$E18
// 0x10020210 _$E17
// 0x100202e0 _$E21
// 0x100202f0 _$E20
// 0x100203c0 _$E24
// 0x100203d0 _$E23
// 0x10020490 _$E27
// 0x100204a0 _$E26
// 0x10020560 _$E30
// 0x10020570 _$E29
// 0x10020620 _$E33
// 0x10020630 _$E32
// 0x100206e0 _$E36
// 0x100206f0 _$E35
// 0x100207a0 _$E39
// 0x100207b0 _$E38
// 0x10020870 _$E42
// 0x10020880 _$E41
// 0x10020940 _$E45
// 0x10020950 _$E44
// 0x10020a00 _$E48
// 0x10020a10 _$E47
// 0x10020ac0 _$E51
// 0x10020ad0 _$E50
// 0x10020b80 _$E54
// 0x10020b90 _$E53
// 0x10020c40 _$E57
// 0x10020c50 _$E56
// 0x10020d00 _$E60
// 0x10020d10 _$E59

// TODO 0x1001fee0 ?CreateObject@?$MBComPtr@VIMBResourceFileList@@@@QAEJABUMBGUID@@@Z: MBComPtr template body in CMBAnimButton.h, not emitted: our compile inlines every CreateObject call (see SetAnim)
