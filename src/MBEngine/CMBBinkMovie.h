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
#include "_RTL_CRITICAL_SECTION.h"
extern "C" __declspec(dllimport) void __stdcall InitializeCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall DeleteCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall EnterCriticalSection(_RTL_CRITICAL_SECTION *);
extern "C" __declspec(dllimport) void __stdcall LeaveCriticalSection(_RTL_CRITICAL_SECTION *);

// own copy: this unit declares its kernel32 imports itself and does not open <windows.h>, which ../mbruntime/CMBCriticalSection.h includes
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
#include "../MBCom/MBGUID.h"

// PDB ldata order, CMBBinkMovie.obj 0x1008a3b8..0x1008a4c8
#include "../MBCom/IMBUnknown.h"
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


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// ---- MBColor header: masks and colour constants (internal linkage, $E1..$E47) ----
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
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

// ---- geometry ----
#include "MBPointStruct3.h"

#include "MBPoint3.h"

#include "MBPointStruct.h"
#include "../MBEngine/MBPoint.h"

#include "MBSize.h"

#include "../MBEngine/MBRect.h"

// ---- COM pointer ----
// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
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
// own copy: built on this unit's own MBColorMask
struct MBPixelFormat {
    short mBitsPerPixel;
    MBColorMask mColorMask;
};
class MBPalette;

#include "IMBStream.h"

#include "IMBWinFile.h"

#include "IMBSurface.h"

#include "MBScreenState.h"

#include "IMBScreen.h"

struct MBScreenAttributes;

#include "IMBScreenManager.h"

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);

class IMBObjectGroup;

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "IMBResource.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBEventManager.h"

#include "IMBSoundManager.h"

#include "IMBMovie.h"

#include "MBMovieEventStruct.h"

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
