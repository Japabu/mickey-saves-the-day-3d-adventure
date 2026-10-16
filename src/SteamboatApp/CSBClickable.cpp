#include "CSBClickable.h"
#include <locale> // evidence: the original unit ends with the std::ctype<unsigned short>::id and std::collate<char>::id initializers
#include "MBColor.h"
#include "MBRuntime.h"
#include "MBSoundUtils.h"
#include <stdio.h>

// 0x1001b7e0 ?MakeCSBClickable@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCSBClickable()
{
    IMBUnknown *obj = (ISBClickable *)new CSBClickable;
    if (obj)
        obj->AddRef();
    return obj;
}

// 0x1001b870 ??0CSBClickable@@QAE@XZ
CSBClickable::CSBClickable()
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

// 0x1001ba00 ?QueryInterface@CSBClickable@@UAEJABUMBGUID@@PAPAX@Z
long CSBClickable::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (ISBClickable *)this;
    if (iid == MBIID_IMBObject)
        p = (ISBClickable *)this;
    if (iid == MBIID_ISBClickable)
        p = (ISBClickable *)this;
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

// 0x1001bc00 ?AddRef@CSBClickable@@UAEKXZ
unsigned long CSBClickable::AddRef()
{
    return ++_mRefCnt;
}

// 0x1001bc20 ?Release@CSBClickable@@UAEKXZ
unsigned long CSBClickable::Release()
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x1001bc60 ?GetClickData@CSBClickable@@UAEPAXXZ
void *CSBClickable::GetClickData()
{
    return mClickData;
}

// 0x1001bc80 ?GetRolloverData@CSBClickable@@UAEPAXXZ
void *CSBClickable::GetRolloverData()
{
    return mRolloverData;
}

// 0x1001bca0 ?GetLeaveData@CSBClickable@@UAEPAXXZ
void *CSBClickable::GetLeaveData()
{
    return mLeaveData;
}

// 0x1001bcc0 ?SetPixelTest@CSBClickable@@UAEX_N@Z
void CSBClickable::SetPixelTest(bool set)
{
    mPixelTest = set;
}

// 0x1001bce0 ?GetPixelTest@CSBClickable@@UAE_NXZ
bool CSBClickable::GetPixelTest()
{
    return mPixelTest;
}

// 0x1001bd00 ?GetIfProperty@CSBClickable@@UAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
std::basic_string<char,std::char_traits<char>,MBAllocator<char> > &CSBClickable::GetIfProperty()
{
    return mIfProperty;
}

// 0x1001bd20 ?GetIfNotProperty@CSBClickable@@UAEAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@XZ
std::basic_string<char,std::char_traits<char>,MBAllocator<char> > &CSBClickable::GetIfNotProperty()
{
    return mIfNotProperty;
}

// 0x1001bd40 ??1CSBClickable@@QAE@XZ
CSBClickable::~CSBClickable()
{
}

// 0x1001beb0 ?SetClickMessage@CSBClickable@@UAEXJPAX@Z
void CSBClickable::SetClickMessage(long message, void *data)
{
    mClickMessage = message;
    mClickData = data;
}

// 0x1001bee0 ?SetRolloverMessage@CSBClickable@@UAEXJPAX@Z
void CSBClickable::SetRolloverMessage(long message, void *data)
{
    mRolloverMessage = message;
    mRolloverData = data;
}

// 0x1001bf10 ?SetLeaveMessage@CSBClickable@@UAEXJPAX@Z
void CSBClickable::SetLeaveMessage(long message, void *data)
{
    mLeaveMessage = message;
    mLeaveData = data;
}

// 0x1001bf40 ?SetLeaveMessage@CSBClickable@@UAEXJ@Z
void CSBClickable::SetLeaveMessage(long message)
{
    mLeaveData = this;
    mLeaveMessage = message;
}

// 0x1001bf60 ?SetClickMessage@CSBClickable@@UAEXJ@Z
void CSBClickable::SetClickMessage(long message)
{
    mClickData = this;
    mClickMessage = message;
}

