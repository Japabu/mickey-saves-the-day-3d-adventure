// MBEngine.dll unit MBAnimSequence (.\MBEngineRelease\MBAnimSequence.obj)
#include "MBAnimSequence.h"

// PDB names these without C++ decoration: C linkage, file scope
extern "C" {
static void SetBit(unsigned long &bitField, unsigned long bit, bool value);
static bool TestBit(unsigned long bitField, unsigned long bit);
}

// 0x1002e8d0 ??0MBAnimSequence@@QAE@XZ
MBAnimSequence::MBAnimSequence(void)
    : mZPlane(100), mDelay(100), mProperties(0), mSequenceEndAction(1),
      mJumpSequence(0xffff), mJumpRandomSequenceMin(0xffff), mJumpRandomSequenceMax(0xffff),
      mTileWidth(0), mTileHeight(0), mInSync(true)
{
    _mRefCnt = 0;
}

// 0x1002e930 ??1MBAnimSequence@@AAE@XZ
MBAnimSequence::~MBAnimSequence(void)
{
    for (MBVector<MBAnimFrame *>::iterator it = mFrames.begin(); it != mFrames.end(); ++it)
        (*it)->Release();
}

// 0x1002e980 ?SetCastID@MBAnimSequence@@QAEXJJ@Z
void MBAnimSequence::SetCastID(long frameIndex, long castID)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        frame->SetCastID(castID);
}

// 0x1002e9a0 ?GetCastID@MBAnimSequence@@QBEJJ@Z
long MBAnimSequence::GetCastID(long frameIndex) const
{
    long castID = 0;
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        castID = frame->GetCastID();
    return castID;
}

// 0x1002e9d0 ?SetSequenceProperty@MBAnimSequence@@QAEJW4MBSequenceProperty@@G@Z
long MBAnimSequence::SetSequenceProperty(MBSequenceProperty property, unsigned short value)
{
    long result = 0;
    switch (property) {
    case kSequenceZPlane:
        mZPlane = value;
        break;
    case kSequenceDelay:
        mDelay = value;
        break;
    case kSequenceEndAction:
        mSequenceEndAction = value;
        break;
    case kSequenceJumpSequence:
        mJumpSequence = value;
        break;
    case kSequenceJumpRandomSequenceMin:
        mJumpRandomSequenceMin = value;
        break;
    case kSequenceJumpRandomSequenceMax:
        mJumpRandomSequenceMax = value;
        break;
    case kSequenceTileWidth:
        mTileWidth = value;
        break;
    case kSequenceTileHeight:
        mTileHeight = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002ea70 ?SetSequenceProperty@MBAnimSequence@@QAEJW4MBSequenceProperty@@_N@Z
long MBAnimSequence::SetSequenceProperty(MBSequenceProperty property, bool value)
{
    long result = 0;
    switch (property) {
    case kSequenceInSync:
        mInSync = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002ea90 ?SetFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@ABVMBPoint@@@Z
long MBAnimSequence::SetFrameProperty(long frameIndex, MBFrameProperty property, const MBPoint &value)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->SetFrameProperty(property, value);
    return -11011;
}

// 0x1002eac0 ?SetFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
long MBAnimSequence::SetFrameProperty(long frameIndex, MBFrameProperty property, const MBString &value)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->SetFrameProperty(property, value);
    return -11011;
}

// 0x1002eaf0 ?SetFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@K@Z
long MBAnimSequence::SetFrameProperty(long frameIndex, MBFrameProperty property, unsigned long value)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->SetFrameProperty(property, value);
    return -11011;
}

// 0x1002eb20 ?SetFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@G@Z
long MBAnimSequence::SetFrameProperty(long frameIndex, MBFrameProperty property, unsigned short value)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->SetFrameProperty(property, value);
    return -11011;
}

// 0x1002eb50 ?SetFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@_N@Z
long MBAnimSequence::SetFrameProperty(long frameIndex, MBFrameProperty property, bool value)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->SetFrameProperty(property, value);
    return -11011;
}

// 0x1002eb80 ?GetSequenceProperty@MBAnimSequence@@QBEJW4MBSequenceProperty@@AAG@Z
long MBAnimSequence::GetSequenceProperty(MBSequenceProperty property, unsigned short &value) const
{
    long result = 0;
    switch (property) {
    case kSequenceZPlane:
        if (HasProperty(kSequenceZPlane))
            value = mZPlane;
        else
            value = 0;
        break;
    case kSequenceDelay:
        if (HasProperty(kSequenceDelay))
            value = mDelay;
        else
            value = 0;
        break;
    case kSequenceEndAction:
        value = mSequenceEndAction;
        break;
    case kSequenceJumpSequence:
        value = mJumpSequence;
        break;
    case kSequenceJumpRandomSequenceMin:
        value = mJumpRandomSequenceMin;
        break;
    case kSequenceJumpRandomSequenceMax:
        value = mJumpRandomSequenceMax;
        break;
    case kSequenceTileWidth:
        value = mTileWidth;
        break;
    case kSequenceTileHeight:
        value = mTileHeight;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002ec90 ?GetSequenceProperty@MBAnimSequence@@QBEJW4MBSequenceProperty@@AA_N@Z
long MBAnimSequence::GetSequenceProperty(MBSequenceProperty property, bool &value) const
{
    long result = 0;
    switch (property) {
    case kSequenceInSync:
        value = mInSync;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1002ecb0 ?GetFrameProperty@MBAnimSequence@@QBEJJW4MBFrameProperty@@AAVMBPoint@@@Z
long MBAnimSequence::GetFrameProperty(long frameIndex, MBFrameProperty property, MBPoint &value) const
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->GetFrameProperty(property, value);
    return -11011;
}

// 0x1002ece0 ?GetFrameProperty@MBAnimSequence@@QBEJJW4MBFrameProperty@@AAK@Z
long MBAnimSequence::GetFrameProperty(long frameIndex, MBFrameProperty property, unsigned long &value) const
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->GetFrameProperty(property, value);
    return -11011;
}

// 0x1002ed10 ?GetFrameProperty@MBAnimSequence@@QBEJJW4MBFrameProperty@@AAG@Z
long MBAnimSequence::GetFrameProperty(long frameIndex, MBFrameProperty property, unsigned short &value) const
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->GetFrameProperty(property, value);
    return -11011;
}

// 0x1002ed40 ?GetFrameProperty@MBAnimSequence@@QBEJJW4MBFrameProperty@@AA_N@Z
long MBAnimSequence::GetFrameProperty(long frameIndex, MBFrameProperty property, bool &value) const
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->GetFrameProperty(property, value);
    return -11011;
}

