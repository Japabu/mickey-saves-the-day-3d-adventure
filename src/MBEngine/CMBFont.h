// CMBFont.h - declarations for MBEngine.dll CMBFont.obj
#ifndef CMBFONT_H
#define CMBFONT_H

typedef unsigned int size_t;

// ---- MBColor header (copied from CMBSurfaceObject.h): $E1..$E47 come first ----
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

// PDB ldata order in CMBFont.obj (0x1008fe58..0x1008fed8)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
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

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

struct MBPoint {
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    long x;
    long y;
};

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

struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};

// ---- interfaces ----
class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

enum IMBStreamSeekValues { kIMBStreamSeekDummy };
enum IMBStreamError { kIMBStreamErrorDummy };

class IMBStream : public IMBUnknown {
public:
    virtual unsigned char Read(void *, unsigned long &) = 0;
    virtual unsigned char Write(const void *, unsigned long) = 0;
    virtual unsigned char Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual unsigned char Flush(void) = 0;
    virtual unsigned char SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

class MBPalette;

class IMBSurface : public IMBUnknown {
public:
    virtual unsigned long GetWidth(void) = 0;
    virtual unsigned long GetHeight(void) = 0;
    virtual unsigned long GetPitch(void) = 0;
    virtual unsigned long GetSize(void) = 0;
    virtual unsigned long GetDepth(void) = 0;
    virtual MBPixelFormat GetFormat(void) = 0;
    virtual long Lock(void **, unsigned long) = 0;
    virtual long Unlock(void **) = 0;
    virtual MBColor GetPixelColor(unsigned long, unsigned long) = 0;
    virtual long SetTransparentColor(MBColor) = 0;
    virtual MBColor GetTransparentColor(void) = 0;
    virtual long SetPalette(MBPalette *) = 0;
    virtual MBPalette *GetPalette(void) = 0;
    virtual long ColorFill(MBColor, MBRect *) = 0;
    virtual long Blit(IMBSurface *, MBRect *, MBRect *, unsigned long) = 0;
};

enum MBScreenState { kMBScreenStateDummy };
struct MBScreenAttributes;

class IMBScreen : public IMBUnknown {
public:
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual unsigned char CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
};

class IMBScreenManager : public IMBUnknown {
public:
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
    virtual void GetScreenByID(long, IMBScreen **) = 0;
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0;
};

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

class IMBFont : public IMBUnknown {
public:
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual long DrawText(IMBSurface *, long, long, const unsigned short *const, unsigned long) = 0; // +0x18
    virtual long DrawText(IMBSurface *, MBRect &, const unsigned short *const, unsigned long) = 0;    // +0x14
    virtual long DrawText(IMBSurface *, long, long, const char *const, unsigned long) = 0;           // +0x10
    virtual long DrawText(IMBSurface *, MBRect &, const char *const, unsigned long) = 0;              // +0x0c
    virtual long GetTextSize(IMBSurface *, const unsigned short *, unsigned long, MBSize &) = 0;
    virtual long GetTextSize(IMBSurface *, const char *, unsigned long, MBSize &) = 0;
};

class IMBBitmapFont : public IMBFont {
public:
    virtual long LoadFont(unsigned long, IMBStream *) = 0;
    virtual long LoadCharacterMap(unsigned long, IMBStream *) = 0;
};

// ---- stream helpers ----
class CMBStreamSwapLSB {
public:
    CMBStreamSwapLSB(IMBStream *s) : mStream(s) {}
    IMBStream *operator->(void) { return mStream; }
    void Read(unsigned short &v) {
        unsigned long size = 2;
        mStream->Read(&v, size);
    }
    IMBStream *mStream;
};

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
enum SquishFileOpenFlags {
    kSquishFileOpenReadBit = 1,
    kSquishFileOpenWriteBit = 2,
    kSquishFileOpenMustExist = 4
};

enum SquishFileSeekValues {
    kSquishFileSeekStart = 1,
    kSquishFileSeekEnd = 2,
    kSquishFileSeekCurrent = 3
};

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

struct SquishPixel {
    union SquishPixelUnion {
        unsigned char pixel[4];
        unsigned long bgra32;
    };
    SquishPixelUnion Pixel;
    SquishPixel(void) { Pixel.bgra32 = 0xffffffff; }
    SquishPixel(unsigned long p) { Pixel.bgra32 = p; }
    unsigned char Red(void) { return Pixel.pixel[2]; }
    unsigned char Green(void) { return Pixel.pixel[1]; }
    unsigned char Blue(void) { return Pixel.pixel[0]; }
    unsigned char Alpha(void) { return Pixel.pixel[3]; }
};

class SquishPalette;

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

class SquishImageBlock {
public:
    short xOffset;
    short yOffset;
    SquishImageDescriptor desc;
    unsigned char *mData;
};

template <class T> class MBAllocator : public std::allocator<T> {
};

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
