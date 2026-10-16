#include "CSBAnimButton.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBColor.h"
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include "SBMouseEvent.h"
#include <stdio.h>

// 0x10018ed0 ?MakeCSBAnimButton@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBAnimButton()
{
    IMBUnknown *obj = (ISBAnimButton *)new CSBAnimButton;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x10018f60 ??0CSBAnimButton@@QAE@XZ
CSBAnimButton::CSBAnimButton()
    : mOffset(0, 0, 0), mIsActive(true), mIsEnabled(true), mParent(0), mBroadcastChannel(-1), mClickMessage(-1),
      mRolloverMessage(-1), mBehavior(kPushButtonBehavior), mButtonState(kButtonUp), mPixelTest(false),
      mIsTrackingClick(false), mIsOn(false), mRolloverData(0), mClickData(0), mLeaveData(0)
{
    _mRefCnt = 0;
    for (int i = 0; i < 5; i++) {
        mDrawStateSequences[i] = -1;
        mDrawStateSounds[i].soundID = 0;
        mDrawStateSounds[i].soundChannel = -1;
    }
}

// 0x10019030 ?QueryInterface@CSBAnimButton@@UAEJABUMBGUID@@PAPAX@Z
long CSBAnimButton::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_IMBObject)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_ISBAnimButton)
        p = (ISBAnimButton *)this;
    if (iid == MBIID_IMBMouseable)
        p = (IMBMouseable *)this;
    if (iid == MBIID_IMBObjectGroup)
        p = (IMBObjectGroup *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x10019230 ?AddRef@CSBAnimButton@@UAEKXZ
unsigned long CSBAnimButton::AddRef()
{
    return ++_mRefCnt;
}

// 0x10019250 ?Release@CSBAnimButton@@UAEKXZ
unsigned long CSBAnimButton::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x10019290 ?GetClickData@CSBAnimButton@@UAEPAXXZ
void *CSBAnimButton::GetClickData()
{
    return mClickData;
}

// 0x100192b0 ?GetRolloverData@CSBAnimButton@@UAEPAXXZ
void *CSBAnimButton::GetRolloverData()
{
    return mRolloverData;
}

// 0x100192d0 ?GetLeaveData@CSBAnimButton@@UAEPAXXZ
void *CSBAnimButton::GetLeaveData()
{
    return mLeaveData;
}

// 0x100192f0 ?SetPixelTest@CSBAnimButton@@UAEX_N@Z
void CSBAnimButton::SetPixelTest(bool set)
{
    mPixelTest = set;
}

// 0x10019310 ?GetPixelTest@CSBAnimButton@@UAE_NXZ
bool CSBAnimButton::GetPixelTest()
{
    return mPixelTest;
}

// 0x10019330 ??1CSBAnimButton@@QAE@XZ
CSBAnimButton::~CSBAnimButton()
{
}

// 0x10019370 ?SetClickMessage@CSBAnimButton@@UAEXJPAX@Z
void CSBAnimButton::SetClickMessage(long message, void *data)
{
    mClickMessage = message;
    mClickData = data;
}

// 0x100193a0 ?SetRolloverMessage@CSBAnimButton@@UAEXJPAX@Z
void CSBAnimButton::SetRolloverMessage(long message, void *data)
{
    mRolloverMessage = message;
    mRolloverData = data;
}

// 0x100193d0 ?SetLeaveMessage@CSBAnimButton@@UAEXJPAX@Z
void CSBAnimButton::SetLeaveMessage(long message, void *data)
{
    mLeaveMessage = message;
    mLeaveData = data;
}

// 0x10019400 ?SetLeaveMessage@CSBAnimButton@@UAEXJ@Z
void CSBAnimButton::SetLeaveMessage(long message)
{
    mLeaveData = this;
    mLeaveMessage = message;
}

// 0x10019420 ?SetClickMessage@CSBAnimButton@@UAEXJ@Z
void CSBAnimButton::SetClickMessage(long message)
{
    mClickData = this;
    mClickMessage = message;
}

// 0x10019440 ?SetRolloverMessage@CSBAnimButton@@UAEXJ@Z
void CSBAnimButton::SetRolloverMessage(long message)
{
    mRolloverData = this;
    mRolloverMessage = message;
}

// 0x10019460 ?GetOffset@CSBAnimButton@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBAnimButton::GetOffset() const
{
    return mOffset;
}

// 0x10019490 ?GetPosition@CSBAnimButton@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBAnimButton::GetPosition() const
{
    MBPoint3 pos = mOffset;
    if (mParent)
        pos += mParent->GetPosition();
    return pos;
}

// 0x10019500 ?Place@CSBAnimButton@@UAEXABVMBPoint3@@@Z
void CSBAnimButton::Place(const MBPoint3 &point)
{
    mOffset = point;
    if (mParent)
        mParent->ObjectMoved((ISBAnimButton *)this);
    SetDirty();
}

// 0x10019550 ?PlaceCentered@CSBAnimButton@@UAEXAAVMBPoint3@@@Z
void CSBAnimButton::PlaceCentered(MBPoint3 &point)
{
    MBRect bounds = mAnimation->GetBounds();
    Place(point - MBPoint3(bounds.Width() / 2, bounds.Height() / 2, 0));
}

// 0x100195e0 ?Move@CSBAnimButton@@UAEXABVMBPoint3@@@Z
void CSBAnimButton::Move(const MBPoint3 &point)
{
    mOffset += point;
    if (mParent)
        mParent->ObjectMoved((ISBAnimButton *)this);
    SetDirty();
}

// 0x10019630 ?GetBounds@CSBAnimButton@@UBE?AVMBRect@@XZ
MBRect CSBAnimButton::GetBounds() const
{
    return mAnimation->GetBounds();
}

// 0x10019680 ?IsVisible@CSBAnimButton@@UBE_NXZ
bool CSBAnimButton::IsVisible() const
{
    IMBAnim *anim = mAnimation;
    return anim->IsVisible();
}

// 0x100196a0 ?Show@CSBAnimButton@@UAEJXZ
long CSBAnimButton::Show()
{
    IMBAnim *anim = mAnimation;
    return anim->Show();
}

// 0x100196c0 ?Hide@CSBAnimButton@@UAEJXZ
long CSBAnimButton::Hide()
{
    return mAnimation->Hide();
}

// 0x100196e0 ?IsDirty@CSBAnimButton@@UBE_NXZ
bool CSBAnimButton::IsDirty() const
{
    return mAnimation->IsDirty();
}

// 0x10019700 ?SetDirty@CSBAnimButton@@UAEXXZ
void CSBAnimButton::SetDirty()
{
    mAnimation->SetDirty();
}

// 0x10019720 ?ClearDirty@CSBAnimButton@@UAEXXZ
void CSBAnimButton::ClearDirty()
{
    mAnimation->ClearDirty();
}

// 0x10019740 ?Draw@CSBAnimButton@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CSBAnimButton::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    return mAnimation->Draw(dest, srcRect, destRect);
}

