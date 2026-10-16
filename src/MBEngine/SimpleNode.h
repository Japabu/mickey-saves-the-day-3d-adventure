// SimpleNode.h - muffin/include/Game/SimpleNode.h: the engine's base node, with the
// interface declarations its units (SimpleNode.obj, ScriptHelpNode.obj) use. Member
// names and layout from MBEngine.pdb. Also compiled into SteamboatApp.dll through
// SteamboatApp/SimpleNode.cpp and ScriptHelpNode.cpp.
#ifndef SIMPLENODE_H
#define SIMPLENODE_H

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);
extern "C" __declspec(dllimport) unsigned int __cdecl MBstrlen(const char *s);
extern "C" __declspec(dllimport) void __cdecl MBDebugStr(const char *format, ...);
extern "C" __declspec(dllimport) unsigned long __cdecl MBHashString(const char *name);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { if (p) MBfree_release(p); }

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

// ---- colours (header statics: every including unit has its own $E1..$E47) ----
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

#include "MBStl.h"
#include <string>
#include <map>

// No construct/destroy: the original calls std::_Construct out of line for the database map
template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

template <class K, class V, class Pr> class MBMultimap : public std::multimap<K, V, Pr, MBAllocator<V> > {
};

// PDB ldata order in SimpleNode.obj (0x10089680..0x10089910)
static const MBGUID MBIID_IMBUnknown = { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory = { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject = { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup = { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim = { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBAnim = { 0x7ecbd150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface = { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFont = { 0x7f4301f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFont = { 0x7f4aa5e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBUniFont = { 0x7ff13550, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSystemFont = { 0x7ff75210, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinSystemFont = { 0x80020460, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBBitmapFont = { 0x800699f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodical = { 0x79529260, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager = { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBView = { 0x796b0560, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBView = { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld = { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWorld = { 0x797ee2d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNode = { 0x795ecbe0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBNodeManager = { 0x7984ff90, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager = { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener = { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager = { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBViewManager = { 0x79b15000, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseMonitor = { 0x79c52d70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMouseMonitor = { 0x79cb4a30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameEngine = { 0x7a1c4720, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFile = { 0x896bcb10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStream = { 0x8971e7d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec = { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource = { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFile = { 0x899e3840, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResourceFileList = { 0x898a5ad0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr = { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceTree = { 0x89a45500, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceZipfile = { 0x89fe7d10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBStringTable = { 0x8f1468d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBStringTable = { 0x8f1a8590, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundDescription = { 0x84481ea0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(T *p) : mInterface(p) { if (mInterface) mInterface->AddRef(); }
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    MBComPtr(const MBComPtr &p) : mInterface(p.mInterface) { if (mInterface) mInterface->AddRef(); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
    T **operator&(void) { return &mInterface; }
    MBComPtr &operator=(T *p) {
        if (mInterface != p) {
            T *old = mInterface;
            mInterface = p;
            AddRef();
            if (old) old->Release();
        }
        return *this;
    }
    MBComPtr &operator=(const MBComPtr &p) { return *this = p.mInterface; }
    void AddRef(void) {
        if (mInterface) mInterface->AddRef();
    }
    // do/while(0) (a release macro in the original, it seems): same code as a plain if, but the
    // extra nodes change VC6's inline budget; Attach's block does the same for the Database ctor
    void Release(void) {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void Attach(T *p) {
        do {
            Release();
        } while (0);
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
    long _QueryInterface(IMBUnknown *const &p) {
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
    template <class U> long _QueryInterface(const MBComPtr<U> &p) {
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
            do {
                *this = 0;
            } while (0);
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

// ---- geometry ----
struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

struct MBPointStruct3 {
    long x;
    long y;
    long z;
};

class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(void) { x = y = z = 0; }
    MBPoint3(long ax, long ay, long az) { x = ax; y = ay; z = az; }
};

class MBSize {
public:
    MBSize(long width, long height) { w = width; h = height; }
    MBSize(void) {}
    void SetSize(long width, long height) { w = width; h = height; }
    long w;
    long h;
};

class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(MBPoint p, MBSize s) { left = p.x; top = p.y; right = p.x + s.w; bottom = p.y + s.h; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    long left;
    long top;
    long right;
    long bottom;
};

// ---- events ----
struct MBKeyboardEvent {
    unsigned short mCharCode;
    unsigned short mSpecialCode;
};

class IMBScreen;

struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

class IMBAnim;

struct MBAnimEventStruct {
    IMBAnim *anim;
    unsigned long id;
    long sequence;
    long frame;
};

class IMBSoundDescription;

struct SoundEndedEvent {
    IMBSoundDescription *mSoundDescription;
    bool mCompleted;
};

// ---- interfaces ----
class IMBSurface;
class IMBObjectGroup;
class IMBStream;
class IMBMemoryBlock;
class IMBStreamLocator;
class IMBMouseable;
class IMBNodeManager;
struct MBScreenAttributes;

class IMBSurface : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurface; }
    // (only Release is called here; the surface methods are in CMBSurfaceObject.h)
};

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
    static const MBGUID &GetGUID(void) { return MBIID_IMBObjectGroup; }
    virtual void GetBackground(IMBObject **) const = 0;
    virtual void SetBackground(IMBObject *) = 0;
    virtual void AddObject(IMBObject *) = 0;
    virtual void RemoveObject(IMBObject *) = 0;
    virtual void ObjectMoved(IMBObject *) = 0;
};

enum MBAnimationProperty { kAnimationDelay, kAnimationZPlane, kAnimationQuietBackground, kAnimationAutoUnload,
                           kAnimationDontAdjustZPlane, kAnimationExternalCanvas, kAnimationExternalSequence };
enum MBSequenceProperty { kSequenceComment, kSequenceZPlane, kSequenceDelay, kSequenceInSync, kSequenceEndAction,
                          kSequenceJumpSequence, kSequenceJumpRandomSequenceMin, kSequenceJumpRandomSequenceMax,
                          kSequenceTileWidth, kSequenceTileHeight };
enum MBFrameProperty { kFrameNotify, kFrameDelay, kFrameStartSound, kFrameStartAnim, kFrameStartAnimName,
                       kFrameStartAnimSequence, kFrameStopAnim, kFrameStartScript, kFrameStopScript,
                       kFrameCastMemberOffset };

class IMBAnim : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBAnim; }
    virtual long Load(unsigned long, IMBStream *, bool) = 0;              // +0x44
    virtual long Save(IMBStream *) = 0;
    virtual void Clone(IMBAnim **) = 0;                                   // +0x4c
    virtual long SetAnimationProperty(MBAnimationProperty, const MBString &) = 0;         // +0x50..+0x5c
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned long &) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned short) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned char) = 0;
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned short) = 0;      // +0x60..+0x64
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned char) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBPoint &) = 0;  // +0x68..+0x78
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBString &) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned long) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned short) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned char) = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned long &) const = 0;    // +0x7c..+0x84
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned short &) const = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned char &) const = 0;
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned short &) const = 0; // +0x88..+0x8c
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned char &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, MBPoint &) const = 0;  // +0x90..+0x9c
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned long &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned short &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned char &) const = 0;
    virtual long UseAnimationProperty(MBAnimationProperty, bool) = 0;                     // +0xa0
    virtual bool HasAnimationProperty(MBAnimationProperty) const = 0;
    virtual long UseSequenceProperty(long, MBSequenceProperty, bool) = 0;
    virtual bool HasSequenceProperty(long, MBSequenceProperty) const = 0;
    virtual long UseFrameProperty(long, long, MBFrameProperty, bool) = 0;
    virtual bool HasFrameProperty(long, long, MBFrameProperty) const = 0;
    virtual void SetCastID(long, long, long) = 0;                         // +0xb8
    virtual long GetCastID(long, long) const = 0;
    virtual void Play(void) = 0;                                          // +0xc0
    virtual void Stop(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Resume(void) = 0;
    virtual bool IsPaused(void) const = 0;
    virtual bool IsStalled(void) const = 0;
    virtual bool IsRendering(void) const = 0;
    virtual bool IsPlaying(void) const = 0;
    virtual void Scale(float, float, float) = 0;                          // +0xe0
    virtual unsigned long GetNumSequences(void) const = 0;
    virtual long AddSequence(void) = 0;
    virtual bool SetCurrentSequence(long) = 0;                            // +0xec
    virtual long GetCurrentSequenceIndex(void) const = 0;
    virtual unsigned long GetNumFrames(long) const = 0;
    virtual long AddFrame(long) = 0;
    virtual bool SetCurrentFrame(long) = 0;
    virtual long GetCurrentFrameIndex(void) const = 0;                    // +0x100
    virtual unsigned long GetNumCasts(void) const = 0;                    // +0x104
    virtual long AddCastMember(IMBSurface *, const MBPoint &) = 0;
    virtual void GetCastMemberSurface(long, IMBSurface **) = 0;
    virtual void SetCastMemberSurface(long, IMBSurface *) = 0;
    virtual MBPoint GetCastMemberRegistrationPoint(long) const = 0;
    virtual void SetCastMemberRegistrationPoint(long, const MBPoint &) = 0; // +0x118
    virtual MBPoint GetCastMemberAttachmentPoint(long) const = 0;
    virtual void SetCastMemberAttachmentPoint(long, const MBPoint &) = 0;
};

class IMBFont : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFont; }
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
    static const MBGUID &GetGUID(void) { return MBIID_IMBBitmapFont; }
    virtual long LoadFont(unsigned long, IMBStream *) = 0;          // +0x24
    virtual long LoadCharacterMap(unsigned long, IMBStream *) = 0;  // +0x28
};

class IMBSystemFont : public IMBFont {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSystemFont; }
    virtual long LoadFont(IMBSurface *, const char *, short, const MBColor &, unsigned long) = 0; // +0x24
};

enum IMBStreamSeekValues { kMBStreamSeekDummy };
enum IMBStreamError { kMBStreamErrorDummy };

class IMBStream : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStream; }
    virtual bool Read(void *, unsigned long &) = 0;
    virtual bool Write(const void *, unsigned long) = 0;
    virtual bool Seek(long, IMBStreamSeekValues) = 0;
    virtual unsigned long GetPosition(void) = 0;
    virtual bool Flush(void) = 0;
    virtual bool SetLastError(IMBStreamError) = 0;
    virtual IMBStreamError GetLastError(void) = 0;
    virtual unsigned long GetStreamSize(void) = 0;
    virtual void Clone(IMBStream **) = 0;
};

class IMBFileSpec : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileSpec; }
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void SetPath(const char *) = 0;                       // vftable+0x10
    virtual void SetPath(const IMBFileSpec *, const char *) = 0;  // vftable+0x0c
    virtual MBString GetName(void) const = 0;
    virtual MBString GetExtension(void) const = 0;
    virtual MBString GetPath(void) const = 0;                     // vftable+0x1c
    virtual MBString GetTitle(void) const = 0;
    virtual unsigned long GetType(void) const = 0;
    virtual void GetParent(IMBFileSpec **) const = 0;
    virtual unsigned long Hash(void) const = 0;
};

class IMBFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFile; }
    virtual bool Open(const IMBFileSpec *, unsigned long) = 0;    // vftable+0x0c
    virtual void GetFileSpec(IMBFileSpec **) = 0;
    virtual bool Close(void) = 0;
    virtual bool SetSize(unsigned long) = 0;
};

class IMBResourceFile : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFile; }
    virtual void LoadResource(unsigned long, unsigned long, IMBMemoryBlock **) = 0;
    virtual void GetStream(unsigned long, unsigned long, IMBStream **) = 0;
};

enum MBResFileListAddLoc { kMBResFileListAddFirst, kMBResFileListAddLast };

class IMBResourceFileList : public IMBResourceFile {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBResourceFileList; }
    virtual void AddFile(IMBResourceFile *, MBResFileListAddLoc, unsigned long) = 0;
    virtual void RemoveFile(IMBResourceFile *) = 0;
};

enum MBScreenState { kScreenStateActive, kScreenStatePaused, kScreenStateInactive };

class IMBScreen : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreen; }
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
    virtual void GetScreenByID(long, IMBScreen **) = 0;
    virtual void FindScreenContainingPoint(const MBPoint &, IMBScreen **) = 0;
};

class IMBWorld : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWorld; }
    virtual MBRect GetBounds(void) const = 0;
    virtual void SetBounds(const MBRect &) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0;
    virtual IMBObjectGroup &GetRoot(void) = 0;
    virtual const IMBObjectGroup &GetConstRoot(void) const = 0;
    void AddObject(IMBObject *object) { GetRoot().AddObject(object); }
    void RemoveObject(IMBObject *object) { GetRoot().RemoveObject(object); }
    void GetBackground(IMBObject **background) const { GetConstRoot().GetBackground(background); }
    void SetBackground(IMBObject *background) { GetRoot().SetBackground(background); }
};

class IMBView : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBView; }
    virtual bool IsDirty(void) = 0;
    virtual void Draw(IMBSurface *) = 0;
    virtual void SetWorldPosition(MBPoint) = 0;
    virtual MBPoint GetWorldPosition(void) = 0;
    virtual MBRect GetWorldBounds(void) = 0;
    virtual void SetScreenPosition(MBPoint) = 0;
    virtual MBPoint GetScreenPosition(void) = 0;
    virtual MBRect GetScreenBounds(void) = 0;
    virtual void SetSize(unsigned short, unsigned short) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual void AttachToWorld(IMBWorld *) = 0;
    virtual void AttachToScreen(IMBScreen *) = 0;
    virtual bool IsAttachedToScreen(IMBScreen *) = 0;
    virtual void GetWorld(IMBWorld **) = 0;
    virtual MBPoint ScreenToWorldPoint(const MBPoint &) = 0;
};

class IMBViewManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBViewManager; }
    virtual void AddView(IMBView *) = 0;
    virtual void RemoveView(IMBView *) = 0;
    virtual void FindViewContaingPoint(IMBScreen *, const MBPoint &, IMBView **) = 0;
};

class IMBMouseMonitor : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMouseMonitor; }
    virtual void AddMouseable(IMBMouseable *) = 0;
    virtual void RemoveMouseable(IMBMouseable *) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual bool IsActive(void) = 0;
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

class IMBEventListener : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventListener; }
    virtual void ListenToEvent(long channel, long event, void *data) = 0;
};

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual void RemoveListener(long, IMBEventListener *) = 0; // vftable+0x18
    virtual void RemoveListener(IMBEventListener *) = 0;       // vftable+0x14
};

