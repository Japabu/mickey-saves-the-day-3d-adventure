// CMBUniFont.h - declarations for MBEngine.dll CMBUniFont.obj
// Prologue/colours as CMBTextControl.h (MBString16 instantiated before the colours: $E15..),
// screen/surface/Squish declarations from CMBFont.h, IMBPref from CMBStringTable.h.
#ifndef CMBUNIFONT_H
#define CMBUNIFONT_H

#include <stdio.h>
#include <string>
#include <vector>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) char *__cdecl MBstrncpy(char *, const char *, unsigned int);
extern "C" __declspec(dllimport) long __cdecl MBatol(const char *);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

// mbi18n.h
extern "C" {
void MB_utf8_to_ucs2(const MBString &utf8string, MBString16 &unicode);
void MB_ucs2_to_utf8(const MBString16 &unicode, MBString &utf8);
}
inline MBString16 MB_utf8_to_ucs2(const MBString &utf8string)
{
    MBString16 unicode;
    MB_utf8_to_ucs2(utf8string, unicode);
    return unicode;
}

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &g) const {
        return data1 == g.data1 && data2 == g.data2 && data3 == g.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)g.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)g.data4)[1];
    }
};

// PDB ldata order in CMBUniFont.obj (0x1008fff0..0x10090090)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont = { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBBitmapFont = { 0x800699f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBPref = { 0x79fc3030, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPref = { 0x7a00c5c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
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
            T *p;
            long hr = unk->QueryInterface(T::GetGUID(), (void **)&p);
            if (hr)
                Attach(0);
            else
                Attach(p);
            unk->Release();
            return hr;
        }
        return -2;
    }
    T *mInterface;
};

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
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
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

// ---- colors (header statics: every including TU has its own $E initializers) ----
#include "MBColorMask.h"

static const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

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

#include "MBPixelFormat.h"

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

// own copy: a default argument for the MBString16 GetString only (NOTES.md: the MBString one shifts a $E here)
class IMBPref : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPref; }
    // overloads in reverse declaration order (see IMBFont)
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString16 &,
                           const MBString16 & = MBString16()) = 0;                     // vftable+0x10
    // no `= MBString()` default here: MBString's npos must be numbered after sCachedScreenPixelFormat ($E64)
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString &,
                           const MBString &) = 0;                                      // vftable+0x0c
    virtual long GetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString16 &) = 0; // vftable+0x1c
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString &) = 0;   // vftable+0x18
    virtual long SetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
};

// ---- stream helpers ----
#include "CMBStreamSwapLSB.h"

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


// ---- CMBUniFont ----
class CMBUniFont : public IMBBitmapFont {
public:
    CMBUniFont(void);
    virtual ~CMBUniFont(void);
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
    void LoadFontFrame(int index);
    void LoadFontFrameWidths(int index, MBString &widths);
    unsigned short FindSurfaceId(unsigned short &thechar);
    MBComPtr<IMBStream> mFontStream;                         // +0x08
    MBComPtr<IMBStream> mPrefStream;                         // +0x0c
    SquishMovie mMovie;                                      // +0x10
    unsigned long mColorMask;                                // +0x28
    unsigned long mFrameCount;                               // +0x2c
    unsigned short mPointSize;                               // +0x30
    unsigned short mCharHeightOffset;                        // +0x32
    MBVector<IMBSurface *> mFontSurfaces;                    // +0x34
    MBVector<std::pair<unsigned short, unsigned short> > mFontRanges; // +0x44
    MBVector<unsigned short *> mFontWidthMaps;               // +0x54
    static MBPixelFormat sCachedScreenPixelFormat;
};

IMBUnknown *MakeCMBUniFont(void);

#endif