// 0x1001bf80 ?SetRolloverMessage@CSBClickable@@UAEXJ@Z
void CSBClickable::SetRolloverMessage(long message)
{
    mRolloverData = this;
    mRolloverMessage = message;
}

// 0x1001bfa0 ?GetOffset@CSBClickable@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBClickable::GetOffset() const
{
    return mOffset;
}

// 0x1001bfd0 ?GetPosition@CSBClickable@@UBE?AVMBPoint3@@XZ
MBPoint3 CSBClickable::GetPosition() const
{
    MBPoint3 pos = mOffset;
    if (mParent)
        pos += mParent->GetPosition();
    return pos;
}

// 0x1001c040 ?Place@CSBClickable@@UAEXABVMBPoint3@@@Z
void CSBClickable::Place(const MBPoint3 &point)
{
    mOffset = point;
    if (mParent)
        mParent->ObjectMoved((ISBClickable *)this);
    SetDirty();
}

// 0x1001c090 ?PlaceCentered@CSBClickable@@UAEXAAVMBPoint3@@@Z
void CSBClickable::PlaceCentered(MBPoint3 &point)
{
    MBRect bounds = mAnimation->GetBounds();
    Place(point - MBPoint3(bounds.Width() / 2, bounds.Height() / 2, 0));
}

// 0x1001c120 ?Move@CSBClickable@@UAEXABVMBPoint3@@@Z
void CSBClickable::Move(const MBPoint3 &point)
{
    mOffset += point;
    if (mParent)
        mParent->ObjectMoved((ISBClickable *)this);
    SetDirty();
}

// 0x1001c170 ?GetBounds@CSBClickable@@UBE?AVMBRect@@XZ
MBRect CSBClickable::GetBounds() const
{
    return mAnimation->GetBounds();
}

// 0x1001c1c0 ?IsVisible@CSBClickable@@UBE_NXZ
bool CSBClickable::IsVisible() const
{
    IMBAnim *anim = mAnimation;
    return anim->IsVisible();
}

// 0x1001c1e0 ?Show@CSBClickable@@UAEJXZ
long CSBClickable::Show()
{
    return mAnimation->Show();
}

// 0x1001c200 ?Hide@CSBClickable@@UAEJXZ
long CSBClickable::Hide()
{
    return mAnimation->Hide();
}

// 0x1001c220 ?IsDirty@CSBClickable@@UBE_NXZ
bool CSBClickable::IsDirty() const
{
    return mAnimation->IsDirty();
}

// 0x1001c240 ?SetDirty@CSBClickable@@UAEXXZ
void CSBClickable::SetDirty()
{
    mAnimation->SetDirty();
}

// 0x1001c260 ?ClearDirty@CSBClickable@@UAEXXZ
void CSBClickable::ClearDirty()
{
    mAnimation->ClearDirty();
}

// 0x1001c280 ?Draw@CSBClickable@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z
long CSBClickable::Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const
{
    return mAnimation->Draw(dest, srcRect, destRect);
}

// 0x1001c2b0 ?GetParent@CSBClickable@@UBEXPAPAVIMBObjectGroup@@@Z
void CSBClickable::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x1001c2e0 ?SetParent@CSBClickable@@UAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CSBClickable::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    mParent = 0;
    if (oldParent)
        oldParent->RemoveObject((ISBClickable *)this);
    mParent = newParent;
    return oldParent;
}

// 0x1001c330 ?GetBackground@CSBClickable@@UBEXPAPAVIMBObject@@@Z
void CSBClickable::GetBackground(IMBObject **background) const
{
    *background = 0;
}

// 0x1001c350 ?SetBackground@CSBClickable@@UAEXPAVIMBObject@@@Z
void CSBClickable::SetBackground(IMBObject *newBg)
{
}

// 0x1001c370 ?AddObject@CSBClickable@@UAEXPAVIMBObject@@@Z
void CSBClickable::AddObject(IMBObject *obj)
{
}

// 0x1001c390 ?RemoveObject@CSBClickable@@UAEXPAVIMBObject@@@Z
void CSBClickable::RemoveObject(IMBObject *obj)
{
}