// 0x1002ed70 ?UseProperty@MBAnimSequence@@QAEJW4MBSequenceProperty@@_N@Z
long MBAnimSequence::UseProperty(MBSequenceProperty property, bool use)
{
    long result = 0;
    switch (property) {
    case kSequenceZPlane:
        SetBit(mProperties, 0x01, use);
        break;
    case kSequenceDelay:
        SetBit(mProperties, 0x02, use);
        break;
    case kSequenceTileWidth:
        SetBit(mProperties, 0x04, use);
        break;
    case kSequenceTileHeight:
        SetBit(mProperties, 0x08, use);
        break;
    default:
        result = -1;
        break;
    }
    return result;
}

// 0x1002ee20 ?HasProperty@MBAnimSequence@@QBE_NW4MBSequenceProperty@@@Z
bool MBAnimSequence::HasProperty(MBSequenceProperty property) const
{
    bool result = true;
    switch (property) {
    case kSequenceZPlane:
        result = TestBit(mProperties, 0x01);
        break;
    case kSequenceDelay:
        result = TestBit(mProperties, 0x02);
        break;
    case kSequenceTileWidth:
        result = TestBit(mProperties, 0x04);
        break;
    case kSequenceTileHeight:
        result = TestBit(mProperties, 0x08);
        break;
    }
    return result;
}

// 0x1002eea0 ?UseFrameProperty@MBAnimSequence@@QAEJJW4MBFrameProperty@@_N@Z
long MBAnimSequence::UseFrameProperty(long frameIndex, MBFrameProperty property, bool use)
{
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        return frame->UseProperty(property, use);
    return -11011;
}

// 0x1002eed0 ?HasFrameProperty@MBAnimSequence@@QBE_NJW4MBFrameProperty@@@Z
bool MBAnimSequence::HasFrameProperty(long frameIndex, MBFrameProperty property) const
{
    bool result = false;
    MBAnimFrame *frame = GetAnimFrame(frameIndex);
    if (frame)
        result = frame->HasProperty(property);
    return result;
}

// 0x1002ef00 ?GetNumFrames@MBAnimSequence@@QBEJXZ
long MBAnimSequence::GetNumFrames(void) const
{
    return mFrames.size();
}

// 0x1002ef20 ?AddFrame@MBAnimSequence@@QAEJXZ
long MBAnimSequence::AddFrame(void)
{
    long result = -1;
    MBAnimFrame *frame = new MBAnimFrame;
    if (frame) {
        frame->AddRef();
        mFrames.push_back(frame);
        result = GetNumFrames() - 1;
    }
    return result;
}

// 0x1002f110 ?GetAnimFrame@MBAnimSequence@@IBEPAVMBAnimFrame@@J@Z
MBAnimFrame *MBAnimSequence::GetAnimFrame(long frameIndex) const
{
    MBAnimFrame *frame = 0;
    if (frameIndex >= 0 && frameIndex < GetNumFrames())
        frame = mFrames[frameIndex];
    return frame;
}

// 0x1002f140 _SetBit (original SetBit)
static void SetBit(unsigned long &bitField, unsigned long bit, bool value)
{
    if (value)
        bitField |= bit;
    else
        bitField &= ~bit;
}

// 0x1002f160 _TestBit (original TestBit)
static bool TestBit(unsigned long bitField, unsigned long bit)
{
    return (bitField & bit) != 0;
}

// 0x1002f240 ?QueryInterface@MBAnimSequence@@UAEJABUMBGUID@@PAPAX@Z
long MBAnimSequence::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    return -3;
}

// 0x1002f260 ?AddRef@MBAnimSequence@@UAEKXZ
unsigned long MBAnimSequence::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1002f270 ?Release@MBAnimSequence@@UAEKXZ
unsigned long MBAnimSequence::Release(void)
{
    unsigned long ref = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return ref;
}


// Matched by this code:
// 0x1002f180 ?_Destroy@?$vector@PAVMBAnimFrame@@V?$MBAllocator@PAVMBAnimFrame@@@@@std@@IAEXPAPAVMBAnimFrame@@0@Z
// 0x1002f190 ?_Ucopy@?$vector@PAVMBAnimFrame@@V?$MBAllocator@PAVMBAnimFrame@@@@@std@@IAEPAPAVMBAnimFrame@@PBQAV3@0PAPAV3@@Z
// 0x1002f1c0 ?_Ufill@?$vector@PAVMBAnimFrame@@V?$MBAllocator@PAVMBAnimFrame@@@@@std@@IAEXPAPAVMBAnimFrame@@IABQAV3@@Z
// 0x1002f1f0 ?_Construct@std@@YAXPAPAVMBAnimFrame@@ABQAV2@@Z
// 0x1002f210 _$E20
// 0x1002f230 _$E19
