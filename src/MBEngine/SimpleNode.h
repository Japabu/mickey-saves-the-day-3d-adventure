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

#include "../MBCom/MBGUID.h"

// ---- colours (header statics: every including unit has its own $E1..$E47) ----
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
#include "../MBCom/IMBUnknown.h"
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


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
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
#include "MBPointStruct.h"

// own copy: this unit uses the default constructor (x = y = 0), whose inline body here differs from MBPoint.h (store order / code shape the matches depend on)
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

#include "MBPointStruct3.h"

#include "MBPoint3.h"

// own copy: members w/h, a default constructor that leaves them unset, SetSize
class MBSize {
public:
    MBSize(long width, long height) { w = width; h = height; }
    MBSize(void) {}
    void SetSize(long width, long height) { w = width; h = height; }
    long w;
    long h;
};

// own copy: takes MBPoint and MBSize by value and uses its own MBSize
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
#include "MBKeyboardEvent.h"

class IMBScreen;

// own copy: built on this unit's own MBPoint
struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

class IMBAnim;

#include "MBAnimEventStruct.h"

class IMBSoundDescription;

#include "SoundEndedEvent.h"

// ---- interfaces ----
class IMBSurface;
class IMBObjectGroup;
class IMBStream;
class IMBMemoryBlock;
class IMBStreamLocator;
class IMBMouseable;
class IMBNodeManager;
struct MBScreenAttributes;

#include "IMBSurface.h"

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "MBAnimationProperty.h"
#include "MBSequenceProperty.h"
#include "MBFrameProperty.h"

#include "IMBAnim.h"

#include "IMBFont.h"

#include "IMBBitmapFont.h"

#include "IMBSystemFont.h"

#include "IMBStreamSeekValues.h"
#include "IMBStreamError.h"

#include "IMBStream.h"

#include "IMBFileSpec.h"

#include "IMBFile.h"

#include "IMBResourceFile.h"

#include "MBResFileListAddLoc.h"

#include "IMBResourceFileList.h"

#include "MBScreenState.h"

#include "IMBScreen.h"

#include "IMBScreenManager.h"

#include "IMBWorld.h"

#include "IMBView.h"

#include "IMBViewManager.h"

#include "IMBMouseMonitor.h"

#include "IMBPeriodical.h"

#include "IMBPeriodicalManager.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

#include "IMBSoundDescription.h"

#include "../MBEngine/MBLocale.h"

#include "IMBStringTable.h"

#include "IMBNode.h"

#include "IMBGameEngine.h"

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