// 0x1001c3b0 ?ObjectMoved@CSBClickable@@UAEXPAVIMBObject@@@Z
void CSBClickable::ObjectMoved(IMBObject *obj)
{
}

// 0x1001c3d0 ?SetAnim@CSBClickable@@UAEXPAVIMBAnim@@@Z
void CSBClickable::SetAnim(IMBAnim *anim)
{
    mAnimation = anim;
    if (mAnimation) {
        mAnimation->SetParent(this);
        mAnimation->SetCurrentSequence(0);
        mAnimation->Play();
    }
}

// 0x1001c450 ?SetAnim@CSBClickable@@UAEXPBD@Z
void CSBClickable::SetAnim(const char *animName)
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
        }
    }
    char message[80];
    sprintf(message, "NULL animation in CSBClickable:%s", animName);
}

// 0x1001c600 ?GetAnim@CSBClickable@@UAE?AV?$MBComPtr@VIMBAnim@@@@XZ
MBComPtr<IMBAnim> CSBClickable::GetAnim()
{
    return mAnimation;
}

// 0x1001c640 ?SetBroadcastChannel@CSBClickable@@UAEXJ@Z
void CSBClickable::SetBroadcastChannel(long channel)
{
    mBroadcastChannel = channel;
}

// 0x1001c660 ?GetBroadcastChannel@CSBClickable@@UAEJXZ
long CSBClickable::GetBroadcastChannel()
{
    return mBroadcastChannel;
}

// 0x1001c680 ?GetClickMessage@CSBClickable@@UAEJXZ
long CSBClickable::GetClickMessage()
{
    return mClickMessage;
}

// 0x1001c6a0 ?GetLeaveMessage@CSBClickable@@UAEJXZ
long CSBClickable::GetLeaveMessage()
{
    return mLeaveMessage;
}

// 0x1001c6c0 ?GetRolloverMessage@CSBClickable@@UAEJXZ
long CSBClickable::GetRolloverMessage()
{
    return mRolloverMessage;
}

// 0x1001c6e0 ?IsEnabled@CSBClickable@@UBE_NXZ
bool CSBClickable::IsEnabled() const
{
    return mIsEnabled;
}

// 0x1001c700 ?Enable@CSBClickable@@UAEXXZ
void CSBClickable::Enable()
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

// 0x1001c780 ?Disable@CSBClickable@@UAEXXZ
void CSBClickable::Disable()
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

// 0x1001c800 ?SetBehavior@CSBClickable@@UAEXW4MBButtonBehavior@@@Z
void CSBClickable::SetBehavior(MBButtonBehavior behavior)
{
    mBehavior = behavior;
}

// 0x1001c820 ?GetBehavior@CSBClickable@@UBE?AW4MBButtonBehavior@@XZ
MBButtonBehavior CSBClickable::GetBehavior() const
{
    return mBehavior;
}

// 0x1001c840 ?GetButtonState@CSBClickable@@UBE?AW4MBButtonState@@XZ
MBButtonState CSBClickable::GetButtonState() const
{
    return mButtonState;
}

// 0x1001c860 ?SetButtonState@CSBClickable@@UAEXW4MBButtonState@@@Z
void CSBClickable::SetButtonState(MBButtonState buttonState)
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

// 0x1001c8e0 ?SetSound@CSBClickable@@UAEXW4MBButtonDrawState@@PBDJ@Z
void CSBClickable::SetSound(MBButtonDrawState drawState, const char *soundName, long soundChannel)
{
    mDrawStateSounds[drawState].soundID = MBHashString(soundName);
    mDrawStateSounds[drawState].soundChannel = soundChannel;
}

// 0x1001c920 ?IsRolledOver@CSBClickable@@UBE_NABVMBPoint@@@Z
bool CSBClickable::IsRolledOver(const MBPoint &world_mouse_pos) const
{
    return IsActive() && IsVisible() && IsWorldPointInAnim(world_mouse_pos);
}

// 0x1001c970 ?EnterRollover@CSBClickable@@UAEXABVMBPoint@@@Z
void CSBClickable::EnterRollover(const MBPoint &world_mouse_pos)
{
    if (!IsEnabled())
        return;
    if (!mIsTrackingClick && mBroadcastChannel != -1 && mRolloverMessage != -1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mRolloverMessage, mRolloverData);
    }
}

