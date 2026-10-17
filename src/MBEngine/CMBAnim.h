// CMBAnim.h - declarations for MBEngine.dll CMBAnim.obj
// Colour header and interfaces copied from CMBDragObject.h, MBComPtr from CMBGameEngine.h,
// MBAnimSequence/MBAnimCast declarations from MBAnimSequence.h / MBAnimCast.h.
#ifndef CMBANIM_H
#define CMBANIM_H

typedef unsigned int size_t;

#include <string>
#include <vector>
#include <list>

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
class MBColorMask {
public:
    MBColorMask(void) { Set(0, 0, 0, 0); }
    MBColorMask(unsigned long r, unsigned long g, unsigned long b, unsigned long a) { Set(r, g, b, a); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask, unsigned long alphamask) {
        mRedMask = redmask;
        mBlueMask = bluemask;
        mGreenMask = greenmask;
        mAlphaMask = alphamask;
        mRedShift = GetShiftCount(redmask) + GetBitCount(redmask) - 8;
        mBlueShift = GetShiftCount(bluemask) + GetBitCount(bluemask) - 8;
        mGreenShift = GetShiftCount(greenmask) + GetBitCount(greenmask) - 8;
        mAlphaShift = GetShiftCount(alphamask) + GetBitCount(alphamask) - 8;
    }
    unsigned long mRedMask;
    unsigned long mBlueMask;
    unsigned long mGreenMask;
    unsigned long mAlphaMask;
    char mRedShift;
    char mBlueShift;
    char mGreenShift;
    char mAlphaShift;
    static unsigned char GetShiftCount(unsigned long mask) {
        unsigned char count = 0;
        while (!(mask & 1)) {
            if (count >= 32)
                break;
            mask >>= 1;
            count++;
        }
        return count;
    }
    static unsigned char GetBitCount(unsigned long mask) {
        unsigned char count = 0;
        while (mask) {
            mask &= mask - 1;
            count++;
        }
        return count;
    }
};

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: built on this unit's own MBColorMask
class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        mColor = Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor |= (mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor |= (mRed >> (char)-mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= (mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= (mGreen >> (char)-mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= (mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= (mBlue >> (char)-mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= (mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= (mAlpha >> (char)-mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    bool operator!=(const MBColor &c) const {
        return mRed != c.mRed || mGreen != c.mGreen || mBlue != c.mBlue || mAlpha != c.mAlpha;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

const MBColor kWhite(255, 255, 255, 0);
const MBColor kBlack(0, 0, 0, 0);
const MBColor kRed(255, 0, 0, 0);
const MBColor kGreen(0, 255, 0, 0);
const MBColor kBlue(0, 0, 255, 0);
const MBColor kYellow(255, 255, 0, 0);
const MBColor kCyan(0, 255, 255, 0);
const MBColor kMagenta(255, 0, 255, 0);
const MBColor kGrey25(64, 64, 64, 0);
const MBColor kGrey50(128, 128, 128, 0);
const MBColor kGrey75(192, 192, 192, 0);


extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);
extern "C" __declspec(dllimport) int MBstrcmp(const char *a, const char *b);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    unsigned char operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

// PDB ldata order in CMBAnim.obj (0x1008a808..0x1008a8f8)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim =
    { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile =
    { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList =
    { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// No construct/destroy: the original calls std::_Construct out of line (vector insert)
template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

// Stand-in (see NOTES): the PDB numbering ($E65 for CMBAnim::sAnimList, ctype $E78) needs
// MBString (npos) instantiated before the class statics; the original header use is unknown.
inline void UnknownMBStringInstantiation(void) { MBString s; }

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) {
        IMBUnknown *unk;
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            _QueryInterface(unk);
            unk->Release();
        }
    }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            T *pInterface;
            long hr = unk->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr)
                Attach(0);
            else
                Attach(pInterface);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    T *mInterface;
};

// ---- geometry ----
#include "MBPointStruct.h"

// own copy: this unit uses the default constructor (x = y = 0), whose inline body here differs from MBPoint.h (store order / code shape the matches depend on)
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

#include "MBPointStruct3.h"

#include "MBPoint3.h"

#include "MBSize.h"

// own copy: built on this unit's own MBPoint
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    long left;
    long top;
    long right;
    long bottom;
};

// own copy: built on this unit's own MBColorMask
struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};

// ---- interfaces ----
class IMBSurface;
class IMBObjectGroup;
class MBPalette;

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBSurface.h"

#include "MBScreenState.h"
struct MBScreenAttributes;

#include "IMBScreen.h"

#include "IMBScreenManager.h"

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);
extern "C" void MBPlaySoundID(unsigned long id, long loopCount);

#include "IMBGameClock.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

class IMBEventListener;

#include "IMBEventManager.h"

#include "IMBResource.h"

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "MBAnimationProperty.h"

#include "MBSequenceProperty.h"

#include "MBFrameProperty.h"

#include "IMBAnim.h"

#include "MBAnimEventStruct.h"

// ---- MBAnimSequence / MBAnimCast (defined in their own objects) ----
#include "MBAnimSequence.h"

// own copy: built on this unit's own MBPoint
class MBAnimCast : public IMBUnknown {
public:
    MBAnimCast(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    MBSize GetSize(void) const;
    void GetSurface(unsigned long currentTime, IMBSurface **surface);
    void GetSurface(IMBSurface **surface);
    void SetSurface(IMBSurface *surface);
    bool IsSurfaceResident(void) const;
    MBPoint GetRegistrationPoint(void) const;
    void SetRegistrationPoint(const MBPoint &registrationPoint);
    MBPoint GetAttachmentPoint(void) const;
    void SetAttachmentPoint(const MBPoint &attachmentPoint);
    void ClearAttachmentPoint(void);
    bool HasAttachmentPoint(void) const;
    bool Init(IMBStream *stream, MBPixelFormat pf);
    unsigned long GetLastAccess(void) const;
    unsigned long Purge(void);
    unsigned long GetApproxSize(void) const;
    void Lock(void);
protected:
    MBPoint mRegistrationPoint;
    MBPoint mAttachmentPoint;
    MBSize mSize;
    MBComPtr<IMBSurface> mSurface;
    MBComPtr<IMBStream> mSource;
    unsigned long mSourceOffset;
    class SquishImageFrame *mFrame;
    unsigned long mDepth;
    unsigned long mSquishPixelFmt;
    unsigned long mLastAccess;
    bool mCanBePurged;
    bool mHasAttachmentPoint;
};

class MBAnimFrame;

// ---- CMBAnim ----
class CMBAnim : public IMBAnim, public IMBResource, public IMBPeriodical {
public:
    CMBAnim(void);
    ~CMBAnim(void);
    static bool LowMemHandler(unsigned long sizeRequest, unsigned long agressionLevel);
    static MBList<CMBAnim *> sAnimList;
    unsigned long Purge(unsigned long agressionLevel);
protected:
    bool ReadAnimInfo(IMBStream *stream, long version);
    bool ReadSequenceInfo(IMBStream *stream, long version, long sequenceIndex);
    bool ReadFrameInfo(IMBStream *stream, long version, long sequenceIndex, long frameIndex);
    bool ReadCast(IMBStream *stream, long version);
    bool ReadString(IMBStream *stream, MBString &string);
    bool WriteAnimInfo(IMBStream *stream);
    bool WriteSequenceInfo(IMBStream *stream, MBAnimSequence *sequence);
    bool WriteFrameInfo(IMBStream *stream, MBAnimSequence *sequence, MBAnimFrame *frame);
    bool WriteCast(IMBStream *stream);
    bool WriteString(IMBStream *stream, MBString &string);
    void SendMessage(long msg);
    unsigned long _mRefCnt;                     // +0x0c
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long Load(unsigned long id, IMBStream *in, bool unpackFrames);
    virtual long Save(IMBStream *out);
    virtual void Clone(IMBAnim **clone);
    virtual long SetAnimationProperty(MBAnimationProperty property, bool value);
    virtual long SetAnimationProperty(MBAnimationProperty property, unsigned short value);
    virtual long SetAnimationProperty(MBAnimationProperty property, unsigned long &value);
    virtual long SetAnimationProperty(MBAnimationProperty property, const MBString &value);
    virtual long SetSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool value);
    virtual long SetSequenceProperty(long sequenceIndex, MBSequenceProperty property, unsigned short value);
    virtual long SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool value);
    virtual long SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned short value);
    virtual long SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned long value);
    virtual long SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, const MBString &value);
    virtual long SetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, const MBPoint &value);
    virtual long GetAnimationProperty(MBAnimationProperty property, bool &value) const;
    virtual long GetAnimationProperty(MBAnimationProperty property, unsigned short &value) const;
    virtual long GetAnimationProperty(MBAnimationProperty property, unsigned long &value) const;
    virtual long GetSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool &value) const;
    virtual long GetSequenceProperty(long sequenceIndex, MBSequenceProperty property, unsigned short &value) const;
    virtual long GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool &value) const;
    virtual long GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned short &value) const;
    virtual long GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, unsigned long &value) const;
    virtual long GetFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, MBPoint &value) const;
    virtual long UseAnimationProperty(MBAnimationProperty property, bool use);
    virtual bool HasAnimationProperty(MBAnimationProperty property) const;
    virtual long UseSequenceProperty(long sequenceIndex, MBSequenceProperty property, bool use);
    virtual bool HasSequenceProperty(long sequenceIndex, MBSequenceProperty property) const;
    virtual long UseFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property, bool use);
    virtual bool HasFrameProperty(long sequenceIndex, long frameIndex, MBFrameProperty property) const;
    virtual void SetCastID(long sequenceIndex, long frameIndex, long castID);
    virtual long GetCastID(long sequenceIndex, long frameIndex) const;
    virtual void Play(void);
    virtual void Stop(void);
    virtual long Pause(void);
    virtual long Resume(void);
    virtual bool IsPaused(void) const;
    virtual bool IsStalled(void) const;
    virtual bool IsRendering(void) const;
    virtual bool IsPlaying(void) const;
    virtual void Scale(float sx, float sy, float sz);
    virtual unsigned long GetNumSequences(void) const;
    virtual long AddSequence(void);
    virtual MBAnimSequence *GetSequence(long sequenceIndex) const;
    virtual bool SetCurrentSequence(long sequenceIndex);
    virtual long GetCurrentSequenceIndex(void) const;
    virtual unsigned long GetNumFrames(long sequenceIndex) const;
    virtual long AddFrame(long sequenceIndex);
    virtual bool SetCurrentFrame(long frameIndex);
    virtual long GetCurrentFrameIndex(void) const;
    virtual unsigned long GetNumCasts(void) const;
    virtual long AddCastMember(IMBSurface *image, const MBPoint &registrationPoint);
    virtual void GetCastMemberSurface(long castID, IMBSurface **surface);
    virtual void SetCastMemberSurface(long castID, IMBSurface *surface);
    virtual MBPoint GetCastMemberRegistrationPoint(long castID) const;
    virtual void SetCastMemberRegistrationPoint(long castID, const MBPoint &registrationPoint);
    virtual MBPoint GetCastMemberAttachmentPoint(long castID) const;
    virtual void SetCastMemberAttachmentPoint(long castID, const MBPoint &attachmentPoint);
