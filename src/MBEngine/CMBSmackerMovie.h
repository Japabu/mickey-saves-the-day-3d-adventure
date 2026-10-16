// CMBSmackerMovie.h - declarations for MBEngine.dll CMBSmackerMovie.obj
// Colour header copied from CMBView.h (16 statics = $E1..$E47, no STL numbers follow);
// interfaces from CMBSurfaceObject.h / CMBViewManager.h; RAD Smacker and Miles imports.
#ifndef CMBSMACKERMOVIE_H
#define CMBSMACKERMOVIE_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) void MBDebugStr(const char *fmt, ...);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

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

// PDB ldata order in CMBSmackerMovie.obj (0x1008a650..0x1008a730)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBStream =
    { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWinFile =
    { 0x89780490, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMemorySurface =
    { 0x7f30abb0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMovie =
    { 0x7f5cfc20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical =
    { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// ---- colour header (as CMBView.h) ----
class MBColorMask {
public:
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

struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};

// ---- geometry ----
struct MBPointStruct3 {
    long x;
    long y;
    long z;
};

class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(long ax, long ay, long az) { x = ax; y = ay; z = az; }
    MBPoint3(void) { x = y = z = 0; }
    MBPoint3 &operator+=(const MBPoint3 &p) { x += p.x; y += p.y; z += p.z; return *this; }
    MBPoint3 operator+(const MBPoint3 &p) const { return MBPoint3(x + p.x, y + p.y, z + p.z); }
};

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

class MBRect {
public:
    MBRect(MBPoint3 p, MBSize s) : left(p.x), top(p.y), right(p.x + s.cx), bottom(p.y + s.cy) {}
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    long left;
    long top;
    long right;
    long bottom;
};

// ---- interfaces ----
class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
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

class IMBObjectGroup;

class IMBObject : public IMBUnknown {
public:
    virtual MBPoint3 GetOffset(void) const = 0;
    virtual MBPoint3 GetPosition(void) const = 0;
    virtual void Place(const MBPoint3 &) = 0;
    virtual void Move(const MBPoint3 &) = 0;
    virtual MBRect GetBounds(void) const = 0;
    virtual bool IsVisible(void) const = 0;
    virtual long Show(void) = 0;
    virtual long Hide(void) = 0;
    virtual bool IsDirty(void) const = 0;
    virtual void SetDirty(void) = 0;
    virtual void ClearDirty(void) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) const = 0;
    virtual void GetParent(IMBObjectGroup **) const = 0;
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *) = 0;
};

class IMBObjectGroup : public IMBObject {
public:
    virtual void GetBackground(IMBObject **) const = 0;
    virtual void SetBackground(IMBObject *) = 0;
    virtual void AddObject(IMBObject *) = 0;
    virtual void RemoveObject(IMBObject *) = 0;
    virtual void ObjectMoved(IMBObject *) = 0;
};

class IMBStream : public IMBUnknown {
};

class IMBWinFile : public IMBUnknown {
public:
    virtual void *GetFileHandle(void) = 0;
};

class IMBPeriodical : public IMBUnknown {
public:
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

enum MBScreenState { kMBScreenStateDummy };
struct MBPoint {
    long x;
    long y;
};

class IMBScreen : public IMBUnknown {
public:
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual bool CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
    virtual void Flip(void) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual MBRect GetGlobalBounds(void) = 0;
    virtual MBPoint GlobalToScreenPoint(const MBPoint &) = 0;
};

struct MBScreenAttributes;

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
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

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
    MBComPtr(T *p, bool addRef) : mInterface(p) { if (addRef && p) p->AddRef(); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            if (p) p->AddRef();
            if (old) old->Release();
        }
        return *this;
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
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
private:
    long _QueryInterface(IMBUnknown *const &p); // emitted in other objects
    long _QueryInterface(IMBStream *const &p);
    T *mInterface;
};

// ---- RAD Smacker (SMACKW32.DLL) and Miles (MSS32.DLL) ----
struct SmackTag {
    unsigned long Version;
    unsigned long Width;
    unsigned long Height;
    unsigned long Frames;
    unsigned long MSPerFrame;
    unsigned long SmackerType;
    unsigned long LargestInTrack[7];
    unsigned long tablesize;
    unsigned long codesize;
    unsigned long absize;
    unsigned long detailsize;
    unsigned long typesize;
    unsigned long TrackType[7];
    unsigned long extra;
    unsigned long NewPalette;
    unsigned char Palette[772];
    unsigned long PalType;
    unsigned long FrameNum;
    unsigned long FrameSize;
    unsigned long SndSize;
    // remaining fields not used here (size 0x3b0)
};
struct _SMACKBLIT;
struct _DIG_DRIVER;
struct _MDI_DRIVER;
struct _DLSDEVICE;

extern "C" {
__declspec(dllimport) SmackTag *__stdcall SmackOpen(const char *name, unsigned long flags, unsigned long extrabuf);
__declspec(dllimport) void __stdcall SmackClose(SmackTag *smk);
__declspec(dllimport) unsigned long __stdcall SmackDoFrame(SmackTag *smk);
__declspec(dllimport) void __stdcall SmackNextFrame(SmackTag *smk);
__declspec(dllimport) unsigned long __stdcall SmackWait(SmackTag *smk);
__declspec(dllimport) void __stdcall SmackGoto(SmackTag *smk, unsigned long frame);
__declspec(dllimport) void __stdcall SmackToBuffer(SmackTag *smk, unsigned long left, unsigned long top,
                                                   unsigned long pitch, unsigned long destheight,
                                                   void *buf, unsigned long flags);
__declspec(dllimport) unsigned char __stdcall SmackSoundUseMSS(void *dd);
__declspec(dllimport) unsigned long __stdcall SmackUseMMX(unsigned long flag);
__declspec(dllimport) _SMACKBLIT *__stdcall SmackBlitOpen(unsigned long flags);
__declspec(dllimport) void __stdcall SmackBlitClose(_SMACKBLIT *sblit);
__declspec(dllimport) void __stdcall SmackBlitSetPalette(_SMACKBLIT *sblit, void *palette, unsigned long paltype);
__declspec(dllimport) void __stdcall SmackBlit(_SMACKBLIT *sblit, void *dest, unsigned long destpitch,
                                               unsigned long destx, unsigned long desty, void *src,
                                               unsigned long srcpitch, unsigned long srcx, unsigned long srcy,
                                               unsigned long srcw, unsigned long srch);
__declspec(dllimport) void __stdcall AIL_quick_handles(_DIG_DRIVER **pdig, _MDI_DRIVER **pmdi, _DLSDEVICE **pdls);
}

// ---- movie ----
class IMBMovie : public IMBUnknown {
public:
    virtual bool Load(unsigned long, IMBStream *, unsigned long) = 0;
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    virtual bool IsPlaying(void) = 0;
    virtual void Rewind(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Unpause(void) = 0;
    virtual bool IsPaused(void) = 0;
    virtual void SetLoop(bool) = 0;
    virtual bool IsLooping(void) = 0;
    virtual long CurrentFrameNum(void) const = 0;
    virtual long NumTotalFrames(void) const = 0;
    virtual long FrameDelay(void) const = 0;
    virtual long Width(void) const = 0;
    virtual long Height(void) const = 0;
    virtual long Depth(void) const = 0;
    virtual void SetTransparentColor(MBColor) = 0;
    virtual MBColor GetTransparentColor(void) = 0;
    virtual void SetPalette(MBPalette *) = 0;
    virtual MBPalette *GetPalette(void) = 0;
};

class IMBResource : public IMBUnknown {
public:
    virtual unsigned long GetID(void) = 0;
    virtual unsigned long GetType(void) = 0;
};

struct MBMovieEventStruct {
    IMBMovie *movie;
    unsigned long id;
};

class CMBSmackerMovie : public IMBMovie, public IMBResource, public IMBPeriodical, public IMBObject {
public:
    CMBSmackerMovie(void);
protected:
    ~CMBSmackerMovie(void);
    bool InitializeBuffers(void);
    void FreeBuffers(void);
    bool IsTimeForNextFrame(void);
    bool RenderCurrentFrame(void);
    bool PrepareNextFrame(void);
    unsigned long _mRefCnt;                 // +0x10
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool Load(unsigned long id, IMBStream *in, unsigned long flags);
    virtual void Play(void);
    virtual void Stop(void);
    virtual bool IsPlaying(void);
    virtual void Rewind(void);
    virtual long Pause(void);
    virtual long Unpause(void);
    virtual bool IsPaused(void);
    virtual void SetLoop(bool loop);
    virtual bool IsLooping(void);
    virtual long CurrentFrameNum(void) const;
    virtual long NumTotalFrames(void) const;
    virtual long FrameDelay(void) const;
    virtual long Width(void) const;
    virtual long Height(void) const;
    virtual long Depth(void) const;
    virtual void SetTransparentColor(MBColor color);
    virtual MBColor GetTransparentColor(void);
    virtual void SetPalette(MBPalette *pal);
    virtual MBPalette *GetPalette(void);
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
    virtual long Draw(IMBSurface *dest, MBRect &srcRect, MBRect &destRect) const;
    virtual void GetParent(IMBObjectGroup **parent) const;
protected:
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
public:
    virtual void Idle(void);
    virtual unsigned long GetID(void);
    virtual unsigned long GetType(void);
protected:
    MBComPtr<IMBStream> mStream;            // +0x14
    MBComPtr<IMBSurface> mIntermediate;     // +0x18
    bool mIsLoaded;                         // +0x1c
    bool mIsPlaying;                        // +0x1d
    bool mIsDone;                           // +0x1e
    bool mIsLooping;                        // +0x1f
    long mPauseCount;                       // +0x20
    long mDroppedFrames;                    // +0x24
    bool mUseTransparent;                   // +0x28
    SmackTag *mSmacker;                     // +0x2c
    bool mPixDouble;                        // +0x30
    _SMACKBLIT *mBlitter;                   // +0x34
    unsigned char *mBlitSource;             // +0x38
    long mBlitSourceWidth;                  // +0x3c
    long mBlitSourcePitch;                  // +0x40
    long mBlitSourceHeight;                 // +0x44
    IMBObjectGroup *mParent;                // +0x48
    MBPoint3 mPos;                          // +0x4c
    long mHideCount;                        // +0x58
    bool mDirty;                            // +0x5c
    unsigned long mID;                      // +0x60
};

IMBUnknown *MakeCMBSmackerMovie(void);

#endif