// 0x1001caa0 ?WithinRollover@CSBClickable@@UAEXABVMBPoint@@@Z
void CSBClickable::WithinRollover(const MBPoint &world_mouse_pos)
{
}

// 0x1001cac0 ?LeaveRollover@CSBClickable@@UAEXXZ
void CSBClickable::LeaveRollover()
{
    if (mBroadcastChannel != -1 && mLeaveMessage != -1) {
        MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
        broadcaster->BroadcastEvent(mBroadcastChannel, mLeaveMessage, mLeaveData);
    }
}

// 0x1001cbf0 ?IsClicked@CSBClickable@@UBE_NABVMBPoint@@F@Z
bool CSBClickable::IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const
{
    bool isClicked = false;
    MBRect bounds;
    if (mAnimation == 0 || !IsActive() || !IsVisible())
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

// 0x1001cdf0 ?IsTrackingClick@CSBClickable@@UBE_NXZ
bool CSBClickable::IsTrackingClick() const
{
    return mIsTrackingClick;
}

// 0x1001ce10 ?StartTrackingClick@CSBClickable@@UAEXABVMBPoint@@F@Z
void CSBClickable::StartTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    IMBAnim *anim = mAnimation;
    unsigned long sequence = anim->GetCurrentSequenceIndex();
    unsigned short endAction;
    anim->GetSequenceProperty(sequence, kSequenceEndAction, endAction);
    if (!anim->IsPlaying() || anim->IsStalled() || endAction == 2) {
        sequence++;
        if (sequence >= anim->GetNumSequences())
            sequence = 0;
        anim->SetCurrentSequence(sequence);
        mIsTrackingClick = true;
    }
}

// 0x1001ceb0 ?TrackClick@CSBClickable@@UAEXABVMBPoint@@@Z
void CSBClickable::TrackClick(const MBPoint &world_mouse_pos)
{
}

// 0x1001ced0 ?StopTrackingClick@CSBClickable@@UAEXABVMBPoint@@F@Z
void CSBClickable::StopTrackingClick(const MBPoint &world_mouse_pos, short button)
{
    mIsTrackingClick = false;
}

// 0x1001cef0 ?IsActive@CSBClickable@@UBE_NXZ
bool CSBClickable::IsActive() const
{
    return mIsActive;
}

// 0x1001cf10 ?Activate@CSBClickable@@UAEXXZ
void CSBClickable::Activate()
{
    mIsActive = true;
}

// 0x1001cf30 ?Deactivate@CSBClickable@@UAEXXZ
void CSBClickable::Deactivate()
{
    mIsActive = false;
}

// Adjustor thunks of the IMBMouseable and IMBObjectGroup vftables (emitted with the vftables):

// 0x1001cf50 ?IsWorldPointInAnim@CSBClickable@@IBE_NABVMBPoint@@@Z
bool CSBClickable::IsWorldPointInAnim(const MBPoint &worldPoint) const
{
    bool result = false;
    if (mAnimation) {
        MBRect bounds = mAnimation->GetBounds();
        result = bounds.ContainsPoint(worldPoint);
    }
    return result;
}

// 0x1001cfd0 ?GetUpSequenceIndex@CSBClickable@@IBEJXZ
long CSBClickable::GetUpSequenceIndex() const
{
    if (!IsEnabled() && mDrawStateSequences[kButtonDisabledDrawState] != -1)
        return mDrawStateSequences[kButtonDisabledDrawState];
    return mDrawStateSequences[kButtonUpDrawState];
}

// 0x1001d000 ?GetDownSequenceIndex@CSBClickable@@IBEJXZ
long CSBClickable::GetDownSequenceIndex() const
{
    if (!IsEnabled()) {
        if (mDrawStateSequences[kButtonDisabledDownDrawState] != -1)
            return mDrawStateSequences[kButtonDisabledDownDrawState];
        if (mDrawStateSequences[kButtonDisabledDrawState] != -1)
            return mDrawStateSequences[kButtonDisabledDrawState];
    }
    return mDrawStateSequences[kButtonDownDrawState];
}

