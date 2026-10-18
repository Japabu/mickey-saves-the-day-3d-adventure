// MBEngine.dll unit CMBAnim (.\MBEngineRelease\CMBAnim.obj)
#include "CMBAnim.h"

// PDB names these without C++ decoration: C linkage, file scope
extern "C" {
static void SetBit(unsigned long &bitField, unsigned long bit, bool value);
static bool TestBit(unsigned long bitField, unsigned long bit);
}

// 0x1001ab20 ?MakeCMBAnim@@YAPAVIMBUnknown@@XZ
IMBUnknown *MakeCMBAnim(void)
{
    IMBUnknown *p = (IMBAnim *)new CMBAnim;
    if (p)
        p->AddRef();
    return p;
}

// 0x1001ab60 $E65 / 0x1001aba0 $E66 / 0x1001ab90 $E68 / 0x1001ab50 $E69
MBList<CMBAnim *> CMBAnim::sAnimList;

// 0x1001ac30 $E71 / 0x1001ac20 $E72
MBPixelFormat CMBAnim::sCachedScreenPixelFormat;

// TODO 0x1001acb0 ??0CMBAnim@@QAE@XZ: member initialisation scheduled differently (ours zeroes the members before the 100 defaults; ebp/edi swapped) (77% similar)
CMBAnim::CMBAnim(void)
    : mProperties(0), mExternalCanvas(0), mExternalSequence(0), mDelay(100), mZPlane(100),
      mQuietBackground(false), mAutoUnload(false), mDontAdjustZPlane(false),
      mScaleX(1.0f), mScaleY(1.0f), mScaleZ(1.0f)
{
    _mRefCnt = 0;
    MBComPtr<IMBPeriodicalManager> _periodicalManager(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->AddPeriodical(this);
    mPos.x = 0;
    mPos.y = 0;
    mID = 0;
    mParent = 0;
    mPos.z = 100;
    mHideCount = 0;
    mDirty = true;
    mNextFrameAt = 0;
    mCurrentSequenceIndex = -1;
    mCurrentFrameIndex = -1;
    mPlaying = mStalled = mRender = false;
    mPauseCount = 0;
    mClock.CreateObject(MBIID_IMBGameClock);
}

// 0x1001ae50 ?QueryInterface@CMBAnim@@UAEJABUMBGUID@@PAPAX@Z
long CMBAnim::QueryInterface(const MBGUID &iid, void **obj)
{
    if (!obj)
        return -2;
    *obj = 0;
    IMBUnknown *p = 0;
    if (iid == MBIID_IMBUnknown)
        p = (IMBAnim *)this;
    if (iid == MBIID_IMBResource)
        p = (IMBResource *)this;
    if (iid == MBIID_IMBObject)
        p = (IMBObject *)this;
    if (iid == MBIID_IMBAnim)
        p = (IMBAnim *)this;
    if (iid == MBIID_IMBPeriodical)
        p = (IMBPeriodical *)this;
    if (p) {
        p->AddRef();
        *obj = p;
        return 0;
    }
    return -3;
}

// 0x1001afe0 ?AddRef@CMBAnim@@UAEKXZ
unsigned long CMBAnim::AddRef(void)
{
    return ++_mRefCnt;
}

// 0x1001aff0 ?Release@CMBAnim@@UAEKXZ
unsigned long CMBAnim::Release(void)
{
    unsigned long r = --_mRefCnt;
    if (!_mRefCnt)
        delete this;
    return r;
}

// EQUIV 0x1001b020 ??1CMBAnim@@QAE@XZ: reviewed: the original calls the list<CMBAnim *> erase and iterator helpers out of line in the sAnimList removal loop, VC6 inlines them (frame 4 bytes smaller); calls and order agree. Was: the sAnimList removal loop: original calls a list<CMBAnim*> helper out of line and has a 4-byte larger frame; ours inlines erase and iterator++ (65% similar)
CMBAnim::~CMBAnim(void)
{
    for (MBList<CMBAnim *>::iterator it = sAnimList.begin(); it != sAnimList.end();) {
        if (*it == this)
            sAnimList.erase(it++);
        else
            ++it;
    }
    if (mID)
        SendMessage(1);
    MBComPtr<IMBPeriodicalManager> _periodicalManager;
    _periodicalManager.CreateObject(MBIID_IMBPeriodicalManager);
    if (_periodicalManager)
        _periodicalManager->RemovePeriodical(this);
    MBVector<MBAnimSequence *>::iterator seq;
    for (seq = mSequences.begin(); seq != mSequences.end(); ++seq)
        (*seq)->Release();
    MBVector<MBAnimCast *>::iterator cast;
    for (cast = mCasts.begin(); cast != mCasts.end(); ++cast)
        (*cast)->Release();
}

// EQUIV 0x1001b160 ?Load@CMBAnim@@UAEJKPAVIMBStream@@_N@Z: reviewed: stack slots only (the size locals sit in other frame slots)
long CMBAnim::Load(unsigned long id, IMBStream *in, bool unpackFrames)
{
    unsigned long animFlags;
    long version;
    char cookie[5];

    if (!in)
        return -11001;
    unsigned long size = 4;
    in->Read(cookie, size);
    cookie[4] = 0;
    if (MBstrcmp(cookie, "ANIM"))
        return -11002;
    {
        unsigned long size = 4;
        in->Read(&version, size);
        if (version < 3 || version > 5)
            return -11003;
        if (version < 5) {
            long bitmapOffset;
            size = 4;
            in->Read(&bitmapOffset, size);
        }
        size = 4;
        in->Read(&animFlags, size);
    }
    SetAnimationProperty(kAnimationAutoUnload, (bool)((animFlags >> 4) & 1));
    SetAnimationProperty(kAnimationQuietBackground, (bool)((animFlags >> 1) & 1));
    SetAnimationProperty(kAnimationDontAdjustZPlane, (bool)((animFlags >> 3) & 1));
    if (version >= 5) {
        short colorDepth;
        short numColors;
        unsigned long size = 2;
        in->Read(&colorDepth, size);
        size = 2;
        in->Read(&numColors, size);
    }
    if (!ReadAnimInfo(in, version))
        return -11004;
    if (!ReadCast(in, version))
        return -11004;
    unsigned long totalSize = 0;
    MBVector<MBAnimCast *>::iterator it;
    for (it = mCasts.begin(); it != mCasts.end(); ++it)
        totalSize += (*it)->GetApproxSize();
    if (totalSize < 0x10000) {
        for (it = mCasts.begin(); it != mCasts.end(); ++it)
            (*it)->Lock();
    }
    mID = id;
    SendMessage(0);
    sAnimList.push_back(this);
    return 0;
}

// TODO 0x1001b380 ?Clone@CMBAnim@@UAEXPAPAVIMBAnim@@@Z: register allocation: original keeps the new anim in ebp (one more frame slot), ours in a stack slot (83% similar)
void CMBAnim::Clone(IMBAnim **clone)
{
    *clone = 0;
    CMBAnim *anim = new CMBAnim;
    if (anim) {
        anim->mProperties = mProperties;
        anim->mExternalCanvas = mExternalCanvas;
        anim->mExternalSequence = mExternalSequence;
        anim->mDelay = mDelay;
        anim->mZPlane = mZPlane;
        anim->mQuietBackground = mQuietBackground;
        anim->mAutoUnload = mAutoUnload;
        anim->mDontAdjustZPlane = mDontAdjustZPlane;
        anim->mSequences.reserve(mSequences.size());
        long i;
        long numSequences = mSequences.size();
        for (i = 0; i < numSequences; i++) {
            MBAnimSequence *seq = mSequences[i];
            seq->AddRef();
            anim->mSequences.push_back(seq);
        }
        anim->mCasts.reserve(mCasts.size());
        long numCasts = mCasts.size();
        for (i = 0; i < numCasts; i++) {
            MBAnimCast *cast = mCasts[i];
            cast->AddRef();
            anim->mCasts.push_back(cast);
        }
        anim->mID = mID;
        anim->AddRef();
        *clone = anim;
    }
}

// 0x1001b5a0 ?ReadAnimInfo@CMBAnim@@IAE_NPAVIMBStream@@J@Z
bool CMBAnim::ReadAnimInfo(IMBStream *stream, long version)
{
    unsigned long size;
    unsigned short numSequences;
    unsigned short animDelay;
    unsigned short zplane;

    size = 2;
    stream->Read(&animDelay, size);
    SetAnimationProperty(kAnimationDelay, animDelay);
    size = 2;
    stream->Read(&zplane, size);
    SetAnimationProperty(kAnimationZPlane, zplane);
    size = 2;
    stream->Read(&numSequences, size);
    for (long i = 0; i < numSequences; i++) {
        AddSequence();
        if (!ReadSequenceInfo(stream, version, i))
            return false;
    }
    return true;
}

// EQUIV 0x1001b660 ?ReadSequenceInfo@CMBAnim@@IAE_NPAVIMBStream@@JJ@Z: reviewed: stack slots only: each read's value and size locals live in other frame slots
bool CMBAnim::ReadSequenceInfo(IMBStream *stream, long version, long sequenceIndex)
{
    unsigned long size;
    unsigned short numFrames;
    unsigned long sequenceFlags;

    size = 2;
    stream->Read(&numFrames, size);
    size = 4;
    stream->Read(&sequenceFlags, size);
    SetSequenceProperty(sequenceIndex, kSequenceInSync, (bool)(sequenceFlags & 1));
    UseSequenceProperty(sequenceIndex, kSequenceZPlane, (bool)((sequenceFlags >> 3) & 1));
    UseSequenceProperty(sequenceIndex, kSequenceDelay, (bool)((sequenceFlags >> 1) & 1));
    UseSequenceProperty(sequenceIndex, kSequenceTileWidth, (bool)((sequenceFlags >> 4) & 1));
    UseSequenceProperty(sequenceIndex, kSequenceTileHeight, (bool)((sequenceFlags >> 5) & 1));
    if (version < 5) {
        SetSequenceProperty(sequenceIndex, kSequenceEndAction, (unsigned short)0);
        if (sequenceFlags & 4)
            SetSequenceProperty(sequenceIndex, kSequenceEndAction, (unsigned short)2);
    }
    if (sequenceFlags & 8) {
        unsigned short sequenceZPlane;
        size = 2;
        stream->Read(&sequenceZPlane, size);
        SetSequenceProperty(sequenceIndex, kSequenceZPlane, sequenceZPlane);
    }
    if (sequenceFlags & 2) {
        unsigned short sequenceDelay;
        size = 2;
        stream->Read(&sequenceDelay, size);
        SetSequenceProperty(sequenceIndex, kSequenceDelay, sequenceDelay);
    }
    if (version >= 5) {
        if (sequenceFlags & 0x10) {
            unsigned short tileWidth;
            size = 2;
            stream->Read(&tileWidth, size);
            SetSequenceProperty(sequenceIndex, kSequenceTileWidth, tileWidth);
        }
        if (sequenceFlags & 0x20) {
            unsigned short tileHeight;
            size = 2;
            stream->Read(&tileHeight, size);
            SetSequenceProperty(sequenceIndex, kSequenceTileHeight, tileHeight);
        }
        unsigned short sequenceEndAction;
        size = 2;
        stream->Read(&sequenceEndAction, size);
        SetSequenceProperty(sequenceIndex, kSequenceEndAction, sequenceEndAction);
        if (sequenceEndAction == 3) {
            unsigned short jumpSequence;
            size = 2;
            stream->Read(&jumpSequence, size);
            SetSequenceProperty(sequenceIndex, kSequenceJumpSequence, jumpSequence);
        } else if (sequenceEndAction == 4) {
            unsigned short jumpRandomSequenceMin;
            unsigned short jumpRandomSequenceMax;
            size = 2;
            stream->Read(&jumpRandomSequenceMin, size);
            size = 2;
            stream->Read(&jumpRandomSequenceMax, size);
            SetSequenceProperty(sequenceIndex, kSequenceJumpRandomSequenceMin, jumpRandomSequenceMin);
            SetSequenceProperty(sequenceIndex, kSequenceJumpRandomSequenceMax, jumpRandomSequenceMax);
        }
    }
    MBString str;
    if (!ReadString(stream, str))
        return false;
    for (long i = 0; i < numFrames; i++) {
        AddFrame(sequenceIndex);
        if (!ReadFrameInfo(stream, version, sequenceIndex, i))
            return false;
    }
    return true;
}

// EQUIV 0x1001b9a0 ?ReadFrameInfo@CMBAnim@@IAE_NPAVIMBStream@@JJJ@Z: reviewed: inline choices of the same helpers: the original constructs the string local out of line and calls the string _Tidy, VC6 inlines both and lays out the shared failure tails differently; calls, conditions and order agree. Was: original constructs a local through an out-of-line call at the top; ours inlines it and schedules the zeroing first (74% similar)
bool CMBAnim::ReadFrameInfo(IMBStream *stream, long version, long sequenceIndex, long frameIndex)
{
    unsigned long size;
    unsigned short castID;
    unsigned long frameFlags;
    MBString str;

    size = 2;
    stream->Read(&castID, size);
    SetCastID(sequenceIndex, frameIndex, castID);
    size = 4;
    stream->Read(&frameFlags, size);
    SetFrameProperty(sequenceIndex, frameIndex, kFrameNotify, (bool)(frameFlags & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameDelay, (bool)((frameFlags >> 2) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameStartSound, (bool)((frameFlags >> 4) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameStartAnim, (bool)((frameFlags >> 5) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameStopAnim, (bool)((frameFlags >> 6) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameStartScript, (bool)((frameFlags >> 7) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameStopScript, (bool)((frameFlags >> 8) & 1));
    UseFrameProperty(sequenceIndex, frameIndex, kFrameCastMemberOffset, (bool)((frameFlags >> 10) & 1));
    if (version < 5) {
        if (frameFlags & 2)
            SetSequenceProperty(sequenceIndex, kSequenceEndAction, (unsigned short)1);
        if (frameFlags & 8) {
            unsigned short jumpSequence;
            size = 2;
            stream->Read(&jumpSequence, size);
            SetSequenceProperty(sequenceIndex, kSequenceEndAction, (unsigned short)3);
            SetSequenceProperty(sequenceIndex, kSequenceJumpSequence, jumpSequence);
        }
    }
    if (frameFlags & 4) {
        unsigned short frameDelay;
        size = 2;
        stream->Read(&frameDelay, size);
        SetFrameProperty(sequenceIndex, frameIndex, kFrameDelay, frameDelay);
    }
    if (frameFlags & 0x10) {
        if (!ReadString(stream, str))
            return false;
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStartSound, str);
    }
    if (frameFlags & 0x20) {
        if (!ReadString(stream, str))
            return false;
        unsigned short startAnimSequence;
        size = 2;
        stream->Read(&startAnimSequence, size);
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStartAnim, str);
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStartAnimSequence, startAnimSequence);
    }
    if (frameFlags & 0x40) {
        if (!ReadString(stream, str))
            return false;
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStopAnim, str);
    }
    if (frameFlags & 0x80) {
        if (!ReadString(stream, str))
            return false;
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStartScript, str);
    }
    if (frameFlags & 0x100) {
        if (!ReadString(stream, str))
            return false;
        SetFrameProperty(sequenceIndex, frameIndex, kFrameStopScript, str);
    }
    if (frameFlags & 0x400) {
        unsigned short offsetX;
        unsigned short offsetY;
        size = 2;
        stream->Read(&offsetX, size);
        size = 2;
        stream->Read(&offsetY, size);
        SetFrameProperty(sequenceIndex, frameIndex, kFrameCastMemberOffset, MBPoint(offsetX, offsetY));
    }
    return true;
}

// TODO 0x1001bd90 ?ReadCast@CMBAnim@@IAE_NPAVIMBStream@@J@Z: ours is 334 bytes larger with a 0x14 larger frame: different inlining of the cast-reading helpers (73% similar)
bool CMBAnim::ReadCast(IMBStream *stream, long version)
{
    unsigned long size;
    unsigned short num_casts;
    MBPixelFormat screenPixelFormat = sCachedScreenPixelFormat;
    IMBScreenManager *screenManager;

    MBGetScreenManager(&screenManager);
    if (screenManager) {
        IMBScreen *screen;
        screenManager->GetScreenByIndex(0, &screen);
        if (screen) {
            IMBSurface *surface;
            screen->GetBackBuffer(&surface);
            if (surface) {
                screenPixelFormat = surface->GetFormat();
                sCachedScreenPixelFormat = screenPixelFormat;
                surface->Release();
            }
            screen->Release();
        }
        screenManager->Release();
    }
    size = 2;
    stream->Read(&num_casts, size);
    for (long i = 0; i < num_casts; i++) {
        MBAnimCast *cast = new MBAnimCast;
        cast->AddRef();
        MBPoint registrationPoint;
        if (version >= 5) {
            long x;
            long y;
            size = 4;
            stream->Read(&x, size);
            size = 4;
            stream->Read(&y, size);
            registrationPoint.x = x;
            registrationPoint.y = y;
        } else {
            unsigned short x;
            unsigned short y;
            size = 2;
            stream->Read(&x, size);
            size = 2;
            stream->Read(&y, size);
            registrationPoint.x = x;
            registrationPoint.y = y;
        }
        cast->SetRegistrationPoint(registrationPoint);
        unsigned long castMemberFlags = 0;
        if (version >= 5) {
            size = 4;
            stream->Read(&castMemberFlags, size);
        }
        if (castMemberFlags & 1) {
            long x;
            long y;
            size = 4;
            stream->Read(&x, size);
            size = 4;
            stream->Read(&y, size);
            cast->SetAttachmentPoint(MBPoint(x, y));
        }
        mCasts.push_back(cast);
    }
    unsigned long movieVersion;
    unsigned short movieWidth;
    unsigned short movieHeight;
    unsigned short movieDepth;
    unsigned short movieFrames;
    size = 4;
    stream->Read(&movieVersion, size);
    size = 2;
    stream->Read(&movieWidth, size);
    size = 2;
    stream->Read(&movieHeight, size);
    size = 2;
    stream->Read(&movieDepth, size);
    size = 2;
    stream->Read(&movieFrames, size);
    for (unsigned short frame = 0; frame < movieFrames; frame++) {
        if (!mCasts[frame]->Init(stream, screenPixelFormat))
            return false;
    }
    return true;
}

// 0x1001c0a0 ?ReadString@CMBAnim@@IAE_NPAVIMBStream@@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
bool CMBAnim::ReadString(IMBStream *stream, MBString &string)
{
    unsigned char strlength;
    unsigned long size;
    char buffer[256];

    size = 1;
    stream->Read(&strlength, size);
    size = strlength;
    if (size > 0) {
        if (!stream->Read(buffer, size))
            return false;
        buffer[strlength] = 0;
        string = buffer;
    } else
        string = "";
    return true;
}

// 0x1001c290 ?Save@CMBAnim@@UAEJPAVIMBStream@@@Z
long CMBAnim::Save(IMBStream *out)
{
    return 0;
}

// 0x1001c2a0 ?WriteAnimInfo@CMBAnim@@IAE_NPAVIMBStream@@@Z
bool CMBAnim::WriteAnimInfo(IMBStream *stream)
{
    return false;
}

// 0x1001c2b0 ?WriteSequenceInfo@CMBAnim@@IAE_NPAVIMBStream@@PAVMBAnimSequence@@@Z
bool CMBAnim::WriteSequenceInfo(IMBStream *stream, MBAnimSequence *sequence)
{
    return false;
}

// 0x1001c2c0 ?WriteFrameInfo@CMBAnim@@IAE_NPAVIMBStream@@PAVMBAnimSequence@@PAVMBAnimFrame@@@Z
bool CMBAnim::WriteFrameInfo(IMBStream *stream, MBAnimSequence *sequence, MBAnimFrame *frame)
{
    return false;
}

// 0x1001c2d0 ?WriteCast@CMBAnim@@IAE_NPAVIMBStream@@@Z
bool CMBAnim::WriteCast(IMBStream *stream)
{
    return false;
}

// 0x1001c2e0 ?WriteString@CMBAnim@@IAE_NPAVIMBStream@@AAV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
bool CMBAnim::WriteString(IMBStream *stream, MBString &string)
{
    return false;
}

// 0x1001c2f0 ?SendMessage@CMBAnim@@IAEXJ@Z
void CMBAnim::SendMessage(long msg)
{
    MBComPtr<IMBEventManager> eventManager(MBIID_IMBEventManager);
    if (eventManager) {
        MBAnimEventStruct data;
        data.anim = this;
        data.id = GetID();
        data.sequence = GetCurrentSequenceIndex();
        data.frame = GetCurrentFrameIndex();
        eventManager->BroadcastEvent(-2, msg, &data);
    }
}

// 0x1001c390 ?SetAnimationProperty@CMBAnim@@UAEJW4MBAnimationProperty@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
long CMBAnim::SetAnimationProperty(MBAnimationProperty property, const MBString &value)
{
    long result = 0;
    switch (property) {
    case kAnimationExternalCanvas:
        mExternalCanvas = MBHashString(value.c_str());
        break;
    case kAnimationExternalSequence:
        mExternalSequence = MBHashString(value.c_str());
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c400 ?SetAnimationProperty@CMBAnim@@UAEJW4MBAnimationProperty@@AAK@Z
long CMBAnim::SetAnimationProperty(MBAnimationProperty property, unsigned long &value)
{
    long result = 0;
    switch (property) {
    case kAnimationExternalCanvas:
        mExternalCanvas = value;
        break;
    case kAnimationExternalSequence:
        mExternalSequence = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c430 ?SetAnimationProperty@CMBAnim@@UAEJW4MBAnimationProperty@@G@Z
long CMBAnim::SetAnimationProperty(MBAnimationProperty property, unsigned short value)
{
    long result = 0;
    switch (property) {
    case kAnimationDelay:
        mDelay = value;
        break;
    case kAnimationZPlane: {
        mZPlane = value;
        bool dontAdjustZPlane;
        GetAnimationProperty(kAnimationDontAdjustZPlane, dontAdjustZPlane);
        if (!dontAdjustZPlane)
            mPos.z = mZPlane;
        break;
    }
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c490 ?SetAnimationProperty@CMBAnim@@UAEJW4MBAnimationProperty@@_N@Z
long CMBAnim::SetAnimationProperty(MBAnimationProperty property, bool value)
{
    long result = 0;
    switch (property) {
    case kAnimationQuietBackground:
        mQuietBackground = value;
        break;
    case kAnimationAutoUnload:
        mAutoUnload = value;
        break;
    case kAnimationDontAdjustZPlane:
        mDontAdjustZPlane = value;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c4d0 ?SetSequenceProperty@CMBAnim@@UAEJJW4MBSequenceProperty@@G@Z
long CMBAnim::SetSequenceProperty(long sequenceIndex, MBSequenceProperty property, unsigned short value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetSequenceProperty(property, value);
    return -11010;
}

// 0x1001c500 ?SetSequenceProperty@CMBAnim@@UAEJJW4MBSequenceProperty@@_N@Z
long CMBAnim::SetSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetSequenceProperty(property, value);
    return -11010;
}

// 0x1001c530 ?SetFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@ABVMBPoint@@@Z
long CMBAnim::SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, const MBPoint &value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c570 ?SetFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@ABV?$basic_string@DU?$char_traits@D@std@@V?$MBAllocator@D@@@std@@@Z
long CMBAnim::SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, const MBString &value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c5b0 ?SetFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@K@Z
long CMBAnim::SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned long value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c5f0 ?SetFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@G@Z
long CMBAnim::SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned short value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c630 ?SetFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@_N@Z
long CMBAnim::SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool value)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->SetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c670 ?GetAnimationProperty@CMBAnim@@UBEJW4MBAnimationProperty@@AAK@Z
long CMBAnim::GetAnimationProperty(MBAnimationProperty property, unsigned long &value) const
{
    long result = 0;
    switch (property) {
    case kAnimationExternalCanvas:
        value = mExternalCanvas;
        break;
    case kAnimationExternalSequence:
        value = mExternalSequence;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c6a0 ?GetAnimationProperty@CMBAnim@@UBEJW4MBAnimationProperty@@AAG@Z
long CMBAnim::GetAnimationProperty(MBAnimationProperty property, unsigned short &value) const
{
    long result = 0;
    switch (property) {
    case kAnimationDelay:
        value = mDelay;
        break;
    case kAnimationZPlane:
        value = mZPlane;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c6e0 ?GetAnimationProperty@CMBAnim@@UBEJW4MBAnimationProperty@@AA_N@Z
long CMBAnim::GetAnimationProperty(MBAnimationProperty property, bool &value) const
{
    long result = 0;
    switch (property) {
    case kAnimationQuietBackground:
        value = mQuietBackground;
        break;
    case kAnimationAutoUnload:
        value = mAutoUnload;
        break;
    case kAnimationDontAdjustZPlane:
        value = mDontAdjustZPlane;
        break;
    default:
        result = -11008;
        break;
    }
    return result;
}

// 0x1001c720 ?GetSequenceProperty@CMBAnim@@UBEJJW4MBSequenceProperty@@AAG@Z
long CMBAnim::GetSequenceProperty(long sequenceIndex, MBSequenceProperty property, unsigned short &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetSequenceProperty(property, value);
    return -11010;
}

// 0x1001c750 ?GetSequenceProperty@CMBAnim@@UBEJJW4MBSequenceProperty@@AA_N@Z
long CMBAnim::GetSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetSequenceProperty(property, value);
    return -11010;
}

// 0x1001c780 ?GetFrameProperty@CMBAnim@@UBEJJJW4MBFrameProperty@@AAVMBPoint@@@Z
long CMBAnim::GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, MBPoint &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c7c0 ?GetFrameProperty@CMBAnim@@UBEJJJW4MBFrameProperty@@AAK@Z
long CMBAnim::GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned long &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c800 ?GetFrameProperty@CMBAnim@@UBEJJJW4MBFrameProperty@@AAG@Z
long CMBAnim::GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned short &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c840 ?GetFrameProperty@CMBAnim@@UBEJJJW4MBFrameProperty@@AA_N@Z
long CMBAnim::GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool &value) const
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->GetFrameProperty(frameIndex, property, value);
    return -11010;
}

// 0x1001c880 ?UseAnimationProperty@CMBAnim@@UAEJW4MBAnimationProperty@@_N@Z
long CMBAnim::UseAnimationProperty(MBAnimationProperty property, bool use)
{
    long result = 0;
    switch (property) {
    case kAnimationExternalCanvas:
        SetBit(mProperties, 0x04, use);
        break;
    case kAnimationExternalSequence:
        SetBit(mProperties, 0x20, use);
        break;
    default:
        result = -11009;
        break;
    }
    return result;
}

// 0x1001c8d0 ?HasAnimationProperty@CMBAnim@@UBE_NW4MBAnimationProperty@@@Z
bool CMBAnim::HasAnimationProperty(MBAnimationProperty property) const
{
    bool result = false;
    switch (property) {
    case kAnimationExternalCanvas:
        result = TestBit(mProperties, 0x04);
        break;
    case kAnimationExternalSequence:
        result = TestBit(mProperties, 0x20);
        break;
    }
    return result;
}

// 0x1001c900 ?UseSequenceProperty@CMBAnim@@UAEJJW4MBSequenceProperty@@_N@Z
long CMBAnim::UseSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool use)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->UseProperty(property, use);
    return -11010;
}

// 0x1001c930 ?HasSequenceProperty@CMBAnim@@UBE_NJW4MBSequenceProperty@@@Z
bool CMBAnim::HasSequenceProperty(long sequenceIndex, MBSequenceProperty property) const
{
    bool result = false;
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        result = seq->HasProperty(property);
    return result;
}

// 0x1001c960 ?UseFrameProperty@CMBAnim@@UAEJJJW4MBFrameProperty@@_N@Z
long CMBAnim::UseFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool use)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        return seq->UseFrameProperty(frameIndex, property, use);
    return -11010;
}

// 0x1001c9a0 ?HasFrameProperty@CMBAnim@@UBE_NJJW4MBFrameProperty@@@Z
bool CMBAnim::HasFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property) const
{
    bool result = false;
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        result = seq->HasFrameProperty(frameIndex, property);
    return result;
}

// 0x1001c9d0 ?SetCastID@CMBAnim@@UAEXJJJ@Z
void CMBAnim::SetCastID(long sequenceIndex, long frameIndex, long castID)
{
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        seq->SetCastID(frameIndex, castID);
}

// 0x1001ca00 ?GetCastID@CMBAnim@@UBEJJJ@Z
long CMBAnim::GetCastID(long sequenceIndex, long frameIndex) const
{
    long castID = 0;
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        castID = seq->GetCastID(frameIndex);
    return castID;
}

// 0x1001ca30 ?Play@CMBAnim@@UAEXXZ
void CMBAnim::Play(void)
{
    mHideCount = 0;
    mRender = true;
    mPauseCount = 0;
    mStalled = false;
    if (!mPlaying) {
        SendMessage(3);
        mPlaying = true;
        if (GetCurrentSequenceIndex() == -1)
            SetCurrentSequence(0);
    }
}

// 0x1001ca80 ?Stop@CMBAnim@@UAEXXZ
void CMBAnim::Stop(void)
{
    mPlaying = false;
    mRender = false;
    mStalled = false;
    SendMessage(4);
}

// 0x1001caa0 ?Pause@CMBAnim@@UAEJXZ
long CMBAnim::Pause(void)
{
    return ++mPauseCount;
}

// 0x1001cab0 ?Resume@CMBAnim@@UAEJXZ
long CMBAnim::Resume(void)
{
    if (mPauseCount > 0)
        mPauseCount--;
    return mPauseCount;
}

// 0x1001cac0 ?IsPaused@CMBAnim@@UBE_NXZ
bool CMBAnim::IsPaused(void) const
{
    return mPauseCount > 0;
}

// 0x1001cad0 ?IsStalled@CMBAnim@@UBE_NXZ
bool CMBAnim::IsStalled(void) const
{
    return mStalled;
}

// 0x1001cae0 ?IsRendering@CMBAnim@@UBE_NXZ
bool CMBAnim::IsRendering(void) const
{
    return mRender;
}

// 0x1001caf0 ?IsPlaying@CMBAnim@@UBE_NXZ
bool CMBAnim::IsPlaying(void) const
{
    return mPlaying;
}

// 0x1001cb00 ?GetNumFrames@CMBAnim@@UBEKJ@Z
unsigned long CMBAnim::GetNumFrames(long sequenceIndex) const
{
    unsigned long numFrames = 0;
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        numFrames = seq->GetNumFrames();
    return numFrames;
}

// 0x1001cb30 ?GetNumSequences@CMBAnim@@UBEKXZ
unsigned long CMBAnim::GetNumSequences(void) const
{
    return mSequences.size();
}

// 0x1001cb50 ?AddSequence@CMBAnim@@UAEJXZ
long CMBAnim::AddSequence(void)
{
    long result = -1;
    MBAnimSequence *seq = new MBAnimSequence;
    if (seq) {
        seq->AddRef();
        mSequences.push_back(seq);
        result = GetNumSequences() - 1;
    }
    return result;
}

// 0x1001cd50 ?GetSequence@CMBAnim@@UBEPAVMBAnimSequence@@J@Z
MBAnimSequence *CMBAnim::GetSequence(long sequenceIndex) const
{
    if (sequenceIndex >= 0 && sequenceIndex < GetNumSequences())
        return mSequences[sequenceIndex];
    return 0;
}

// 0x1001cd80 ?SetCurrentSequence@CMBAnim@@UAE_NJ@Z
bool CMBAnim::SetCurrentSequence(long sequenceIndex)
{
    if (sequenceIndex >= 0 && sequenceIndex < GetNumSequences()) {
        mCurrentSequenceIndex = sequenceIndex;
        bool dontAdjustZPlane;
        GetAnimationProperty(kAnimationDontAdjustZPlane, dontAdjustZPlane);
        if (!dontAdjustZPlane && HasSequenceProperty(mCurrentSequenceIndex, kSequenceZPlane)) {
            unsigned short zplane;
            GetSequenceProperty(mCurrentSequenceIndex, kSequenceZPlane, zplane);
            Place(MBPoint3(mPos.x, mPos.y, zplane));
        }
        SetDirty();
        if (IsPlaying())
            SendMessage(5);
        return SetCurrentFrame(0);
    }
    return false;
}

// 0x1001ce50 ?GetCurrentSequenceIndex@CMBAnim@@UBEJXZ
long CMBAnim::GetCurrentSequenceIndex(void) const
{
    return mCurrentSequenceIndex;
}

// EQUIV 0x1001ce60 ?SetCurrentFrame@CMBAnim@@UAE_NJ@Z: reviewed: stack slots and tail merging only (the delay-property address is pushed once before the sequence/animation branch); same property lookups
bool CMBAnim::SetCurrentFrame(long frameIndex)
{
    mStalled = false;
    if (frameIndex >= 0 && frameIndex < GetNumFrames(mCurrentSequenceIndex)) {
        mCurrentFrameIndex = frameIndex;
        bool notify;
        GetFrameProperty(mCurrentSequenceIndex, frameIndex, kFrameNotify, notify);
        if (notify)
            SendMessage(2);
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStartSound)) {
            unsigned long sound;
            GetFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStartSound, sound);
            MBPlaySoundID(sound, 1);
        }
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStartAnim)) {
        }
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStopAnim)) {
        }
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStartScript)) {
        }
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameStopScript)) {
        }
        unsigned long now = mClock->GetGameTime();
        unsigned short delay;
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameDelay))
            GetFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameDelay, delay);
        else if (HasSequenceProperty(mCurrentSequenceIndex, kSequenceDelay))
            GetSequenceProperty(mCurrentSequenceIndex, kSequenceDelay, delay);
        else
            GetAnimationProperty(kAnimationDelay, delay);
        mNextFrameAt = delay + now;
        SetDirty();
        return true;
    }
    return false;
}