class IMBSoundDescription : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSoundDescription; }
    virtual void SetStreamLocator(IMBStreamLocator *) = 0;
    virtual void GetStreamLocator(IMBStreamLocator **) = 0;
    virtual unsigned long GetSoundID(void) const = 0;
    virtual void SetChannel(long) = 0;
    virtual long GetChannel(void) const = 0;
    virtual void SetLoopCount(long) = 0;
    virtual long GetLoopCount(void) const = 0;
    virtual void SetStreaming(bool) = 0;
    virtual bool IsStreaming(void) const = 0;
    virtual void SetRateMultiplier(float) = 0;
    virtual float GetRateMultiplier(void) const = 0;
};

class MBLocale {
public:
    MBLocale(void) {}
    MBLocale(const MBString &lcv) : mLCV(lcv) {}
protected:
    MBString mLCV;
    MBString16 mDisplayName;
};

class IMBStringTable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBStringTable; }
    // overloads in reverse declaration order
    virtual bool Load(const char *) = 0;                 // vftable+0x10
    virtual bool Load(unsigned long, IMBStream *) = 0;   // vftable+0x0c
    virtual bool SetLocale(const MBLocale &) = 0;
    virtual bool GetString(unsigned long, MBString16 &) = 0;
    virtual bool GetNamedString(const MBString &, MBString16 &) = 0;
};

