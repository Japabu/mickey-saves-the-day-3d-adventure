// CMBFPSMonitor.h - declarations for MBEngine.dll CMBFPSMonitor.obj
#ifndef CMBFPSMONITOR_H
#define CMBFPSMONITOR_H

// colour statics come first: their $E1..$E47 precede every other number in the TU
// (MBColorMask/MBColor/k* copied from MBScreenUtils.h)
// own copy: this unit uses the constructors or GetShiftCount, whose inline body here differs from MBColorMask.h (store order / code shape the matches depend on)
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

static MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
static MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
static MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
static MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
static MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { Set(r, g, b, a); }
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
            mColor = ((unsigned long)mRed << mask.mRedShift) & mask.mRedMask;
        else
            mColor = ((unsigned long)mRed >> -mask.mRedShift) & mask.mRedMask;
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

static MBColor kWhite(255, 255, 255, 0);
static MBColor kBlack(0, 0, 0, 0);
static MBColor kRed(255, 0, 0, 0);
static MBColor kGreen(0, 255, 0, 0);
static MBColor kBlue(0, 0, 255, 0);
static MBColor kYellow(255, 255, 0, 0);
static MBColor kCyan(0, 255, 255, 0);
static MBColor kMagenta(255, 0, 255, 0);
static MBColor kGrey25(64, 64, 64, 0);
static MBColor kGrey50(128, 128, 128, 0);
static MBColor kGrey75(192, 192, 192, 0);

#include <string> // evidence: original TU has $E68/$E69 for std::ctype<unsigned short>::id
#include <stdio.h>

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);
extern "C" __declspec(dllimport) unsigned int MBstrlen(const char *);

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

static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFPSMonitor =
    { 0x8f082f50, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener =
    { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFont =
    { 0x7f4aa5e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBBitmapFont =
    { 0x800699f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr =
    { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" IMBUnknown *MBComUtilAddSingleton(IMBUnknown *unk);
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBStream.h"
class IMBMemoryBlock;
struct MBRect;
struct MBSize;

// own copy: carries the sUnknownHeaderStatic $E stand-in (see NOTES.md)
class IMBSurface : public IMBUnknown {
public:
    // placeholder: PDB has the Make guard at $S63, one number past colours (48) + <string>
    // baseline (14); one static data member declaration in the original interface headers
    // takes it (name unknown; never defined or used)
    static const unsigned long sUnknownHeaderStatic;
};

static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBScreen.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

// own copy: without GetGUID: this unit's MBComPtr<IMBResourceFileList> queries with the inherited IMBUnknown::GetGUID (MBIID_IMBUnknown), as the original does
class IMBResourceFileList : public IMBUnknown {
public:
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

// own copy: without GetGUID: this unit's MBComPtr<IMBFont> queries with the inherited IMBUnknown::GetGUID (MBIID_IMBUnknown), as the original does
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

// own copy: derives from this unit's own IMBFont
class IMBBitmapFont : public IMBFont {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBBitmapFont; }
    virtual long LoadFont(unsigned long, IMBStream *) = 0;
    virtual long LoadCharacterMap(unsigned long, IMBStream *) = 0;
};

#include "IMBFPSMonitor.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
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
    long _QueryInterface(IMBUnknown *const &p); // emitted in another object
    template <class U> long _QueryInterface(const MBComPtr<U> &p) {
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
        *this = 0;
        return hr;
    }
    T *mInterface;
};

class CMBFPSMonitor : public IMBFPSMonitor, public IMBEventListener {
public:
    CMBFPSMonitor(void);
    ~CMBFPSMonitor(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void SetEventLoopTime(unsigned long time);
    virtual void SetDrawLoopTime(unsigned long time);
    virtual unsigned long GetEventLoopFPS(void) const;
    virtual unsigned long GetDrawLoopFPS(void) const;
    virtual void Enable(bool isEnabled);
    virtual void ListenToEvent(long channel, long event, void *data);
protected:
    void MakeFont(void);
    MBComPtr<IMBFont> mFont;
    unsigned long mEventLoopHistory[3];
    unsigned long mEventLoopSlot;
    unsigned long mEventLoopFPS;
    unsigned long mDrawLoopHistory[3];
    unsigned long mDrawLoopSlot;
    unsigned long mDrawLoopFPS;
    bool mIsEnabled;
};

IMBUnknown *MakeCMBFPSMonitor(void);

#endif