// 0x1001cff0 ?GetCurrentFrameIndex@CMBAnim@@UBEJXZ
long CMBAnim::GetCurrentFrameIndex(void) const
{
    return mCurrentFrameIndex;
}

// 0x1001d000 ?AddFrame@CMBAnim@@UAEJJ@Z
long CMBAnim::AddFrame(long sequenceIndex)
{
    long result = -1;
    MBAnimSequence *seq = GetSequence(sequenceIndex);
    if (seq)
        result = seq->AddFrame();
    return result;
}

// 0x1001d030 ?GetNumCasts@CMBAnim@@UBEKXZ
unsigned long CMBAnim::GetNumCasts(void) const
{
    return mCasts.size();
}

// 0x1001d050 ?AddCastMember@CMBAnim@@UAEJPAVIMBSurface@@ABVMBPoint@@@Z
long CMBAnim::AddCastMember(IMBSurface *image, const MBPoint &registrationPoint)
{
    long result = 0;
    MBAnimCast *cast = new MBAnimCast;
    if (cast) {
        cast->AddRef();
        cast->SetRegistrationPoint(registrationPoint);
        cast->SetSurface(image);
        mCasts.push_back(cast);
        result = GetNumCasts();
    }
    return result;
}

// 0x1001d270 ?GetCast@CMBAnim@@MBEPAVMBAnimCast@@J@Z
MBAnimCast *CMBAnim::GetCast(long castID) const
{
    MBAnimCast *cast = 0;
    if (castID > 0 && castID <= GetNumCasts())
        cast = mCasts[castID - 1];
    return cast;
}