// 0x10019770 ?GetParent@CSBAnimButton@@UBEXPAPAVIMBObjectGroup@@@Z
void CSBAnimButton::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100197a0 ?SetParent@CSBAnimButton@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CSBAnimButton::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject((ISBAnimButton *)this);
    mParent = newParent;
    return oldParent;
}

// 0x100197f0 ?GetBackground@CSBAnimButton@@UBEXPAPAVIMBObject@@@Z
void CSBAnimButton::GetBackground(IMBObject **background) const
{
    *background = 0;
}

// 0x10019810 ?SetBackground@CSBAnimButton@@UAEXPAVIMBObject@@@Z
void CSBAnimButton::SetBackground(IMBObject *newBg)
{
}

// 0x10019830 ?AddObject@CSBAnimButton@@UAEXPAVIMBObject@@@Z
void CSBAnimButton::AddObject(IMBObject *obj)
{
}

// 0x10019850 ?RemoveObject@CSBAnimButton@@UAEXPAVIMBObject@@@Z
void CSBAnimButton::RemoveObject(IMBObject *obj)
{
}

// 0x10019870 ?ObjectMoved@CSBAnimButton@@UAEXPAVIMBObject@@@Z
void CSBAnimButton::ObjectMoved(IMBObject *obj)
{
}

// 0x10019890 ?SetAnim@CSBAnimButton@@UAEXPAVIMBAnim@@@Z
void CSBAnimButton::SetAnim(IMBAnim *anim)
{
    mAnimation = anim;
    if (mAnimation) {
        mAnimation->SetParent(this);
        mAnimation->SetCurrentSequence(0);
        mAnimation->Play();
        long numSequences = mAnimation->GetNumSequences();
        if (numSequences >= 1)
            mDrawStateSequences[0] = 0;
        if (numSequences >= 2)
            mDrawStateSequences[1] = 1;
        if (numSequences >= 3)
            mDrawStateSequences[2] = 2;
        if (numSequences >= 4)
            mDrawStateSequences[3] = 3;
        if (numSequences >= 5)
            mDrawStateSequences[4] = 4;
    }
}

