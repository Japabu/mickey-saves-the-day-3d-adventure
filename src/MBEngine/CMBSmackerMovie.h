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
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

#include "../MBEngine/MBColor.h"

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

#include "MBPixelFormat.h"

// ---- geometry ----
#include "MBPointStruct3.h"

#include "MBPoint3.h"

#include "MBSize.h"

// own copy: built on this unit's own MBPoint
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

#include "IMBSurface.h"

class IMBObjectGroup;

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "IMBStream.h"

#include "IMBWinFile.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

class IMBEventListener;

#include "IMBEventManager.h"

#include "MBScreenState.h"
// own copy: a plain struct without constructors
struct MBPoint {
    long x;
    long y;
};

#include "IMBScreen.h"

struct MBScreenAttributes;

#include "IMBScreenManager.h"

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

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
#include "IMBMovie.h"

#include "IMBResource.h"

#include "MBMovieEventStruct.h"

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