// 0x1001d2a0 ?GetCastMemberSurface@CMBAnim@@UAEXJPAPAVIMBSurface@@@Z
void CMBAnim::GetCastMemberSurface(long castID, IMBSurface **surface)
{
    *surface = 0;
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        cast->GetSurface(surface);
}

// 0x1001d2d0 ?SetCastMemberSurface@CMBAnim@@UAEXJPAVIMBSurface@@@Z
void CMBAnim::SetCastMemberSurface(long castID, IMBSurface *surface)
{
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        cast->SetSurface(surface);
}

// 0x1001d2f0 ?GetCastMemberRegistrationPoint@CMBAnim@@UBE?AVMBPoint@@J@Z
MBPoint CMBAnim::GetCastMemberRegistrationPoint(long castID) const
{
    MBPoint point;
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        point = cast->GetRegistrationPoint();
    return point;
}

// 0x1001d330 ?SetCastMemberRegistrationPoint@CMBAnim@@UAEXJABVMBPoint@@@Z
void CMBAnim::SetCastMemberRegistrationPoint(long castID, const MBPoint &registrationPoint)
{
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        cast->SetRegistrationPoint(registrationPoint);
}

// 0x1001d350 ?GetCastMemberAttachmentPoint@CMBAnim@@UBE?AVMBPoint@@J@Z
MBPoint CMBAnim::GetCastMemberAttachmentPoint(long castID) const
{
    MBPoint point;
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        point = cast->GetAttachmentPoint();
    return point;
}