// 0x1001d040 ?LoadFromStream@CSBClickable@@UAEHV?$MBComPtr@VIMBStream@@@@ABV?$MBComPtr@VIMBPref@@@@PBD@Z
int CSBClickable::LoadFromStream(const MBComPtr<IMBStream> stream, const MBComPtr<IMBPref> &pref, const char *key)
{
    // the clickable's section of a scene file: anim, position, behavior, click data and the
    // properties that decide whether it is shown
    MBString anim;
    pref->GetString(stream, MBString(key), MBString("anim"), anim, MBString(""));
    if (anim == "")
        return 0;
    SetAnim(anim.c_str());
    long x = pref->GetLong(stream, MBString(key), MBString("x0"), 0);
    long y = pref->GetLong(stream, MBString(key), MBString("y0"), 0);
    long z = pref->GetLong(stream, MBString(key), MBString("z0"), 10);
    Place(MBPoint3(x, y, z));
    long behavior = pref->GetLong(stream, MBString(key), MBString("button behavior"), 3);
    SetBehavior((MBButtonBehavior)behavior);
    MBString action;
    MBString data;
    pref->GetString(stream, MBString(key), MBString("action"), action, MBString(""));
    pref->GetString(stream, MBString(key), MBString("data"), data, MBString(""));
    mEventData.cursorID = pref->GetLong(stream, MBString(key), MBString("cursor"), 0);
    mEventData.action = action;
    mEventData.data = data;
    SetBroadcastChannel(200);
    SetRolloverMessage(202, &mEventData);
    SetLeaveMessage(203);
    if (action.compare("exit") == 0)
        SetClickMessage(201, &mEventData);
    pref->GetString(stream, MBString(key), MBString("if property"), mIfProperty, MBString("null"));
    pref->GetString(stream, MBString(key), MBString("if not property"), mIfNotProperty, MBString("null"));
    return 1;
}

// 0x1001df90 ?QueryInterface@CSBClickable@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1001dfa0 ?AddRef@CSBClickable@@W3AEKXZ
// 0x1001dfb0 ?Release@CSBClickable@@W3AEKXZ
// 0x1001dfc0 ?QueryInterface@CSBClickable@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1001dfd0 ?AddRef@CSBClickable@@W7AEKXZ
// 0x1001dfe0 ?Release@CSBClickable@@W7AEKXZ
// 0x1001dff0 ?GetOffset@CSBClickable@@W7BE?AVMBPoint3@@XZ
// 0x1001e000 ?GetPosition@CSBClickable@@W7BE?AVMBPoint3@@XZ
// 0x1001e010 ?Place@CSBClickable@@W7AEXABVMBPoint3@@@Z
// 0x1001e020 ?Move@CSBClickable@@W7AEXABVMBPoint3@@@Z
// 0x1001e030 ?GetBounds@CSBClickable@@W7BE?AVMBRect@@XZ
// 0x1001e040 ?IsVisible@CSBClickable@@W7BE_NXZ
// 0x1001e050 ?Show@CSBClickable@@W7AEJXZ
// 0x1001e060 ?Hide@CSBClickable@@W7AEJXZ
// 0x1001e070 ?IsDirty@CSBClickable@@W7BE_NXZ
// 0x1001e080 ?SetDirty@CSBClickable@@W7AEXXZ
// 0x1001e090 ?ClearDirty@CSBClickable@@W7AEXXZ
// 0x1001e0a0 ?Draw@CSBClickable@@W7BEJPAVIMBSurface@@AAVMBRect@@1@Z
// 0x1001e0b0 ?GetParent@CSBClickable@@W7BEXPAPAVIMBObjectGroup@@@Z
// 0x1001e0c0 ?SetParent@CSBClickable@@W7AEPAVIMBObjectGroup@@PAV2@@Z
// 0x1001dee0 ??1?$MBComPtr@VIMBAnim@@@@QAE@XZ

// 0x1001df10 _$E26
// 0x1001df50 _$E22
// 0x1001df70 _$E25
