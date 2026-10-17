// MBEngine.dll unit MBAnimFrame (.\MBEngineRelease\MBAnimFrame.obj)
#include "MBAnimFrame_unit.h"

// PDB names these without C++ decoration: C linkage, file scope
extern "C" {
static void SetBit(unsigned long &bitField, unsigned long bit, bool value);
static bool TestBit(unsigned long bitField, unsigned long bit);
}

// 0x1002e2d0 ??0MBAnimFrame@@QAE@XZ
MBAnimFrame::MBAnimFrame(void)
    : mCastID(0), mProperties(0), mStartSound(0), mStartAnim(0), mStopAnim(0),
      mStartScript(0), mStopScript(0), mFrameDelay(100), mStartAnimSequence(0xffff),
      mNotify(false), mScriptData(0)
{
    _mRefCnt = 0;
}

// 0x1002e310 ??1MBAnimFrame@@AAE@XZ
MBAnimFrame::~MBAnimFrame(void)
{
}

// 0x1002e320 ?SetFrameProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@ABVMBPoint@@@Z
long MBAnimFrame::SetFrameProperty(MBFrameProperty property, const MBPoint &value)
{
    long result = 0;
    switch (property) {
    case kFrameCastMemberOffset:
        mCastMemberOffset = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e350 ?SetFrameProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
long MBAnimFrame::SetFrameProperty(MBFrameProperty property, const MBString &value)
{
    long result = 0;
    switch (property) {
    case kFrameStartSound:
        mStartSound = MBHashString(value.c_str());
        break;
    case kFrameStartAnimName:
        mStartAnim = MBHashString(value.c_str());
        break;
    case kFrameStopAnim:
        mStopAnim = MBHashString(value.c_str());
        break;
    case kFrameStartScript:
        mStartScript = MBHashString(value.c_str());
        break;
    case kFrameStopScript:
        mStopScript = MBHashString(value.c_str());
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e450 ?SetFrameProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@K@Z
long MBAnimFrame::SetFrameProperty(MBFrameProperty property, unsigned long value)
{
    long result = 0;
    switch (property) {
    case kFrameStartSound:
        mStartSound = value;
        break;
    case kFrameStartAnimName:
        mStartAnim = value;
        break;
    case kFrameStopAnim:
        mStopAnim = value;
        break;
    case kFrameStartScript:
        mStartScript = value;
        break;
    case kFrameStopScript:
        mStopScript = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e4c0 ?SetFrameProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@G@Z
long MBAnimFrame::SetFrameProperty(MBFrameProperty property, unsigned short value)
{
    long result = 0;
    switch (property) {
    case kFrameDelay:
        mFrameDelay = value;
        break;
    case kFrameStartAnimSequence:
        mStartAnimSequence = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e4f0 ?SetFrameProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@_N@Z
long MBAnimFrame::SetFrameProperty(MBFrameProperty property, bool value)
{
    long result = 0;
    switch (property) {
    case kFrameNotify:
        mNotify = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e510 ?GetFrameProperty@MBAnimFrame@@QBEJW4MBFrameProperty@@AAVMBPoint@@@Z
long MBAnimFrame::GetFrameProperty(MBFrameProperty property, MBPoint &value) const
{
    long result = 0;
    switch (property) {
    case kFrameCastMemberOffset:
        value = mCastMemberOffset;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e540 ?GetFrameProperty@MBAnimFrame@@QBEJW4MBFrameProperty@@AAK@Z
long MBAnimFrame::GetFrameProperty(MBFrameProperty property, unsigned long &value) const
{
    long result = 0;
    switch (property) {
    case kFrameStartSound:
        value = mStartSound;
        break;
    case kFrameStartAnimName:
        value = mStartAnim;
        break;
    case kFrameStopAnim:
        value = mStopAnim;
        break;
    case kFrameStartScript:
        value = mStartScript;
        break;
    case kFrameStopScript:
        value = mStopScript;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e5c0 ?GetFrameProperty@MBAnimFrame@@QBEJW4MBFrameProperty@@AAG@Z
long MBAnimFrame::GetFrameProperty(MBFrameProperty property, unsigned short &value) const
{
    long result = 0;
    switch (property) {
    case kFrameDelay:
        value = mFrameDelay;
        break;
    case kFrameStartAnimSequence:
        value = mStartAnimSequence;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e600 ?GetFrameProperty@MBAnimFrame@@QBEJW4MBFrameProperty@@AA_N@Z
long MBAnimFrame::GetFrameProperty(MBFrameProperty property, bool &value) const
{
    long result = 0;
    switch (property) {
    case kFrameNotify:
        value = mNotify;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e620 ?UseProperty@MBAnimFrame@@QAEJW4MBFrameProperty@@_N@Z
long MBAnimFrame::UseProperty(MBFrameProperty property, bool use)
{
    long result = 0;
    switch (property) {
    case kFrameDelay:
        SetBit(mProperties, 0x01, use);
        break;
    case kFrameStartSound:
        SetBit(mProperties, 0x04, use);
        break;
    case kFrameStartAnim:
        SetBit(mProperties, 0x08, use);
        break;
    case kFrameStopAnim:
        SetBit(mProperties, 0x10, use);
        break;
    case kFrameStartScript:
        SetBit(mProperties, 0x20, use);
        break;
    case kFrameStopScript:
        SetBit(mProperties, 0x40, use);
        break;
    case kFrameCastMemberOffset:
        SetBit(mProperties, 0x80, use);
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002e720 ?HasProperty@MBAnimFrame@@QBE_NW4MBFrameProperty@@@Z
bool MBAnimFrame::HasProperty(MBFrameProperty property) const
{
    bool result = false;
    switch (property) {
    case kFrameNotify:
        result = true;
        break;
    case kFrameDelay:
        result = TestBit(mProperties, 0x01);
        break;
    case kFrameStartSound:
        result = TestBit(mProperties, 0x04);
        break;
    case kFrameStartAnim:
    case kFrameStartAnimName:
    case kFrameStartAnimSequence:
        result = TestBit(mProperties, 0x08);
        break;
    case kFrameStopAnim:
        result = TestBit(mProperties, 0x10);
        break;
    case kFrameStartScript:
        result = TestBit(mProperties, 0x20);
        break;
    case kFrameStopScript:
        result = TestBit(mProperties, 0x40);
        break;
    case kFrameCastMemberOffset:
        result = TestBit(mProperties, 0x80);
        break;
    }
    return result;
}

// 0x1002e7e0 ?GetCastID@MBAnimFrame@@QBEJXZ
long MBAnimFrame::GetCastID(void) const
{
    return mCastID;
}

// 0x1002e7f0 ?SetCastID@MBAnimFrame@@QAEXJ@Z
void MBAnimFrame::SetCastID(long castID)
{
    mCastID = castID;
}

// 0x1002e800 _SetBit (original SetBit)
static void SetBit(unsigned long &bitField, unsigned long bit, bool value)
{
    if (value)
        bitField |= bit;
    else
        bitField &= ~bit;
}

// 0x1002e820 _TestBit (original TestBit)
static bool TestBit(unsigned long bitField, unsigned long bit)
{
    return (bitField & bit) != 0;
}

// 0x1002e870 ?QueryInterface@MBAnimFrame@@UAEJABUMBGUID@@PAPAX@Z
long MBAnimFrame::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    return -3;
}

// 0x1002e890 ?AddRef@MBAnimFrame@@UAEKXZ
unsigned long MBAnimFrame::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002e8a0 ?Release@MBAnimFrame@@UAEKXZ
unsigned long MBAnimFrame::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}


// Matched by this code:
// 0x1001dc10 SetBit
// 0x1001dc30 TestBit
// 0x1002e840 _$E20
// 0x1002e860 _$E19
