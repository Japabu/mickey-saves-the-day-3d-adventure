// CMBMouseMonitor.h - declarations for MBEngine.dll CMBMouseMonitor.obj
#ifndef CMBMOUSEMONITOR_H
#define CMBMOUSEMONITOR_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

#include "../MBCom/MBGUID.h"

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
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

// evidence: colour statics take $E1..$E47 before the STL numbers (ctype $E66/$E67)
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include "MBPointStruct.h"

// own copy: this unit uses the default constructor (x = y = 0), whose inline body here differs from MBPoint.h (store order / code shape the matches depend on)
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
};

#include "MBPointStruct3.h"

#include "MBPoint3.h"

// own copy: built on this unit's own MBPoint
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    long left;
    long top;
    long right;
    long bottom;
};

#include "../MBCom/IMBUnknown.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// PDB ldata order in CMBMouseMonitor.obj (0x1008f418..0x1008f4e8)
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBView =
    { 0x7972a950, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld =
    { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventListener =
    { 0x799755d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager =
    { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseMonitor =
    { 0x79c52d70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseable =
    { 0x79d166f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreenManager =
    { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBScreen =
    { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// allocator without construct/destroy: insert calls the out-of-line std::_Construct
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

class IMBMouseable;

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    MBComPtr(T *p) : mInterface(p) { if (p) p->AddRef(); }
    MBComPtr(T *p, bool addRef) : mInterface(p) { if (addRef && p) p->AddRef(); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
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
    // VC6 calls an IMBUnknown * argument ambiguous against a member template, so
    // the one pointer source this unit converts from has its own overload
    long _QueryInterface(IMBMouseable *const &p) {
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
            *this = 0;
            hr = -3;
        }
        return hr;
    }
    T *mInterface;
};

class IMBSurface;
class IMBObjectGroup;

#include "IMBObject.h"

class IMBScreen;
#include "IMBWorld.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

#include "IMBMouseable.h"

#include "IMBMouseMonitor.h"

// own copy: built on this unit's own MBPoint
struct MBMouseEvent {
    MBPoint mMousePos;
    IMBScreen *mScreen;
    short mButton;
};

extern "C" bool MBGetWorldMouseLocation(MBPoint &worldPoint, IMBWorld **world);

class CMBMouseMonitor : public IMBMouseMonitor, public IMBEventListener {
public:
    CMBMouseMonitor(void);
    ~CMBMouseMonitor(void);
protected:
    unsigned long _mRefCnt;                 // +0x08
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual void AddMouseable(IMBMouseable *mouseable);
    virtual void RemoveMouseable(IMBMouseable *mouseable);
    virtual void Activate(void);
    virtual void Deactivate(void);
    virtual bool IsActive(void);
    virtual void ListenToEvent(long channel, long event, void *data);
protected:
    void HandleMouseDown(const MBPoint &screenPoint, IMBScreen *screen, short button);
    void HandleMouseUp(const MBPoint &globalPoint, short button);
    void CheckRollovers(void);
    void CheckClicks(void);
    void SetCurrentRollover(IMBMouseable *mouseable, const MBPoint &worldPoint);
    void ClearCurrentRollover(void);
    typedef MBVector<IMBMouseable *> MBMouseableList;
    MBMouseableList mMouseables;            // +0x0c
    IMBMouseable *mCurrentRollover;         // +0x1c
    IMBMouseable *mCurrentClick;            // +0x20
    bool mIsActive;                         // +0x24
    bool mDoZTest;                          // +0x25
};

IMBUnknown *MakeCMBMouseMonitor(void);
bool CompareMouseablesGreaterThan(IMBMouseable *arg1, IMBMouseable *arg2);

#endif
