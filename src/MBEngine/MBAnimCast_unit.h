// MBAnimCast unit declarations (MBEngine.dll MBAnimCast.obj)
// Colour header copied from CMBDirectDrawSurface.h (default-arg MBColor with mColor = 0;
// Set), stream/screen interfaces from CMBFont.h, Squish subset from Squish.h.
#ifndef MBANIMCAST_UNIT_H
#define MBANIMCAST_UNIT_H

// ---- colours (header statics: $E1..$E47 come first) ----
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0) { mColor = 0; Set(r, g, b, a); }
    void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
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

#include <string> // evidence: ctype<unsigned short>::id $E66/$E67 after the colour statics
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/IMBUnknown.h"

static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// ---- geometry ----
#include "MBPointStruct.h"

#include "../MBEngine/MBPoint.h"

#include "MBSize.h"

// own copy: a plain struct without constructors
class MBRect {
public:
    long left;
    long top;
    long right;
    long bottom;
};

#include "MBPixelFormat.h"

class MBPalette;

// ---- interfaces ----
#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBStream.h"

static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBSurface.h"

#include "MBScreenState.h"
struct MBScreenAttributes;

static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBScreen.h"

static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBScreenManager.h"

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

#include "IMBGameClock.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
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
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    long CreateObject(const MBGUID &clsid) {
        IMBUnknown *unk;
        Release();
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p) {
        long hr = -3;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
            return hr;
        }
        Attach(0);
        return hr;
    }
    T *mInterface;
};

// ---- Squish subset (Squish.h); SquishFile/SquishStream virtuals inline here ----
#include "SquishFileOpenFlags.h"

#include "SquishFileSeekValues.h"

enum {
    kSquishPixFmtARGB1555 = 4,
    kSquishPixFmtRGB565 = 8,
    kSquishPixFmtARGB8888 = 64,
    kSquishPixFmtBGRA8888 = 128
};

enum {
    kSquishPixFmtMask = 0x1ff,
    kSquishCompFmtMask = 0xe00,
    kSquishFlagTrans = 0x1000,
    kSquishFlagPalette = 0x2000
};

#include "CMBStreamSwapLSB.h"

// own copy: constructor and destructor (MBAnimCast: also the virtuals) are inline in this view; Squish.h has them out of line
class SquishFile {
public:
    SquishFile(void) {}
    virtual ~SquishFile(void) {}
    virtual bool Open(const char *, SquishFileOpenFlags) { return false; }
    virtual bool Close(void) { return false; }
    virtual bool WriteBytes(const void *, unsigned long) = 0;
    virtual bool ReadBytes(void *, unsigned long) = 0;
    virtual bool Seek(long, SquishFileSeekValues) { return false; }
};

// own copy: derives from this unit's own SquishFile
class SquishStream : public SquishFile {
public:
    SquishStream(const CMBStreamSwapLSB &stream) : mStream(stream) {} // PDB: by value; by value is not inlined (CMBFont)
    virtual ~SquishStream(void) {}
    virtual bool Open(const char *filename, SquishFileOpenFlags flags) { return true; }
    virtual bool Close(void) { return true; }
    virtual bool WriteBytes(const void *ptr, unsigned long nBytes) {
        if (!mStream)
            return false;
        return mStream->Write(ptr, nBytes);
    }
    virtual bool ReadBytes(void *ptr, unsigned long nBytes) {
        if (!mStream)
            return false;
        if (!mStream->Read(ptr, nBytes) && mStream->GetLastError() != 6)
            return false;
        return true;
    }
    virtual bool Seek(long offset, SquishFileSeekValues origin) {
        if (!mStream)
            return false;
        IMBStreamSeekValues streamOrigin;
        switch (origin) {
        case kSquishFileSeekStart:
            streamOrigin = kIMBStreamSeekStart;
            break;
        case kSquishFileSeekEnd:
            streamOrigin = kIMBStreamSeekEnd;
            break;
        case kSquishFileSeekCurrent:
            streamOrigin = kIMBStreamSeekCurrent;
            break;
        }
        return mStream->Seek(offset, streamOrigin);
    }
    CMBStreamSwapLSB mStream;
    unsigned char *mOrigin;
    unsigned char *mBuffer;
    unsigned int mBytesAvail;
    unsigned int mSize;
};

#include "SquishPixel.h"

class SquishPalette;

// own copy: GetTransColor returns transColor as is and no copy constructor or assignment is declared
class SquishImageDescriptor {
public:
    unsigned short width;
    unsigned short height;
    unsigned short depth;
    unsigned short rowbytes;
    unsigned long datasize;
    unsigned long flags;
protected:
    SquishPixel transColor;
    unsigned long nativeTransColor;
    SquishPalette *palette;
public:
    SquishImageDescriptor(void);
    ~SquishImageDescriptor(void);
    void Reset(void);
    SquishPixel GetTransColor(void) const { return transColor; }
    bool TransEnabled(void) const { return (flags >> 12) & 1; }
    void SetPixelFmt(unsigned long pixfmt);
    void SetCompFmt(unsigned long f) {
        flags &= ~kSquishCompFmtMask;
        flags |= f;
    }
};

// own copy: a data-only view over this unit's own SquishImageDescriptor
class SquishImageBlock {
public:
    short xOffset;
    short yOffset;
    SquishImageDescriptor desc;
    unsigned char *mData;
};

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

// own copy: a view: this unit declares no SquishRect for the inline BlitBlock wrappers
class SquishImageFrame {
public:
    SquishImageFrame(void);
    ~SquishImageFrame(void);
    unsigned long GetWidth(void) const { return mWidth; }
    unsigned long GetHeight(void) const { return mHeight; }
    bool StreamIn(SquishFile *file);
    SquishImageBlock *GetBlock(unsigned int index) const;
    bool Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
    bool Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette);
    MBVector<SquishImageBlock *> mBlocks;
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
};

// ---- MBAnimCast ----
#include "MBAnimCast.h"

#endif
