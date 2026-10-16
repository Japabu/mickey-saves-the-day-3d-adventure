// CMBTextControl.cpp - MBEngine.dll CMBTextControl.obj
#include "CMBTextControl.h"

// 0x10027f40 ?MakeCMBTextControl@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBTextControl(void)
{
    IMBUnknown *p = (IMBTextControl *)new CMBTextControl;
    if (p)
        p->AddRef();
    return p;
}

// 0x10027f70 ??0CMBTextControl@@QAE@XZ
CMBTextControl::CMBTextControl(void)
    : mParent(0), mPos(0, 0, 100), mHideCount(0), mDirty(true), mSize(0, 0), mColorFlags(0),
      mCharLimit(0xffffffff), mCursorBlinkRate(700), mCursorBlinkOn(false), mTextControlType(kTCTStatic),
      mJustification(kTCJLeft), mLastTime(0)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    broadcaster->AddListener(-1, this);
}

// 0x10028120 ?QueryInterface@CMBTextControl@@UAEJABUMBGUID@@PAPAX@Z
long CMBTextControl::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (iid == MBIID_IMBTextControl)
        p = (IMBTextControl *)this;
    if (iid == MBIID_IMBEventListener)
        p = (IMBEventListener *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (iid == MBIID_IMBUnknown)
        p = (IMBTextControl *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x100282b0 ?AddRef@CMBTextControl@@UAEKXZ
unsigned long CMBTextControl::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x100282c0 ?Release@CMBTextControl@@UAEKXZ
unsigned long CMBTextControl::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// 0x100282f0 ?GetOffset@CMBTextControl@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBTextControl::GetOffset(void) const
{
    return mPos;
}

// 0x10028310 ?GetPosition@CMBTextControl@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBTextControl::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x10028380 ?Place@CMBTextControl@@UAEXABVMBPoint3@@@Z
void CMBTextControl::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x100283c0 ?Move@CMBTextControl@@UAEXABVMBPoint3@@@Z
void CMBTextControl::Move(const MBPoint3 &point)
{
    mPos.x += point.x;
    mPos.y += point.y;
    mPos.z += point.z;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x10028400 ?IsVisible@CMBTextControl@@UBE_NXZ
bool CMBTextControl::IsVisible(void) const
{
    return mHideCount == 0;
}

// 0x10028410 ?IsDirty@CMBTextControl@@UBE_NXZ
bool CMBTextControl::IsDirty(void) const
{
    return mDirty;
}

// 0x10028420 ?SetDirty@CMBTextControl@@UAEXXZ
void CMBTextControl::SetDirty(void)
{
    mDirty = true;
}

// 0x10028430 ?ClearDirty@CMBTextControl@@UAEXXZ
void CMBTextControl::ClearDirty(void)
{
    mDirty = false;
}

// 0x10028440 ??1CMBTextControl@@IAE@XZ
CMBTextControl::~CMBTextControl(void)
{
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBComPtr<IMBEventManager> broadcaster(MBIID_IMBEventManager);
    if (broadcaster)
        broadcaster->RemoveListener(-1, this);
}

// 0x10028590 ?SetFont@CMBTextControl@@UAEXPAVIMBFont@@@Z
void CMBTextControl::SetFont(IMBFont *font)
{
    mFont = font;
    SetDirty();
}

// 0x100285d0 ?GetFont@CMBTextControl@@UAEXPAPAVIMBFont@@@Z
void CMBTextControl::GetFont(IMBFont **font)
{
    *font = mFont;
}

// 0x100285e0 ?GetText@CMBTextControl@@UAEXAAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBTextControl::GetText(MBString &text)
{
    MB_ucs2_to_utf8(mUniString, text);
}

// 0x10028600 ?SetText@CMBTextControl@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
void CMBTextControl::SetText(const MBString &text)
{
    MB_utf8_to_ucs2(text, mUniString);
    SetText(mUniString);
}

// 0x10028630 ?GetText@CMBTextControl@@UAEXAAV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
void CMBTextControl::GetText(MBString16 &text)
{
    text = mUniString;
}

// 0x10028780 ?SetText@CMBTextControl@@UAEXABV?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@@Z
void CMBTextControl::SetText(const MBString16 &text)
{
    mUniString = text.substr(0, mCharLimit);
    SetDirty();
}

// 0x100288c0 ?SetSize@CMBTextControl@@UAEXJJ@Z
void CMBTextControl::SetSize(long width, long height)
{
    mSize.cx = width;
    mSize.cy = height;
    SetDirty();
}

// 0x100288e0 ?GetSize@CMBTextControl@@UBE?AVMBSize@@XZ
MBSize CMBTextControl::GetSize(void) const
{
    return mSize;
}

// 0x10028900 ?GetBounds@CMBTextControl@@UBE?AVMBRect@@XZ
MBRect CMBTextControl::GetBounds(void) const
{
    return MBRect(GetPosition(), GetSize());
}

// 0x10028950 ?Hide@CMBTextControl@@UAEJXZ
long CMBTextControl::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x10028970 ?Show@CMBTextControl@@UAEJXZ
long CMBTextControl::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x100289a0 ?GetParent@CMBTextControl@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBTextControl::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x100289c0 ?SetParent@CMBTextControl@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBTextControl::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// TODO 0x10028a00 ?Draw@CMBTextControl@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z: register allocation: edi and ebp swapped throughout (64% similar)
long CMBTextControl::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const
{
    if (IsVisible()) {
        if (mColorFlags & kTCFBackgroundColor)
            dest->ColorFill(mBackgroundColor, &destRect);
        if (mColorFlags & (kTCFBorderColor | kTCFActiveBorderColor)) {
            MBRect border;
            const MBColor &color = (mTextControlType == kTCTEditable && (mColorFlags & kTCFActiveBorderColor))
                ? mActiveBorderColor : mBorderColor;
            border = destRect;
            border.bottom = destRect.top + 1;
            dest->ColorFill(color, &border);
            border = destRect;
            border.top = destRect.bottom - 1;
            dest->ColorFill(color, &border);
            border = destRect;
            border.right = destRect.left + 1;
            dest->ColorFill(color, &border);
            border = destRect;
            border.left = destRect.right - 1;
            dest->ColorFill(color, &border);
        }
        if (mFont) {
            MBSize size(0, 0);
            mFont->GetTextSize(dest, mUniString.c_str(), mUniString.length(), size);
            MBRect bounds = GetBounds();
            bounds.left += 1;
            bounds.top += 1;
            bounds.right -= 1;
            bounds.bottom -= 1;
            switch (mJustification) {
            case kTCJRight:
                if (size.cx + 2 <= bounds.right - bounds.left)
                    bounds.left = bounds.right - size.cx - 2;
                break;
            case kTCJCenter: {
                long width = bounds.right - bounds.left;
                if (size.cx <= width)
                    bounds.left += (width - size.cx) / 2;
                break;
            }
            }
            long result = mFont->DrawText(dest, bounds, mUniString.c_str(), mUniString.length());
            if (result == 0 && mCursorBlinkOn && mTextControlType != kTCTStatic) {
                MBRect cursorRect(bounds.left + size.cx, destRect.top, bounds.left + size.cx + 2, destRect.top + size.cy);
                cursorRect.left = MBmax(cursorRect.left, destRect.left);
                cursorRect.top = MBmax(cursorRect.top, destRect.top);
                cursorRect.right = MBmin(cursorRect.right, destRect.right);
                cursorRect.bottom = MBmin(cursorRect.bottom, destRect.bottom);
                result = dest->ColorFill(kWhite, &cursorRect);
            }
            return result;
        }
    }
    return 0;
}

// 0x10028d30 ?ListenToEvent@CMBTextControl@@UAEXJJPAX@Z
void CMBTextControl::ListenToEvent(long channel, long event, void *data)
{
    if (channel == -1 && event == 4)
        AddCharacter(*(unsigned short *)data);
}

// 0x10028d60 ?AddCharacter@CMBTextControl@@MAEXG@Z
void CMBTextControl::AddCharacter(unsigned short character)
{
    static const unsigned short kKeyBackspace = 8;
    static MBString16 newstring;
    if (mTextControlType == kTCTStatic)
        return;
    newstring = mUniString;
    unsigned long length = mUniString.length();
    if (character == kKeyBackspace) {
        if (length > 0)
            newstring.erase(length - 1, 1);
    } else if (length < mCharLimit) {
        newstring += character;
    }
    if (mValidatorPtr) {
        TVState state = mValidatorPtr->Validate(newstring);
        if (state == kTVValid || state == kTVAcceptable)
            SetText(newstring);
    } else
        SetText(newstring);
}

// 0x10028f80 ?SetColor@CMBTextControl@@UAEXW4MBTextControlColorFlag@@ABVMBColor@@@Z
void CMBTextControl::SetColor(MBTextControlColorFlag colorflag, const MBColor &color)
{
    mColorFlags |= colorflag;
    switch (colorflag) {
    case kTCFActiveBorderColor:
        mActiveBorderColor = color;
        break;
    case kTCFBorderColor:
        mBorderColor = color;
        break;
    case kTCFBackgroundColor:
        mBackgroundColor = color;
        break;
    }
}

// 0x10028fd0 ?SetColorTransparent@CMBTextControl@@UAEXW4MBTextControlColorFlag@@@Z
void CMBTextControl::SetColorTransparent(MBTextControlColorFlag colorflag)
{
    mColorFlags ^= colorflag;
}

// 0x10028fe0 ?SetCharacterLimit@CMBTextControl@@UAEXK@Z
void CMBTextControl::SetCharacterLimit(unsigned long limit)
{
    mCharLimit = limit;
    mUniString = mUniString.substr(0, limit);
}

// 0x10029110 ?Idle@CMBTextControl@@UAEXXZ
void CMBTextControl::Idle(void)
{
    MBComPtr<IMBGameClock> clock(MBIID_IMBGameClock);
    if (clock->GetGameTime() - mLastTime > mCursorBlinkRate) {
        mLastTime = clock->GetGameTime();
        mCursorBlinkOn = !mCursorBlinkOn;
    }
}

// 0x100291c0 ?SetType@CMBTextControl@@UAEXW4MBTextControlTypeFlag@@@Z
void CMBTextControl::SetType(MBTextControlTypeFlag type)
{
    mTextControlType = type;
}

// 0x100291d0 ?SetValidator@CMBTextControl@@UAEXPAVIMBTextValidator@@@Z
void CMBTextControl::SetValidator(IMBTextValidator *validator)
{
    mValidatorPtr = validator;
}

// 0x10029200 ?SetJustification@CMBTextControl@@UAEXW4MBTextControlJustifyFlag@@@Z
void CMBTextControl::SetJustification(MBTextControlJustifyFlag justify)
{
    mJustification = justify;
}


// Matched by this code:
// 0x1001ea00 ?Release@?$MBComPtr@VIMBGameClock@@@@QAEXXZ
// 0x10028ce0 _wmemmove
// 0x10028f30 _$E65
// 0x10029210 ?_Split@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@AAEXXZ
// 0x100292f0 ?assign@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QAEAAV12@ABV12@II@Z
// 0x10029510 ?max_size@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QBEIXZ
// 0x10029520 ?assign@?$basic_string@GU?$char_traits@G@std@@V?$MBAllocator@G@@@std@@QAEAAV12@PBGI@Z
// 0x100295d0 _$E16
// 0x100296b0 _$E19
// 0x10029790 _$E22
// 0x10029870 _$E25
// 0x10029940 _$E28
// 0x10029a40 ?QueryInterface@CMBTextControl@@W3AEJABUMBGUID@@PAPAX@Z
// 0x10029a50 ?AddRef@CMBTextControl@@W3AEKXZ
// 0x10029a60 ?Release@CMBTextControl@@W3AEKXZ
// 0x10029a70 ?QueryInterface@CMBTextControl@@W7AEJABUMBGUID@@PAPAX@Z
// 0x10029a80 ?AddRef@CMBTextControl@@W7AEKXZ
// 0x10029a90 ?Release@CMBTextControl@@W7AEKXZ
// 0x10029aa0 _$E31
// 0x10029b60 _$E34
// 0x10029c20 _$E37
// 0x10029ce0 _$E40
// 0x10029db0 _$E43
// 0x10029e80 _$E46
// 0x10029f40 _$E49
// 0x1002a000 _$E52
// 0x1002a0c0 _$E55
// 0x1002a180 _$E58
// 0x1002a240 _$E61