// 0x1001d390 ?SetCastMemberAttachmentPoint@CMBAnim@@UAEXJABVMBPoint@@@Z
void CMBAnim::SetCastMemberAttachmentPoint(long castID, const MBPoint &attachmentPoint)
{
    MBAnimCast *cast = GetCast(castID);
    if (cast)
        cast->SetAttachmentPoint(attachmentPoint);
}

// 0x1001d3b0 ?AdvanceFrame@CMBAnim@@MAEXXZ
void CMBAnim::AdvanceFrame(void)
{
    mStalled = false;
    long nextFrame = mCurrentFrameIndex + 1;
    if (nextFrame >= GetNumFrames(mCurrentSequenceIndex)) {
        unsigned short endAction;
        GetSequenceProperty(mCurrentSequenceIndex, kSequenceEndAction, endAction);
        if (endAction == 1) {
            mStalled = true;
            SendMessage(6);
            return;
        } else if (endAction == 2) {
            SendMessage(7);
            nextFrame = 0;
        } else if (endAction == 3) {
            SendMessage(6);
            unsigned short jumpSequence;
            GetSequenceProperty(mCurrentSequenceIndex, kSequenceJumpSequence, jumpSequence);
            SetCurrentSequence(jumpSequence);
            return;
        } else {
            Stop();
            SendMessage(6);
            return;
        }
    }
    SetCurrentFrame(nextFrame);
}