// 0x10019970 ?SetAnim@CSBAnimButton@@UAEXPBD@Z
void CSBAnimButton::SetAnim(const char *animName)
{
    unsigned long id = MBHashString(animName);
    MBComPtr<IMBResourceFileList> mgr(MBIID_CMBResourceMgr);
    IMBStream *stream;
    MBComPtr<IMBStream> tempStream;
    mgr->GetStream(id, 'anm ', &stream);
    tempStream.Attach(stream);
    if (tempStream) {
        mAnimation.CreateObject(MBIID_CMBAnim);
        if (mAnimation) {
            mAnimation->SetParent(this);
            mAnimation->Load(id, tempStream, false);
            mAnimation->SetCurrentSequence(0);
            mAnimation->Play();
            long numSequences = mAnimation->GetNumSequences();
            if (numSequences >= 1)
                mDrawStateSequences[0] = 0;
            if (numSequences >= 2)
                mDrawStateSequences[1] = 1;
            if (numSequences >= 3)
                mDrawStateSequences[2] = 2;
            if (numSequences >= 4)
                mDrawStateSequences[3] = 3;
            if (numSequences >= 5)
                mDrawStateSequences[4] = 4;
        }
    }
    char message[80];
    sprintf(message, "NULL animation in CSBAnimButton:%s", animName);
}

// 0x10019b80 ?GetAnim@CSBAnimButton@@UAE?AV?$MBComPtr@VIMBAnim@@@@XZ
MBComPtr<IMBAnim> CSBAnimButton::GetAnim()
{
    return mAnimation;
}

// 0x10019bc0 ?SetBroadcastChannel@CSBAnimButton@@UAEXJ@Z
void CSBAnimButton::SetBroadcastChannel(long channel)
{
    mBroadcastChannel = channel;
}

// 0x10019be0 ?GetBroadcastChannel@CSBAnimButton@@UAEJXZ
long CSBAnimButton::GetBroadcastChannel()
{
    return mBroadcastChannel;
}

// 0x10019c00 ?GetClickMessage@CSBAnimButton@@UAEJXZ
long CSBAnimButton::GetClickMessage()
{
    return mClickMessage;
}

// 0x10019c20 ?GetLeaveMessage@CSBAnimButton@@UAEJXZ
long CSBAnimButton::GetLeaveMessage()
{
    return mLeaveMessage;
}

// 0x10019c40 ?GetRolloverMessage@CSBAnimButton@@UAEJXZ
long CSBAnimButton::GetRolloverMessage()
{
    return mRolloverMessage;
}

// 0x10019c60 ?IsEnabled@CSBAnimButton@@UBE_NXZ
bool CSBAnimButton::IsEnabled() const
{
    return mIsEnabled;
}

// 0x10019c80 ?Enable@CSBAnimButton@@UAEXXZ
void CSBAnimButton::Enable()
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
    if (newIndex != -1 && newIndex != currentIndex && mBehavior != kStaticAnim)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x10019d00 ?Disable@CSBAnimButton@@UAEXXZ
void CSBAnimButton::Disable()
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
    if (newIndex != -1 && newIndex != currentIndex && mBehavior != kStaticAnim)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x10019d80 ?SetBehavior@CSBAnimButton@@UAEXW4MBButtonBehavior@@@Z
void CSBAnimButton::SetBehavior(MBButtonBehavior behavior)
{
    mBehavior = behavior;
}

// 0x10019da0 ?GetBehavior@CSBAnimButton@@UBE?AW4MBButtonBehavior@@XZ
MBButtonBehavior CSBAnimButton::GetBehavior() const
{
    return mBehavior;
}

// 0x10019dc0 ?GetButtonState@CSBAnimButton@@UBE?AW4MBButtonState@@XZ
MBButtonState CSBAnimButton::GetButtonState() const
{
    return mButtonState;
}

// 0x10019de0 ?SetButtonState@CSBAnimButton@@UAEXW4MBButtonState@@@Z
void CSBAnimButton::SetButtonState(MBButtonState buttonState)
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
    if (newIndex != -1 && newIndex != currentIndex && mBehavior != kStaticAnim)
        mAnimation->SetCurrentSequence(newIndex);
}

