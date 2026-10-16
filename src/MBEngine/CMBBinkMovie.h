// CMBBinkMovie.h - declarations for MBEngine.dll CMBBinkMovie.obj
#ifndef CMBBINKMOVIE_H
#define CMBBINKMOVIE_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) void MBDebugStr(const char *format, ...);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

// ---- Win32 (kernel32 imports) ----
struct _RTL_CRITICAL_SECTION {
    void *DebugInfo;
    long LockCount;
    long RecursionCount;
    void *OwningThread;
    void *LockSemaphore;
    unsigned long SpinCount;
};
extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall DeleteCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall EnterCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall LeaveCriticalSection(_RTL_CRITICAL_SECTION *);

class CMBCriticalSection {
public:
    CMBCriticalSection(void) { InitializeCriticalSection(&mSection); }
    ~CMBCriticalSection(void) { DeleteCriticalSection(&mSection); }
    void Enter(void) { EnterCriticalSection(&mSection); }
    void Leave(void) { LeaveCriticalSection(&mSection); }
private:
    _RTL_CRITICAL_SECTION mSection;
};

// ---- Miles (mss32) ----
struct _DIG_DRIVER;
struct _MDI_DRIVER;
struct _DLSDEVICE;
extern "C" __declspec(dllimport) void __stdcall AIL_quick_handles(_DIG_DRIVER **, _MDI_DRIVER **, _DLSDEVICE **);

// ---- Bink (binkw32) ----
struct BINKSND;
struct BINK {
    unsigned long Width;        // +0x00
    unsigned long Height;       // +0x04
    unsigned long Frames;       // +0x08
    unsigned long FrameNum;     // +0x0c
    unsigned long LastFrameNum; // +0x10
    unsigned long FrameRate;    // +0x14
    unsigned long FrameRateDiv; // +0x18
};
typedef long (__stdcall *BINKSNDOPEN)(BINKSND *, unsigned long, long);
typedef BINKSNDOPEN (__stdcall *BINKSNDSYSOPEN)(unsigned long);
extern "C" __declspec(dllimport) BINK *__stdcall BinkOpen(const char *name, unsigned long flags);
extern "C" __declspec(dllimport) char *__stdcall BinkGetError(void);
extern "C" __declspec(dllimport) void __stdcall BinkClose(BINK *bnk);
extern "C" __declspec(dllimport) long __stdcall BinkWait(BINK *bnk);
extern "C" __declspec(dllimport) void __stdcall BinkNextFrame(BINK *bnk);
extern "C" __declspec(dllimport) long __stdcall BinkDoFrame(BINK *bnk);
extern "C" __declspec(dllimport) long __stdcall BinkCopyToBuffer(BINK *bnk, void *dest, long destpitch,
                                                                 unsigned long destheight, unsigned long destx,
                                                                 unsigned long desty, unsigned long flags);
extern "C" __declspec(dllimport) void __stdcall BinkGoto(BINK *bnk, unsigned long frame, long flags);
extern "C" __declspec(dllimport) long __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN open, unsigned long param);
extern "C" __declspec(dllimport) BINKSNDOPEN __stdcall BinkOpenMiles(unsigned long param);

// ---- MB core ----
struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

// PDB ldata order, CMBBinkMovie.obj 0x1008a3b8..0x1008a4c8
static const MBGUID MBIID_IMBUnknown = {0x59d231f0, 0x9c6c, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBStream = {0x8971e7d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBWinFile = {0x89780490, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBResource = {0x89981b80, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreen = {0x79d783b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBObject = {0x7ed4fc70, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBObjectGroup = {0x7ed99200, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSurface = {0x7ee752b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBMemorySurface = {0x7f30abb0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBMovie = {0x7f5cfc20, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBBinkMovie = {0x7fd5b3f0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBPeriodical = {0x79529260, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBPeriodicalManager = {0x7958af20, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreenManager = {0x79df27a0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSoundManager = {0x79ee6f80, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSoundDescription = {0x84481ea0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// ---- MBColor header: masks and colour constants (internal linkage, $E1..$E47) ----
class MBColorMask {
public:
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

struct MBPointStruct { long x; long y; };
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
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
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    long left;
    long top;
    long right;
    long bottom;
};

// ---- COM pointer ----
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
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
    long CreateObject(const MBGUID &clsid) {
        Release();
        IMBUnknown *unk;
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            long hr = _QueryInterface(unk);
            unk->Release();
            return hr;
        }
        return -2;
    }
    void Attach(T *p) {
        Release();
        mInterface = p;
    }
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
private:
    template <class U> long _QueryInterface(U *const &p) {
        long hr;
        if (p) {
            T *pInterface;
            hr = p->QueryInterface(T::GetGUID(), (void **)&pInterface);
            if (hr) {
                Attach(0);
                return hr;
            }
            Attach(pInterface);
        } else {
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

// ---- interfaces ----
struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};
class MBPalette;

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStream; }
};

class IMBWinFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWinFile; }
    virtual void *GetFileHandle(void) = 0;
};

class IMBSurface : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurface; }
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

enum MBScreenState {
    kScreenStateActive = 0,
    kScreenStatePaused = 1,
    kScreenStateInactive = 2
};

class IMBScreen : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreen; }
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual bool CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
};

struct MBScreenAttributes;

class IMBScreenManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreenManager; }
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
};

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

class IMBObjectGroup;

class IMBObject : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBObject; }
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

class IMBResource : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResource; }
    virtual unsigned long GetID(void) = 0;
    virtual unsigned long GetType(void) = 0;
};

class IMBPeriodical : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodical; }
    virtual void Idle(void) = 0;
};

class IMBPeriodicalManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPeriodicalManager; }
    virtual void AddPeriodical(IMBPeriodical *) = 0;
    virtual void RemovePeriodical(IMBPeriodical *) = 0;
    virtual void DevoteTimeToPeriodicals(void) = 0;
};

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
};

class IMBSoundManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundManager; }
};

class IMBMovie : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMovie; }
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

struct MBMovieEventStruct {
    IMBMovie *movie;
    unsigned long id;
};

class CMBBinkMovie : public IMBMovie, public IMBResource, public IMBPeriodical, public IMBObject {
public:
    CMBBinkMovie(void);
protected:
    ~CMBBinkMovie(void);
    bool InitializeBuffers(void);
    void FreeBuffers(void);
    bool IsTimeForNextFrame(void);
    bool RenderCurrentFrame(void);
    bool PrepareNextFrame(void);
    unsigned long _mRefCnt;
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
    MBComPtr<IMBStream> mStream;                 // +0x14
    bool mIsLoaded;                              // +0x18
    bool mIsPlaying;                             // +0x19
    bool mIsDone;                                // +0x1a
    bool mIsLooping;                             // +0x1b
    long mPauseCount;                            // +0x1c
    long mDroppedFrames;                         // +0x20
    bool mUseTransparent;                        // +0x24
    unsigned long mID;                           // +0x28
    IMBObjectGroup *mParent;                     // +0x2c
    MBPoint3 mPos;                               // +0x30
    long mHideCount;                             // +0x3c
    bool mDirty;                                 // +0x40
    MBComPtr<IMBSurface> mIntermediateSurface;   // +0x44
    BINK *mBinkHandle;                           // +0x48
    unsigned long mFlags;                        // +0x4c
    unsigned long mBinkFrameDelay;               // +0x50
    MBComPtr<IMBSoundManager> mSoundManager;     // +0x54
};

IMBUnknown *MakeCMBBinkMovie(void);

#endif