// 0x1001d480 ?Idle@CMBAnim@@UAEXXZ
void CMBAnim::Idle(void)
{
    unsigned long now = mClock->GetGameTime();
    if (mPlaying && !mStalled && !mPauseCount) {
        while (now > mNextFrameAt && !mStalled && mPlaying && mCurrentFrameIndex != -1 &&
               mCurrentSequenceIndex != -1)
            AdvanceFrame();
    }
}

// EQUIV 0x1001d4e0 ?GetBounds@CMBAnim@@UBE?AVMBRect@@XZ: reviewed: stack slots only, and the cast index is scaled with shl plus a -4 displacement instead of one lea (same address)
MBRect CMBAnim::GetBounds(void) const
{
    MBSize size;
    MBPoint3 pos = GetPosition();
    long castID = GetCastID(mCurrentSequenceIndex, mCurrentFrameIndex);
    if (castID) {
        if (HasFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameCastMemberOffset)) {
            MBPoint offset;
            GetFrameProperty(mCurrentSequenceIndex, mCurrentFrameIndex, kFrameCastMemberOffset, offset);
            MBPoint reg = mCasts[castID - 1]->GetRegistrationPoint();
            pos.x = (long)((reg.x + offset.x) * mScaleX + pos.x);
            pos.y = (long)((reg.y + offset.y) * mScaleY + pos.y);
        } else {
            MBPoint reg = mCasts[castID - 1]->GetRegistrationPoint();
            pos.x = (long)(reg.x * mScaleX + pos.x);
            pos.y = (long)(reg.y * mScaleY + pos.y);
        }
        MBSize castSize = mCasts[castID - 1]->GetSize();
        size.cx = (long)(castSize.cx * mScaleX);
        size.cy = (long)(castSize.cy * mScaleY);
    }
    return MBRect(pos.x, pos.y, pos.x + size.cx, pos.y + size.cy);
}