class IMBNode : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBNode; }
    virtual void OnStart(void) = 0;
    virtual void OnPause(void) = 0;
    virtual void OnResume(void) = 0;
    virtual void OnEnd(void) = 0;
    virtual void OnQuit(void) = 0;
};

class IMBGameEngine : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBGameEngine; }
    virtual void Init(void) = 0;
    virtual void ShutDown(void) = 0;
    virtual MBComPtr<IMBNodeManager> GetNodeMgr(void) const = 0;
    virtual long CreateScreen(IMBScreenManager *) = 0;
    virtual void Start(void) = 0;
    virtual void Idle(void) = 0;
    virtual void UpdateScreen(const MBRect &) = 0;
    virtual bool IsQuitting(void) const = 0;
    virtual void Quit(void) = 0;
    virtual void SetOnEngineStartupFunction(void (__cdecl *)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineShutdownFunction(void (__cdecl *)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineCreateScreenFunction(long (__cdecl *)(IMBGameEngine *, IMBScreenManager *)) = 0;
    virtual void GetStringTable(IMBStringTable **) = 0;
};

// engine entry points (direct calls into other units of this DLL)
extern "C" {
void MBGetScreenManager(IMBScreenManager **screenManager);
bool MBGetScreenMouseLocation(MBPoint &screenPoint, IMBScreen **screen);
bool MBShowSystemCursor(bool show);
void MBPlaySound(const char *soundName, long loopCount);
bool MBFileSpecExists(const IMBFileSpec *spec);
bool MBIsFileSpecAFolder(const IMBFileSpec *spec);
bool MBMakeCDPath(const char *partialPath, IMBFileSpec **spec);
}

class SimpleNode : public IMBNode, public IMBPeriodical, public IMBEventListener {
public:
    SimpleNode(IMBGameEngine *engine, bool enableMessageRouting);
    virtual ~SimpleNode(void);
    static void __cdecl Startup(IMBGameEngine *);
    static void __cdecl Shutdown(IMBGameEngine *);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj)
    {
        if (!obj)
            return -2;
        *obj = 0;
        IMBUnknown *p = 0;
        if (iid == MBIID_IMBUnknown)
            p = (IMBNode *)this;
        if (iid == MBIID_IMBNode)
            p = (IMBNode *)this;
        if (iid == MBIID_IMBPeriodical)
            p = (IMBPeriodical *)this;
        if (iid == MBIID_IMBEventListener)
            p = (IMBEventListener *)this;
        if (p) {
            p->AddRef();
            *obj = p;
            return 0;
        }
        return -3;
    }
    virtual unsigned long AddRef(void) { return ++_mRefCnt; }
    virtual unsigned long Release(void)
    {
        unsigned long r = --_mRefCnt;
        if (!_mRefCnt)
            delete this;
        return r;
    }
    virtual void OnStart(void);
    virtual void OnEnd(void);
    virtual void OnPause(void);
    virtual void OnResume(void);
    virtual void OnQuit(void);
    virtual void OnPreRender(MBComPtr<IMBScreen> screen);
    virtual void OnPostRender(MBComPtr<IMBScreen> screen);
    virtual void OnPostFlip(MBComPtr<IMBScreen> screen);
    virtual void OnKeyDown(MBKeyboardEvent &keyEvent);
    virtual void OnKeyUp(MBKeyboardEvent &keyEvent);
    virtual void OnLMouseDown(MBMouseEvent &evt);
    virtual void OnLMouseUp(MBMouseEvent &evt);
    virtual void OnLMouseDblClk(MBMouseEvent &evt);
    virtual void OnMMouseDown(MBMouseEvent &evt);
    virtual void OnMMouseUp(MBMouseEvent &evt);
    virtual void OnMMouseDblClk(MBMouseEvent &evt);
    virtual void OnRMouseDown(MBMouseEvent &evt);
    virtual void OnRMouseUp(MBMouseEvent &evt);
    virtual void OnRMouseDblClk(MBMouseEvent &evt);
    virtual void OnMouseMove(MBMouseEvent &evt);
    virtual void OnAnimSeqStarted(unsigned long id, unsigned long seq);
    virtual void OnAnimSeqEnded(unsigned long id, unsigned long seq);
    virtual void OnAnimNotify(unsigned long id, unsigned long seq, unsigned long frame);
    virtual void OnSoundStarted(long channel, unsigned long id);
    virtual void OnSoundEnded(long channel, unsigned long id);
    virtual void ListenToEvent(long channel, long event, void *data);
    virtual void Idle(void);
    virtual void HideCursor(void);
    virtual void ShowCursor(void);
    virtual void SetCursor(MBComPtr<IMBAnim> cursor, short hotX, short hotY);
    virtual void SetCursorLocation(MBPoint pt);
    void SetResourceDirectories(const MBString &cdPath, const MBString &hdPath);
    void AddDatabase(const char *name);
    void RemoveDatabase(const char *name);
    MBComPtr<IMBStream> GetStream(unsigned long id, unsigned long type);
    MBComPtr<IMBAnim> CreateAnim(long id);
    MBComPtr<IMBAnim> CreateAnim(const char *name);
    MBComPtr<IMBAnim> CloneAnim(const MBComPtr<IMBAnim> &src);
    MBComPtr<IMBFont> CreateFont(long id);
    MBComPtr<IMBFont> CreateFont(const char *name);
    MBComPtr<IMBFont> CreateUnicodeFont(long id);
    MBComPtr<IMBFont> CreateUnicodeFont(const char *name);
    MBComPtr<IMBFont> CreateSystemFont(const char *name, const int pointsize, const MBColor &color, const int flags);
    void StartSound(const char *name);
    virtual void LoadStringTable(const char *name);
    void GetString(unsigned long id, MBString16 &string);
    void GetNamedString(const char *name, MBString16 &string);
    void SetLocale(MBLocale &locale);
protected:
    IMBGameEngine *mEngine;
    bool mRouteMessages;
    MBComPtr<IMBWorld> mWorld;
    MBComPtr<IMBView> mView;
    MBComPtr<IMBMouseMonitor> mMouseMonitor;
    void PlaceCursor(void);
    MBComPtr<IMBAnim> mCursor;
    virtual void StartListening(void);
    virtual void StopListening(void);
    static IMBStringTable *sStringTable;
    static MBLocale sLocale;
};

#endif
