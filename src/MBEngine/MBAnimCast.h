// MBAnimCast unit declarations (MBEngine.dll MBAnimCast.obj)
// Colour header copied from CMBDirectDrawSurface.h (default-arg MBColor with mColor = 0;
// Set), stream/screen interfaces from CMBFont.h, Squish subset from Squish.h.
#ifndef MBANIMCAST_H
#define MBANIMCAST_H

// ---- colours (header statics: $E1..$E47 come first) ----
class MBColorMask {
public:
    MBColorMask(unsigned long r = 0, unsigned long g = 0, unsigned long b = 0, unsigned long a = 0) { Set(r, g, b, a); }
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

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// ---- geometry ----
struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

class MBSize {
public:
    MBSize(void) { cx = 0; cy = 0; }
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

class MBRect {
public:
    long left;
    long top;
    long right;
    long bottom;
};

struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};

class MBPalette;

// ---- interfaces ----
enum IMBStreamSeekValues {
    kIMBStreamSeekStart = 1,
    kIMBStreamSeekEnd = 2,
    kIMBStreamSeekCurrent = 3
};
enum IMBStreamError { kIMBStreamErrorDummy };

class IMBStream : public IMBUnknown {
public:
    virtual unsigned char Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual unsigned char Flush(void) = 0;
    virtual unsigned char SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

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

class IMBGameClock : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBGameClock; }
    virtual unsigned long GetRealTime(void) = 0;
    virtual unsigned long GetGameTime(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Unpause(void) = 0;
    virtual void SetTime(unsigned long) = 0;
    virtual void AddTime(unsigned long) = 0;
    virtual void SetMSPerSecond(long) = 0;
};

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
    kSquishPixFmtARGB8888 = 64,
    kSquishPixFmtBGRA8888 = 128
};

enum {
    kSquishPixFmtMask = 0x1ff,
    kSquishCompFmtMask = 0xe00,
    kSquishFlagTrans = 0x1000,
    kSquishFlagPalette = 0x2000
};

class CMBStreamSwapLSB {
public:
    CMBStreamSwapLSB(IMBStream *s) : mStream(s) {}
    operator IMBStream *(void) { return mStream; }
    IMBStream *operator->(void) { return mStream; }
    IMBStream *mStream;
};

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
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

template <class T> class MBVector : public std::vector<T, MBAllocator<T> > {
};

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
    bool ReloadSquishFrame(bool needToSeek);
    bool SquishFrameToSurface(void);
    MBPoint mRegistrationPoint;     // +0x08
    MBPoint mAttachmentPoint;       // +0x10
    MBSize mSize;                   // +0x18
    MBComPtr<IMBSurface> mSurface;  // +0x20
    MBComPtr<IMBStream> mSource;    // +0x24
    unsigned long mSourceOffset;    // +0x28
    SquishImageFrame *mFrame;       // +0x2c
    unsigned long mDepth;           // +0x30
    unsigned long mSquishPixelFmt;  // +0x34
    unsigned long mLastAccess;      // +0x38
    bool mCanBePurged;              // +0x3c
    bool mHasAttachmentPoint;       // +0x3d
private:
    ~MBAnimCast(void);
};

#endif