// 0x10019e60 ?SetSound@CSBAnimButton@@UAEXW4MBButtonDrawState@@PBDJ@Z
void CSBAnimButton::SetSound(MBButtonDrawState drawState, const char *soundName, long soundChannel)
{
    mDrawStateSounds[drawState].soundID = MBHashString(soundName);
    mDrawStateSounds[drawState].soundChannel = soundChannel;
}

// 0x10019ea0 ?IsRolledOver@CSBAnimButton@@UBE_NABVMBPoint@@@Z
bool CSBAnimButton::IsRolledOver(const MBPoint &world_mouse_pos) const
{
    return IsActive() && IsEnabled() && IsVisible() && IsWorldPointInAnim(world_mouse_pos);
}

// 0x10019f10 ?EnterRollover@CSBAnimButton@@UAEXABVMBPoint@@@Z
void CSBAnimButton::EnterRollover(const MBPoint &world_mouse_pos)
{
    if (!IsEnabled())
        return;
    if (mIsTrackingClick) {
        long newSequenceIndex;
        switch (mBehavior) {
        case kPushButtonBehavior:
        case kClickableBehavior:
            newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            break;
        case kCheckBoxBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            else if (mButtonState == kButtonDown)
                newSequenceIndex = mDrawStateSequences[kButtonUpDrawState];
            break;
        case kRadioButtonBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            break;
        }
        if (newSequenceIndex != -1 && mBehavior != kStaticAnim)
            mAnimation->SetCurrentSequence(newSequenceIndex);
    } else {
        if (mDrawStateSequences[kButtonRolloverDrawState] != -1 && mBehavior != kStaticAnim)
            mAnimation->SetCurrentSequence(mDrawStateSequences[kButtonRolloverDrawState]);
        unsigned long soundID = mDrawStateSounds[kButtonRolloverDrawState].soundID;
        long soundChannel = mDrawStateSounds[kButtonRolloverDrawState].soundChannel;
        if (soundID)
            MBForceSoundID(soundID, soundChannel, 1);
        if (mBroadcastChannel != -1 && mRolloverMessage != -1) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(mBroadcastChannel, mRolloverMessage, mRolloverData);
        }
    }
}

// 0x1001a110 ?WithinRollover@CSBAnimButton@@UAEXABVMBPoint@@@Z
void CSBAnimButton::WithinRollover(const MBPoint &world_mouse_pos)
{
}

// 0x1001a130 ?LeaveRollover@CSBAnimButton@@UAEXXZ
void CSBAnimButton::LeaveRollover()
{
    if (!IsEnabled())
        return;
    long newSequenceIndex = -1;
    switch (mBehavior) {
    case kPushButtonBehavior:
        newSequenceIndex = mDrawStateSequences[kButtonUpDrawState];
        break;
    case kCheckBoxBehavior:
        if (mButtonState == kButtonUp)
            newSequenceIndex = mDrawStateSequences[kButtonUpDrawState];
        else if (mButtonState == kButtonDown)
            newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
        break;
    case kRadioButtonBehavior:
        if (mButtonState == kButtonUp)
            newSequenceIndex = mDrawStateSequences[kButtonUpDrawState];
        else if (mButtonState == kButtonDown)
            newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
        break;
    }
    if (newSequenceIndex != -1 && mBehavior != kStaticAnim)
        mAnimation->SetCurrentSequence(newSequenceIndex);
    if (mBroadcastChannel != -1 && mLeaveMessage != -1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mLeaveMessage, mLeaveData);
    }
}

// 0x1001a2a0 ?IsClicked@CSBAnimButton@@UBE_NABVMBPoint@@F@Z
bool CSBAnimButton::IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const
{
    bool isClicked = false;
    MBRect bounds;
    if (mAnimation == 0 || !IsActive() || !IsEnabled() || !IsVisible())
        return false;
    bounds = GetBounds();
    if (!bounds.ContainsPoint(world_mouse_pos))
        return false;
    if (!mPixelTest)
        return true;
    long sequence = mAnimation->GetCurrentSequenceIndex();
    long frame = mAnimation->GetCurrentFrameIndex();
    long cast = mAnimation->GetCastID(sequence, frame);
    IMBSurface *surface;
    mAnimation->GetCastMemberSurface(cast, &surface);
    if (surface) {
        MBColor pixelColor = surface->GetPixelColor(world_mouse_pos.x - bounds.left, world_mouse_pos.y - bounds.top);
        MBColor transparentColor = surface->GetTransparentColor();
        isClicked = pixelColor != transparentColor;
        surface->Release();
    }
    return isClicked;
}

