// GameApp.h - declarations for Steamboat.exe GameApp.obj: the game shell (WinMain,
// window procedure) that loads MBEngine.dll and the game DLL through MBCom.
#ifndef GAMEAPP_H
#define GAMEAPP_H
#pragma warning(disable: 4786)
#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);
extern "C" __declspec(dllimport) void __cdecl MBmemclr(void *p, unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBDebugStr(const char *fmt, ...);

void *__cdecl operator new(unsigned int size);
void __cdecl operator delete(void *ptr);

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// Colour constants of the MBEngine colour header: internal-linkage objects, 16
// dynamic initializers per unit ($E1..$E47).
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

// The STL is used without exceptions (see MBEngine/MBStl.h).
#include "../MBEngine/MBStl.h"
#include <string>

template <class T> class MBAllocator : public std::allocator<T> {
public:
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;
typedef std::basic_string<unsigned short, std::char_traits<unsigned short>, MBAllocator<unsigned short> > MBString16;

static const MBGUID MBIID_IMBUnknown = {0x59d231f0, 0x9c6c, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBGameClock = {0x04e3c980, 0x9ea1, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBNode = {0x795ecbe0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBNodeManager = {0x7984ff90, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreenManager = {0x79df27a0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBMilesSoundManager = {0x79f48c40, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBPref = {0x79fc3030, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBPref = {0x7a00c5c0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBGameEngine = {0x7a1c4720, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBScreen = {0x79d783b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBSurface = {0x7ee752b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBDirectDrawScreenManager = {0x7fa4cdf0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectXScreen = {0x7fe4fbd0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBWinSoundManager = {0x8455df50, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBStream = {0x8971e7d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBFileSpec = {0x897e2150, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBFPSMonitor = {0x8f082f50, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_CMBFPSMonitor = {0x8f0e4c10, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

class MBRect;
class IMBSurface;
class IMBStringTable;
class IMBNodeManager;
class IMBEventListener;
template <class T> class MBComPtr;

class IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBUnknown; }
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBFileSpec : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFileSpec; }
    virtual void SetPath(const char *) = 0;
    virtual void SetPath(const IMBFileSpec *, const char *) = 0;
    virtual MBString GetName(void) const = 0;
    virtual MBString GetExtension(void) const = 0;
    virtual MBString GetPath(void) const = 0;
    virtual MBString GetTitle(void) const = 0;
    virtual unsigned long GetType(void) const = 0;
    virtual void GetParent(IMBFileSpec **) const = 0;
    virtual unsigned long Hash(void) const = 0;
};

enum IMBStreamSeekValues {
    kIMBStreamSeekStart = 1,
    kIMBStreamSeekEnd = 2,
    kIMBStreamSeekCurrent = 3
};

enum IMBStreamError {
    kIMBStreamNoError = 0,
    kIMBStreamReadError = 1,
    kIMBStreamWriteError = 2,
    kIMBStreamOpenError = 3,
    kIMBStreamCloseError = 4,
    kIMBStreamSeekError = 5,
    kIMBStreamEOF = 6
};

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

// Overloaded virtuals are laid out in reverse declaration order (MBEngine NOTES).
class IMBPref : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBPref; }
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString16 &, const MBString16 &) = 0;
    virtual long GetString(IMBStream *, const MBString &, const MBString &, MBString &, const MBString &) = 0;
    virtual long GetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString16 &) = 0;
    virtual long SetString(IMBStream *, const MBString &, const MBString &, MBString &) = 0;
    virtual long SetLong(IMBStream *, const MBString &, const MBString &, long) = 0;
};

struct MBScreenAttributes {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

enum MBScreenState {
    kScreenStateActive = 0,
    kScreenStatePaused = 1,
    kScreenStateInactive = 2
};

class IMBScreen;

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

struct HMONITOR__;
typedef BOOL (WINAPI *MBDDEnumCallback)(GUID *, char *, char *, void *, HMONITOR__ *);

class IMBDirectDrawScreenManager : public IMBScreenManager {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBScreenManager; }
    virtual bool EnableDirect3D(bool) = 0;
    virtual void EnterWindowedMode(void) = 0;
    virtual void EnterFullScreenMode(void) = 0;
    virtual void SetHWND(HWND) = 0;
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback) = 0;
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
    virtual void SetOnEngineStartupFunction(void (*)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineShutdownFunction(void (*)(IMBGameEngine *)) = 0;
    virtual void SetOnEngineCreateScreenFunction(long (*)(IMBGameEngine *, IMBScreenManager *)) = 0;
    virtual void GetStringTable(IMBStringTable **) = 0;
};

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

class IMBFPSMonitor : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBFPSMonitor; }
    virtual void SetEventLoopTime(unsigned long) = 0;
    virtual void SetDrawLoopTime(unsigned long) = 0;
    virtual unsigned long GetEventLoopFPS(void) const = 0;
    virtual unsigned long GetDrawLoopFPS(void) const = 0;
    virtual void Enable(bool) = 0;
};

class IMBWinSoundManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBWinSoundManager; }
    virtual HWND GetHWND(void) = 0;
    virtual void SetHWND(HWND) = 0;
};

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

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

class IMBWinScreen : public IMBScreen {
public:
    virtual void SetHWND(HWND) = 0;
    virtual HWND GetHWND(void) = 0;
    virtual bool EnterWindowedMode(unsigned short, unsigned short) = 0;
    virtual bool EnterFullscreenMode(unsigned short, unsigned short, unsigned short) = 0;
};

struct IDirectDraw7;
struct IDirectDrawSurface7;

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
    virtual bool IsFullscreen(void) = 0;
    virtual void SetDirectDrawEnumerator(MBDDEnumCallback) = 0;
    virtual IDirectDraw7 *GetDirectDraw7(void) = 0;
    virtual GUID *GetDeviceGUID(void) = 0;
    virtual IDirectDrawSurface7 *GetBackBuffer(void) = 0;
    virtual IDirectDrawSurface7 *GetFrontBuffer(void) = 0;
    virtual bool IsInGDIMode(void) = 0;
    virtual void GDIModeUpdateRect(RECT &) = 0;
};

// MBCom (through import thunks) and MBEngine exports
extern "C" long __cdecl MBComInitialize(void);
extern "C" long __cdecl MBComUninitialize(void);
extern "C" long __cdecl MBComLoadServer(const char *server);
extern "C" long __cdecl MBComCreateInstance(IMBUnknown *outer, const MBGUID &clsid, IMBUnknown **unknown);
extern "C" void __cdecl MBOpenFile(const IMBFileSpec *fileSpec, unsigned long openFlags, IMBStream **stream);
extern "C" void __cdecl MBRegisterScreenManager(IMBScreenManager *screenManager);
extern "C" void __cdecl MBGetScreenManager(IMBScreenManager **screenManager);
extern "C" bool __cdecl MBTransformGlobalToScreenPoint(const MBPoint &globalPoint, MBPoint &screenPoint, IMBScreen **screen);
extern "C" bool __cdecl MBShowSystemCursor(bool show);

// HCWinBlackBox.obj
long __stdcall BlackBoxExceptionFilter(const char *gameDLLName, _EXCEPTION_POINTERS *info);

typedef long (*MBCreateScreenFunc)(IMBGameEngine *, IMBScreenManager *);
typedef void (*MBEngineFunc)(IMBGameEngine *);

// muffin/include/Com/MBComPtr.h, the members Steamboat.pdb lists (the same class as
// src/SteamboatApp/MBComPtr.h); see the comments there.
template <class T> class MBComPtr {
public:
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(T *p, bool addRef) : mInterface(p) { if (addRef && mInterface) mInterface->AddRef(); }
    MBComPtr(T *p) : mInterface(p) { if (mInterface) mInterface->AddRef(); }
    MBComPtr(int) : mInterface(0) {}
    MBComPtr() : mInterface(0) {}
    template <class U> MBComPtr(const MBComPtr<U> &p) : mInterface(0) { _QueryInterface(p); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    MBComPtr(const MBComPtr &ptr) : mInterface(ptr.mInterface) { if (mInterface) mInterface->AddRef(); }
    MBComPtr<T> &operator=(int) { return *this = (T *)0; }
    // Assigning another interface queries for this one (CMBCursor.obj calls _QueryInterface on the
    // static pointers directly). VC6 takes the MBComPtr<U> template only in front of the copy
    // assignment (C2535 after it).
    template <class U> MBComPtr<T> &operator=(const MBComPtr<U> &p)
    {
        _QueryInterface(p);
        return *this;
    }
    MBComPtr<T> &operator=(const MBComPtr<T> &ptr) { return *this = ptr.mInterface; }
    template <class U> MBComPtr<T> &operator=(U *const &p)
    {
        _QueryInterface(p);
        return *this;
    }
    // AddRef() is the member, not pInterface->AddRef(): SBMovieNode::ListenToEvent calls the
    // out-of-line MBComPtr<IMBMovie>::AddRef from its inlined assignment (and DrawCursor matches).
    MBComPtr<T> &operator=(T *pInterface)
    {
        if (mInterface != pInterface) {
            T *old = mInterface;
            mInterface = pInterface;
            AddRef();
            if (old)
                old->Release();
        }
        return *this;
    }
    ~MBComPtr() { Release(); }
    void Attach(T *p, bool addRef)
    {
        Release();
        mInterface = p;
        if (addRef && mInterface)
            mInterface->AddRef();
    }
    void Attach(T *p)
    {
        Release();
        mInterface = p;
    }
    T *Detach()
    {
        T *p = mInterface;
        mInterface = 0;
        return p;
    }
    operator T *() const { return mInterface; }
    operator T &() const { return *mInterface; }
    T &operator*() const { return *mInterface; }
    T *operator->() const { return mInterface; }
    operator bool() const { return mInterface != 0; }
    bool operator==(int) { return mInterface == 0; }
    bool operator==(MBComPtr<T> &p) { return mInterface == p.mInterface; }
    bool operator==(T *p) { return mInterface == p; }
    bool operator!=(int) { return mInterface != 0; }
    bool operator!=(MBComPtr<T> &p) { return mInterface != p.mInterface; }
    bool operator!=(T *p) { return mInterface != p; }
    // The do/while(0) block (a release macro in the original, it seems) compiles to the same code
    // as a plain if, but its extra nodes change VC6's inlining budget: with it the callers inline
    // CreateObject, fold Release and call _QueryInterface out of line as the originals do.
    void Release()
    {
        do {
            if (mInterface != 0)
                mInterface->Release();
        } while (0);
        mInterface = 0;
    }
    void AddRef()
    {
        if (mInterface)
            mInterface->AddRef();
    }
    T *GetInterfacePtr() const { return mInterface; }
    // WinMain (an SEH frame keeps every local in memory) shows the shape: result is
    // set to 0 before Release and to -2 when no object was made. The name counts: VC6
    // orders the frame slots of result and unk by the symbol hash (as "hr" they swap).
    long CreateObject(const MBGUID &clsid)
    {
        long result = 0;
        Release();
        IMBUnknown *unk;
        MBComCreateInstance(0, clsid, &unk);
        if (unk) {
            result = _QueryInterface(unk);
            unk->Release();
        } else {
            result = -2;
        }
        return result;
    }
private:
    template <class U> long _QueryInterface(U *const &p)
    {
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
    // Same shape as the raw-pointer form (the out-of-line copy in CMBCursor.obj).
    template <class U> long _QueryInterface(const MBComPtr<U> &p)
    {
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

struct MBKeyboardEvent {
    unsigned short mCharCode;
    unsigned short mSpecialCode;
};

struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

enum MBSpecialKeyEnum {
    kSpecialKeyF1 = 0,
    kSpecialKeyF2 = 1,
    kSpecialKeyF3 = 2,
    kSpecialKeyF4 = 3,
    kSpecialKeyF5 = 4,
    kSpecialKeyF6 = 5,
    kSpecialKeyF7 = 6,
    kSpecialKeyF8 = 7,
    kSpecialKeyF9 = 8,
    kSpecialKeyF10 = 9,
    kSpecialKeyF11 = 10,
    kSpecialKeyF12 = 11,
    kSpecialKeyHome = 12,
    kSpecialKeyEnd = 13,
    kSpecialKeyDelete = 14,
    kSpecialKeyInsert = 15,
    kSpecialKeyPageUp = 16,
    kSpecialKeyPageDown = 17,
    kSpecialKeyLeftCursor = 18,
    kSpecialKeyRightCursor = 19,
    kSpecialKeyUpCursor = 20,
    kSpecialKeyDownCursor = 21,
    kSpecialKeyControl = 22,
    kSpecialKeyAlt = 23,
    kSpecialKeyShift = 24,
    kSpecialKeyWindows = 25
};

enum {
    kApplicationActivate = 0,
    kApplicationDeactivate = 1
};

enum MBInputMessageEnum {
    kKeyDownEvent = 0,
    kKeyUpEvent = 1,
    kSpecialKeyDown = 2,
    kSpecialKeyUp = 3,
    kInputCharacter = 4,
    kMouseDownEvent = 5,
    kMouseUpEvent = 6,
    kMouseDblClkEvent = 7,
    kMouseMoveEvent = 8
};

#endif
