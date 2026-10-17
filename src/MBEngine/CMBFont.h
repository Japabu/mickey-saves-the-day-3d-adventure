// CMBFont.h - declarations for MBEngine.dll CMBFont.obj
#ifndef CMBFONT_H
#define CMBFONT_H

typedef unsigned int size_t;

// ---- MBColor header (copied from CMBSurfaceObject.h): $E1..$E47 come first ----
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
class MBColorMask {
public:
    MBColorMask(void) { Set(0, 0, 0, 0); }
    MBColorMask(unsigned long redmask, unsigned long greenmask, unsigned long bluemask,
                unsigned long alphamask) { Set(redmask, greenmask, bluemask, alphamask); }
    void Set(unsigned long redmask, unsigned long greenmask, unsigned long bluemask,
             unsigned long alphamask) {
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
        while (!(mask & 1) && count < 32) {
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

static const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        mRed = r;
        mBlue = b;
        mGreen = g;
        mAlpha = a;
        Convert(kCM_ARGB32);
    }
    unsigned long Convert(MBColorMask mask) {
        mColor = 0;
        if (mask.mRedShift > 0)
            mColor |= ((unsigned long)mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor |= ((unsigned long)mRed >> -mask.mRedShift) & mask.mRedMask;
        if (mask.mGreenShift > 0)
            mColor |= ((unsigned long)mGreen << mask.mGreenShift) & mask.mGreenMask;
        else
            mColor |= ((unsigned long)mGreen >> -mask.mGreenShift) & mask.mGreenMask;
        if (mask.mBlueShift > 0)
            mColor |= ((unsigned long)mBlue << mask.mBlueShift) & mask.mBlueMask;
        else
            mColor |= ((unsigned long)mBlue >> -mask.mBlueShift) & mask.mBlueMask;
        if (mask.mAlphaShift > 0)
            mColor |= ((unsigned long)mAlpha << mask.mAlphaShift) & mask.mAlphaMask;
        else
            mColor |= ((unsigned long)mAlpha >> -mask.mAlphaShift) & mask.mAlphaMask;
        return mColor;
    }
    unsigned char mRed;
    unsigned char mGreen;
    unsigned char mBlue;
    unsigned char mAlpha;
    unsigned long mColor;
};

static const MBColor kWhite(255, 255, 255, 0);
static const MBColor kBlack(0, 0, 0, 0);
static const MBColor kRed(255, 0, 0, 0);
static const MBColor kGreen(0, 255, 0, 0);
static const MBColor kBlue(0, 0, 255, 0);
static const MBColor kYellow(255, 255, 0, 0);
static const MBColor kCyan(0, 255, 255, 0);
static const MBColor kMagenta(255, 0, 255, 0);
static const MBColor kGrey25(64, 64, 64, 0);
static const MBColor kGrey50(128, 128, 128, 0);
static const MBColor kGrey75(192, 192, 192, 0);

#include <string> // evidence: ctype<unsigned short>::id $E70/$E71 after the colour statics

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { if (p) MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// PDB ldata order in CMBFont.obj (0x1008fe58..0x1008fed8)
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont =
    { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBBitmapFont =
    { 0x800699f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// ---- geometry ----
template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }
template <class T> inline const T &MBmin(const T &a, const T &b) { return b < a ? b : a; }

#include "MBSize.h"

// own copy: a struct without the MBPointStruct base and without a default constructor
struct MBPoint {
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    long x;
    long y;
};

// own copy: built on this unit's own MBPoint
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBPoint &p, const MBSize &s) { left = p.x; top = p.y; right = p.x + s.cx; bottom = p.y + s.cy; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    void SetRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    void SetSize(const MBSize &s) { right = left + s.cx; bottom = top + s.cy; }
    unsigned char Intersect(const MBRect &r, MBRect &out) const {
        if (right >= r.left && left <= r.right && bottom >= r.top && top <= r.bottom) {
            out.left = MBmax(left, r.left);
            out.top = MBmax(top, r.top);
            out.right = MBmin(right, r.right);
            out.bottom = MBmin(bottom, r.bottom);
            return true;
        }
        return false;
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

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

class MBPalette;

#include "IMBSurface.h"

#include "MBScreenState.h"
struct MBScreenAttributes;

#include "IMBScreen.h"

#include "IMBScreenManager.h"

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

#include "IMBFont.h"

#include "IMBBitmapFont.h"

// ---- stream helpers ----
#include "CMBStreamSwapLSB.h"

struct CharMapFile {
    unsigned short version;
    unsigned short pointSize;
    unsigned short charHeightOffset;
    unsigned short charWidthOffset;
    unsigned char charMap[256];
    unsigned short charWidth[256];
    void Read(CMBStreamSwapLSB &stream) {
        unsigned long size;
        size = 2;
        stream->Read(&version, size);
        size = 2;
        stream->Read(&pointSize, size);
        size = 2;
        stream->Read(&charHeightOffset, size);
        size = 2;
        stream->Read(&charWidthOffset, size);
        unsigned long count = 256;
        stream->Read(charMap, count);
        for (unsigned long i = 0; i < count;) {
            unsigned short *width = &charWidth[i++];
            size = 2;
            stream->Read(width, size);
        }
    }
};

// ---- Squish (subset of Squish.h; SquishFile/SquishStream inline here) ----
#include "SquishFileOpenFlags.h"

#include "SquishFileSeekValues.h"

enum {
    kSquishPixFmtARGB1555 = 4,
    kSquishPixFmtRGB565 = 8,
    kSquishPixFmtBGRA8888 = 128
};

enum {
    kSquishPixFmtMask = 0x1ff,
    kSquishCompFmtMask = 0xe00,
    kSquishFlagTrans = 0x1000,
    kSquishFlagPalette = 0x2000
};

// own copy: constructor and destructor (MBAnimCast: also the virtuals) are inline in this view; Squish.h has them out of line
class SquishFile {
public:
    SquishFile(void) {}
    virtual ~SquishFile(void) {}
    virtual bool Open(const char *, SquishFileOpenFlags);
    virtual bool Close(void);
    virtual bool WriteBytes(const void *, unsigned long) = 0;
    virtual bool ReadBytes(void *, unsigned long) = 0;
    virtual bool Seek(long, SquishFileSeekValues);
};

// own copy: derives from this unit's own SquishFile
class SquishStream : public SquishFile {
public:
    SquishStream(const CMBStreamSwapLSB &stream) : mStream(stream) {} // by value in the PDB; by value is not inlined
    virtual ~SquishStream(void) {}
    virtual bool Open(const char *filename, SquishFileOpenFlags flags);
    virtual bool Close(void);
    virtual bool WriteBytes(const void *ptr, unsigned long nBytes);
    virtual bool ReadBytes(void *ptr, unsigned long nBytes);
    virtual bool Seek(long offset, SquishFileSeekValues origin);
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
};

// own copy: a view with placeholder storage for the MBVector of blocks
class SquishImageFrame {
public:
    unsigned long GetWidth(void) const { return mWidth; }
    unsigned long GetHeight(void) const { return mHeight; }
    SquishImageBlock *GetBlock(unsigned int index) const;
    bool Blit(unsigned char *dest, const SquishImageDescriptor *destFmt) const;
    void *mBlocks[4];
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
};

// own copy: a view with placeholder storage for the MBVector of frames
class SquishMovie {
public:
    SquishMovie(void);
    ~SquishMovie(void);
    SquishImageFrame *GetFrame(unsigned int index) const;
    bool StreamIn(SquishFile *file);
    bool Remap(unsigned long newDepth, unsigned long newPixFmt, SquishPalette *newPalette);
protected:
    void *mFrames[4];
    unsigned short mWidth;
    unsigned short mHeight;
    unsigned short mDepth;
};

// ---- CMBFont ----
class CMBFont : public IMBBitmapFont {
public:
    CMBFont(void);
    virtual ~CMBFont(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual long LoadFont(unsigned long id, IMBStream *in);
    virtual long LoadCharacterMap(unsigned long id, IMBStream *in);
    virtual long DrawText(IMBSurface *dest, MBRect &destRect, const unsigned short *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, long x, long y, const unsigned short *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, MBRect &destRect, const char *const text, unsigned long count);
    virtual long DrawText(IMBSurface *dest, long x, long y, const char *const text, unsigned long count);
    virtual long GetTextSize(IMBSurface *dest, const unsigned short *text, unsigned long count, MBSize &size);
    virtual long GetTextSize(IMBSurface *dest, const char *text, unsigned long count, MBSize &size);
protected:
    IMBSurface *mFontSurface;          // +0x08
    unsigned short mPointSize;         // +0x0c
    unsigned short mCharHeightOffset;  // +0x0e
    unsigned short mCharWidthOffset;   // +0x10
    unsigned short mCharWidths[256];   // +0x12
    unsigned char mCharMap[256];       // +0x212
    static MBPixelFormat sCachedScreenPixelFormat;
};

IMBUnknown *MakeCMBFont(void);

#endif