// 0x1001a4b0 ?IsTrackingClick@CSBAnimButton@@UBE_NXZ
bool CSBAnimButton::IsTrackingClick() const
{
    return mIsTrackingClick;
}

// 0x1001a4d0 ?StartTrackingClick@CSBAnimButton@@UAEXABVMBPoint@@F@Z
void CSBAnimButton::StartTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    if (IsEnabled()) {
        long newSequenceIndex = -1;
        switch (mBehavior) {
        case kPushButtonBehavior:
        case kClickableBehavior:
            newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            break;
        case kCheckBoxBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            else if (mButtonState == kButtonDown)
                newSequenceIndex = mDrawStateSequences[kButtonUpDrawState];
            break;
        case kRadioButtonBehavior:
            if (mButtonState == kButtonUp)
                newSequenceIndex = mDrawStateSequences[kButtonDownDrawState];
            break;
        }
        if (newSequenceIndex != -1 && mBehavior != kStaticAnim)
            mAnimation->SetCurrentSequence(newSequenceIndex);
    }
    mIsTrackingClick = true;
}

// 0x1001a560 ?TrackClick@CSBAnimButton@@UAEXABVMBPoint@@@Z
void CSBAnimButton::TrackClick(const MBPoint &world_mouse_pos)
{
}

// 0x1001a580 ?StopTrackingClick@CSBAnimButton@@UAEXABVMBPoint@@F@Z
void CSBAnimButton::StopTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = false;
    if (IsVisible() && IsRolledOver(world_mouse_pos)) {
        if (IsEnabled()) {
            long newSequenceIndex = -1;
            switch (mBehavior) {
            case kPushButtonBehavior:
            case kClickableBehavior:
                newSequenceIndex = mDrawStateSequences[kButtonRolloverDrawState];
                break;
            case kCheckBoxBehavior:
                newSequenceIndex = mDrawStateSequences[kButtonRolloverDrawState];
                if (mButtonState == kButtonUp)
                    mButtonState = kButtonDown;
                else if (mButtonState == kButtonDown)
                    mButtonState = kButtonUp;
                break;
            case kRadioButtonBehavior:
                newSequenceIndex = mDrawStateSequences[kButtonRolloverDrawState];
                mButtonState = kButtonDown;
                break;
            }
            if (newSequenceIndex != -1 && mBehavior != kStaticAnim)
                mAnimation->SetCurrentSequence(newSequenceIndex);
        }
        if (mBroadcastChannel != -1 && mClickMessage != -1) {
            MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
            broadcaster->BroadcastEvent(mBroadcastChannel, mClickMessage, mClickData);
        }
    }
}

// 0x1001a740 ?IsActive@CSBAnimButton@@UBE_NXZ
bool CSBAnimButton::IsActive() const
{
    return mIsActive;
}

// 0x1001a760 ?Activate@CSBAnimButton@@UAEXXZ
void CSBAnimButton::Activate()
{
    mIsActive = true;
}

// 0x1001a780 ?Deactivate@CSBAnimButton@@UAEXXZ
void CSBAnimButton::Deactivate()
{
    mIsActive = false;
}

// Adjustor thunks of the IMBMouseable and IMBObjectGroup vftables (emitted with the vftables):

// 0x1001a7a0 ?IsWorldPointInAnim@CSBAnimButton@@IBE_NABVMBPoint@@@Z
bool CSBAnimButton::IsWorldPointInAnim(const MBPoint &worldPoint) const
{
    bool result = false;
    if (mAnimation) {
        MBRect bounds = mAnimation->GetBounds();
        result = bounds.ContainsPoint(worldPoint);
    }
    return result;
}

// 0x1001a820 ?GetUpSequenceIndex@CSBAnimButton@@IBEJXZ
long CSBAnimButton::GetUpSequenceIndex() const
{
    if (!IsEnabled() && mDrawStateSequences[kButtonDisabledDrawState] != -1)
        return mDrawStateSequences[kButtonDisabledDrawState];
    return mDrawStateSequences[kButtonUpDrawState];
}

// 0x1001a850 ?GetDownSequenceIndex@CSBAnimButton@@IBEJXZ
long CSBAnimButton::GetDownSequenceIndex() const
{
    if (!IsEnabled()) {
        if (mDrawStateSequences[kButtonDisabledDownDrawState] != -1)
            return mDrawStateSequences[kButtonDisabledDownDrawState];
        if (mDrawStateSequences[kButtonDisabledDrawState] != -1)
            return mDrawStateSequences[kButtonDisabledDrawState];
    }
    return mDrawStateSequences[kButtonDownDrawState];
}