// 0x1001d660 ?GetOffset@CMBAnim@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBAnim::GetOffset(void) const
{
    return mPos;
}

// 0x1001d680 ?GetPosition@CMBAnim@@UBE?AVMBPoint3@@XZ
MBPoint3 CMBAnim::GetPosition(void) const
{
    if (mParent)
        return mPos + mParent->GetPosition();
    return mPos;
}

// 0x1001d6f0 ?Place@CMBAnim@@UAEXABVMBPoint3@@@Z
void CMBAnim::Place(const MBPoint3 &point)
{
    mPos = point;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x1001d730 ?Move@CMBAnim@@UAEXABVMBPoint3@@@Z
void CMBAnim::Move(const MBPoint3 &point)
{
    mPos.x += point.x;
    mPos.y += point.y;
    mPos.z += point.z;
    if (mParent)
        mParent->ObjectMoved(this);
    SetDirty();
}

// 0x1001d770 ?IsVisible@CMBAnim@@UBE_NXZ
bool CMBAnim::IsVisible(void) const
{
    return IsRendering() && mHideCount == 0;
}

// 0x1001d7a0 ?Hide@CMBAnim@@UAEJXZ
long CMBAnim::Hide(void)
{
    if (mHideCount == 0)
        SetDirty();
    return ++mHideCount;
}

// 0x1001d7c0 ?Show@CMBAnim@@UAEJXZ
long CMBAnim::Show(void)
{
    if (mHideCount > 0)
        mHideCount--;
    if (mHideCount == 0)
        SetDirty();
    return mHideCount;
}

// 0x1001d7f0 ?IsDirty@CMBAnim@@UBE_NXZ
bool CMBAnim::IsDirty(void) const
{
    return mDirty;
}

// 0x1001d800 ?SetDirty@CMBAnim@@UAEXXZ
void CMBAnim::SetDirty(void)
{
    mDirty = true;
}

// 0x1001d810 ?ClearDirty@CMBAnim@@UAEXXZ
void CMBAnim::ClearDirty(void)
{
    mDirty = false;
}

// 0x1001d820 ?SetParent@CMBAnim@@MAEPAVIMBObjectGroup@@PAV2@@Z
IMBObjectGroup *CMBAnim::SetParent(IMBObjectGroup *newParent)
{
    IMBObjectGroup *oldParent = mParent;
    if (oldParent) {
        mParent = 0;
        oldParent->RemoveObject(this);
    }
    mParent = newParent;
    return oldParent;
}

// 0x1001d860 ?GetID@CMBAnim@@UAEKXZ
unsigned long CMBAnim::GetID(void)
{
    return mID;
}

// 0x1001d870 ?GetType@CMBAnim@@UAEKXZ
unsigned long CMBAnim::GetType(void)
{
    return 'anm ';
}

// TODO 0x1001d880 ?Draw@CMBAnim@@UBEJPAVIMBSurface@@AAVMBRect@@1@Z: original keeps a rect value in ebp (+4 frame); register allocation (81% similar)
long CMBAnim::Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const
{
    MBRect srcRect(worldRect);
    if (mPlaying && mRender && mHideCount <= 0) {
        if (!dest)
            return -11001;
        long castID = GetCastID(mCurrentSequenceIndex, mCurrentFrameIndex);
        if (!castID)
            return -11007;
        MBAnimCast *cast = mCasts[castID - 1];
        if (!cast)
            return -11007;
        IMBSurface *surface;
        cast->GetSurface(mClock->GetGameTime(), &surface);
        if (!surface)
            return -11007;
        unsigned long flags = 1;
        if (mScaleX != 1.0 || mScaleY != 1.0) {
            srcRect.right = (long)(1.0f / mScaleX * srcRect.right);
            srcRect.bottom = (long)(1.0f / mScaleY * srcRect.bottom);
            flags = 3;
        }
        long result = surface->Blit(dest, &srcRect, &destRect, flags);
        if (result) {
            cast->Purge();
            IMBSurface *s;
            cast->GetSurface(mClock->GetGameTime(), &s);
            if (s)
                s->Release();
            result = surface->Blit(dest, &worldRect, &destRect, 1);
        }
        surface->Release();
        return result;
    }
    return 0;
}

// 0x1001da10 ?GetParent@CMBAnim@@UBEXPAPAVIMBObjectGroup@@@Z
void CMBAnim::GetParent(IMBObjectGroup **parent) const
{
    IMBObjectGroup *p = mParent;
    *parent = p;
    if (p)
        p->AddRef();
}

// 0x1001da30 ?Purge@CMBAnim@@QAEKK@Z
unsigned long CMBAnim::Purge(unsigned long agressionLevel)
{
    unsigned long amountPurged = 0;
    long numCasts;
    long i;
    switch (agressionLevel) {
    case 0:
        numCasts = mCasts.size();
        for (i = 0; i < numCasts; i++) {
            MBAnimCast *cast = mCasts[i];
            if (cast->GetLastAccess() == 0)
                amountPurged += cast->Purge();
        }
        break;
    case 1: {
        unsigned long oneMinuteAgo = mClock->GetGameTime() - 60000;
        numCasts = mCasts.size();
        for (i = 0; i < numCasts; i++) {
            MBAnimCast *cast = mCasts[i];
            if (cast->GetLastAccess() < oneMinuteAgo)
                amountPurged += cast->Purge();
        }
        break;
    }
    case 2: {
        unsigned long tenSecondsAgo = mClock->GetGameTime() - 10000;
        numCasts = mCasts.size();
        for (i = 0; i < numCasts; i++) {
            MBAnimCast *cast = mCasts[i];
            if (cast->GetLastAccess() < tenSecondsAgo)
                amountPurged += cast->Purge();
        }
        break;
    }
    case 3:
        numCasts = mCasts.size();
        for (i = 0; i < numCasts; i++) {
            MBAnimCast *cast = mCasts[i];
            amountPurged += cast->Purge();
        }
        break;
    }
    return amountPurged;
}

// 0x1001dba0 ?LowMemHandler@CMBAnim@@SA_NKK@Z
bool CMBAnim::LowMemHandler(unsigned long sizeRequest, unsigned long agressionLevel)
{
    unsigned long amountPurged = 0;
    for (MBList<CMBAnim *>::iterator it = sAnimList.begin(); it != sAnimList.end(); ++it) {
        amountPurged += (*it)->Purge(agressionLevel);
        if (amountPurged >= sizeRequest)
            break;
    }
    if (amountPurged >= sizeRequest)
        return true;
    return false;
}

// 0x1001dbf0 ?AnimLowMemHandler@@YA_NK@Z
bool AnimLowMemHandler(unsigned long sizeRequest)
{
    bool result;
    unsigned long agressionLevel = 0;
    do {
        result = CMBAnim::LowMemHandler(sizeRequest, agressionLevel);
        agressionLevel++;
    } while (!result && agressionLevel < 4);
    return result;
}

extern "C" {
// 0x1001dc10 SetBit
static void SetBit(unsigned long &bitField, unsigned long bit, bool value)
{
    if (value)
        bitField |= bit;
    else
        bitField &= ~bit;
}

// 0x1001dc30 TestBit
static bool TestBit(unsigned long bitField, unsigned long bit)
{
    return (bitField & bit) != 0;
}
}

// 0x1001dc50 ?Scale@CMBAnim@@UAEXMMM@Z
void CMBAnim::Scale(float sx, float sy, float sz)
{
    mScaleX = sx;
    mScaleY = sy;
    mScaleZ = sz;
}


// Matched by this code:
// 0x10018a80 ?Release@?$MBComPtr@VIMBPeriodicalManager@@@@QAEXXZ
// 0x1001ab50 _$E69
// 0x1001ab90 _$E68
// 0x1001aba0 _$E66
// 0x1001ac20 _$E72
// 0x1001dc80 ?size@?$vector@PAVMBAnimCast@@V?$MBAllocator@PAVMBAnimCast@@@@@std@@QBEIXZ
// 0x1001dca0 _$E15
// 0x1001dcb0 _$E14
// 0x1001dd80 _$E18
// 0x1001dd90 _$E17
// 0x1001de60 _$E21
// 0x1001de70 _$E20
// 0x1001df40 _$E24
// 0x1001df50 _$E23
// 0x1001e010 _$E27
// 0x1001e020 _$E26
// 0x1001e0e0 _$E30
// 0x1001e0f0 _$E29
// 0x1001e1a0 _$E33
// 0x1001e1b0 _$E32
// 0x1001e260 _$E36
// 0x1001e270 _$E35
// 0x1001e320 _$E39
// 0x1001e330 _$E38
// 0x1001e3f0 _$E42
// 0x1001e400 _$E41
// 0x1001e4c0 _$E45
// 0x1001e4d0 _$E44
// 0x1001e580 _$E48
// 0x1001e590 _$E47
// 0x1001e640 _$E51
// 0x1001e650 _$E50
// 0x1001e700 _$E54
// 0x1001e710 _$E53
// 0x1001e7c0 _$E57
// 0x1001e7d0 _$E56
// 0x1001e880 _$E60
// 0x1001e890 _$E59
// 0x1001e980 ?_Destroy@?$vector@PAVMBAnimSequence@@V?$MBAllocator@PAVMBAnimSequence@@@@@std@@IAEXPAPAVMBAnimSequence@@0@Z
// 0x1001e990 ?_Ucopy@?$vector@PAVMBAnimSequence@@V?$MBAllocator@PAVMBAnimSequence@@@@@std@@IAEPAPAVMBAnimSequence@@PBQAV3@0PAPAV3@@Z
// 0x1001e9c0 ?_Destroy@?$vector@PAVMBAnimCast@@V?$MBAllocator@PAVMBAnimCast@@@@@std@@IAEXPAPAVMBAnimCast@@0@Z
// 0x1001e9d0 ?_Ucopy@?$vector@PAVMBAnimCast@@V?$MBAllocator@PAVMBAnimCast@@@@@std@@IAEPAPAVMBAnimCast@@PBQAV3@0PAPAV3@@Z
// 0x1001ea20 ?insert@?$vector@PAVMBAnimSequence@@V?$MBAllocator@PAVMBAnimSequence@@@@@std@@QAEXPAPAVMBAnimSequence@@IABQAV3@@Z
// 0x1001ec30 ?insert@?$vector@PAVMBAnimCast@@V?$MBAllocator@PAVMBAnimCast@@@@@std@@QAEXPAPAVMBAnimCast@@IABQAV3@@Z
// 0x1001ee40 ?_Ufill@?$vector@PAVMBAnimSequence@@V?$MBAllocator@PAVMBAnimSequence@@@@@std@@IAEXPAPAVMBAnimSequence@@IABQAV3@@Z
// 0x1001ee70 ?_Ufill@?$vector@PAVMBAnimCast@@V?$MBAllocator@PAVMBAnimCast@@@@@std@@IAEXPAPAVMBAnimCast@@IABQAV3@@Z
// 0x1001eea0 ?_Construct@std@@YAXPAPAVMBAnimSequence@@ABQAV2@@Z
// 0x1001eec0 ?_Construct@std@@YAXPAPAVMBAnimCast@@ABQAV2@@Z
// 0x1001eee0 _$E79
// 0x1001ef00 _$E78
// 0x1001ef10 ?QueryInterface@CMBAnim@@W3AEJABUMBGUID@@PAPAX@Z
// 0x1001ef20 ?AddRef@CMBAnim@@W3AEKXZ
// 0x1001ef30 ?Release@CMBAnim@@W3AEKXZ
// 0x1001ef40 ?QueryInterface@CMBAnim@@W7AEJABUMBGUID@@PAPAX@Z
// 0x1001ef50 ?AddRef@CMBAnim@@W7AEKXZ
// 0x1001ef60 ?Release@CMBAnim@@W7AEKXZ
// 0x1003a440 _$E15
// 0x1003a450 _$E14
// 0x1003a520 _$E18
// 0x1003a530 _$E17
// 0x1003a620 _$E21
// 0x1003a630 _$E20
// 0x1003a700 _$E24
// 0x1003a710 _$E23
// 0x1003a7d0 _$E27
// 0x1003a7e0 _$E26
// 0x1003a8a0 _$E30
// 0x1003a8b0 _$E29
// 0x1003a960 _$E33
// 0x1003a970 _$E32
// 0x1003aa20 _$E36
// 0x1003aa30 _$E35
// 0x1003aae0 _$E39
// 0x1003aaf0 _$E38
// 0x1003abe0 _$E42
// 0x1003abf0 _$E41
// 0x1003acb0 _$E45
// 0x1003acc0 _$E44
// 0x1003ad70 _$E48
// 0x1003ad80 _$E47
// 0x1003ae30 _$E51
// 0x1003ae40 _$E50
// 0x1003aef0 _$E54
// 0x1003af00 _$E53
// 0x1003afb0 _$E57
// 0x1003afc0 _$E56
// 0x1003b070 _$E60
// 0x1003b080 _$E59

// TODO 0x1001dc70 ??Eiterator@?$list@PAVCMBAnim@@V?$MBAllocator@PAVCMBAnim@@@@@std@@QAE?AV012@H@Z: STL instance (body in the VC6 <list>), not emitted: our compile inlines every call to it
// TODO 0x1001e940 ?erase@?$list@PAVCMBAnim@@V?$MBAllocator@PAVCMBAnim@@@@@std@@QAE?AViterator@12@V312@@Z: STL instance (body in the VC6 <list>), not emitted: our compile inlines every call to it