protected:
    virtual void AdvanceFrame(void);
    virtual MBAnimCast *GetCast(long castID) const;
public:
    virtual void Idle(void);
    virtual MBPoint3 GetOffset(void) const;
    virtual MBPoint3 GetPosition(void) const;
    virtual void Place(const MBPoint3 &point);
    virtual void Move(const MBPoint3 &point);
    virtual MBRect GetBounds(void) const;
    virtual bool IsVisible(void) const;
    virtual long Show(void);
    virtual long Hide(void);
    virtual bool IsDirty(void) const;
    virtual void SetDirty(void);
    virtual void ClearDirty(void);
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect) const;
    virtual void GetParent(IMBObjectGroup **parent) const;
protected:
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
public:
    virtual unsigned long GetID(void);
    virtual unsigned long GetType(void);
protected:
    long mCurrentSequenceIndex;                 // +0x10
    long mCurrentFrameIndex;                    // +0x14
    bool mPlaying;                              // +0x18
    bool mStalled;                              // +0x19
    bool mRender;                               // +0x1a
    long mPauseCount;                           // +0x1c
    unsigned long mProperties;                  // +0x20
    unsigned long mExternalCanvas;              // +0x24
    unsigned long mExternalSequence;            // +0x28
    unsigned short mDelay;                      // +0x2c
    unsigned short mZPlane;                     // +0x2e
    bool mQuietBackground;                      // +0x30
    bool mAutoUnload;                           // +0x31
    bool mDontAdjustZPlane;                     // +0x32
    MBVector<MBAnimSequence *> mSequences;      // +0x34
    MBVector<MBAnimCast *> mCasts;              // +0x44
    MBComPtr<IMBGameClock> mClock;              // +0x54
    IMBObjectGroup *mParent;                    // +0x58
    MBPoint3 mPos;                              // +0x5c
    long mHideCount;                            // +0x68
    bool mDirty;                                // +0x6c
    unsigned long mID;                          // +0x70
    long mNextFrameAt;                          // +0x74
    float mScaleX;                              // +0x78
    float mScaleY;                              // +0x7c
    float mScaleZ;                              // +0x80
    static MBPixelFormat sCachedScreenPixelFormat;
};

IMBUnknown *MakeCMBAnim(void);
bool AnimLowMemHandler(unsigned long sizeRequest);

#endif