// 0x1001a890 ?LoadFromStream@CSBAnimButton@@UAEHV?$MBComPtr@VIMBStream@@@@ABV?$MBComPtr@VIMBPref@@@@PBD@Z
int CSBAnimButton::LoadFromStream(const MBComPtr<IMBStream> stream, const MBComPtr<IMBPref> &pref, const char *key)
{
    // the button's section of a scene file: anim, position, behavior and what a click does
    MBString anim;
    pref->GetString(stream, MBString(key), MBString("anim"), anim, MBString(""));
    if (anim == "")
        return 0;
    SetAnim(anim.c_str());
    long x = pref->GetLong(stream, MBString(key), MBString("x0"), 0);
    long y = pref->GetLong(stream, MBString(key), MBString("y0"), 0);
    long z = pref->GetLong(stream, MBString(key), MBString("z0"), 0);
    Place(MBPoint3(x, y, z));
    long behavior = pref->GetLong(stream, MBString(key), MBString("button behavior"), 3);
    SetBehavior((MBButtonBehavior)behavior);
    MBString action;
    MBString data;
    pref->GetString(stream, MBString(key), MBString("action"), action, MBString(""));
    pref->GetString(stream, MBString(key), MBString("data"), data, MBString(""));
    if (action.compare("exit") == 0) {
        SBMouseEvent *mouseEvent = new SBMouseEvent;
        mouseEvent->cursorID = pref->GetLong(stream, MBString(key), MBString("cursor"), 0);
        mouseEvent->action = action;
        mouseEvent->data = data;
        SetBroadcastChannel(200);
        SetClickMessage(201, mouseEvent);
        SetRolloverMessage(202, mouseEvent);
        SetLeaveMessage(203);
    }
    return 1;
}

// 0x1001b6a0 ?QueryInterface@CSBAnimButton@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1001b6b0 ?AddRef@CSBAnimButton@@W3AEKXZ
// 0x1001b6c0 ?Release@CSBAnimButton@@W3AEKXZ
// 0x1001b6d0 ?QueryInterface@CSBAnimButton@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1001b6e0 ?AddRef@CSBAnimButton@@W7AEKXZ
// 0x1001b6f0 ?Release@CSBAnimButton@@W7AEKXZ
// 0x1001b700 ?GetOffset@CSBAnimButton@@W7BE?AVMBPoint3@@XZ
// 0x1001b710 ?GetPosition@CSBAnimButton@@W7BE?AVMBPoint3@@XZ
// 0x1001b720 ?Place@CSBAnimButton@@W7AEXABVMBPoint3@@@Z
// 0x1001b730 ?Move@CSBAnimButton@@W7AEXABVMBPoint3@@@Z
// 0x1001b740 ?GetBounds@CSBAnimButton@@W7BE?AVMBRect@@XZ
// 0x1001b750 ?IsVisible@CSBAnimButton@@W7BE_NXZ
// 0x1001b760 ?Show@CSBAnimButton@@W7AEJXZ
// 0x1001b770 ?Hide@CSBAnimButton@@W7AEJXZ
// 0x1001b780 ?IsDirty@CSBAnimButton@@W7BE_NXZ
// 0x1001b790 ?SetDirty@CSBAnimButton@@W7AEXXZ
// 0x1001b7a0 ?ClearDirty@CSBAnimButton@@W7AEXXZ
// 0x1001b7b0 ?Draw@CSBAnimButton@@W7BEJPAVIMBSurface@@AAVMBRect@@1@Z
// 0x1001b7c0 ?GetParent@CSBAnimButton@@W7BEXPAPAVIMBObjectGroup@@@Z
// 0x1001b7d0 ?SetParent@CSBAnimButton@@W7AEPAVIMBObjectGroup@@PAV2@@Z

// Instances the linker took from other objects (this unit emits them too):
// 0x1000bb20 ??1?$MBComPtr@VIMBStream@@@@QAE@XZ
// 0x1001b460 ?CreateObject@?$MBComPtr@VIMBResourceFileList@@@@QAEJABUMBGUID@@@Z
// 0x1001b500 ?_QueryInterface@?$MBComPtr@VIMBAnim@@@@AAEJABQAVIMBUnknown@@@Z

// 0x1001b620 _$E26
// 0x1001b660 _$E22
// 0x1001b680 _$E25
