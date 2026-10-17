// CMBDragObject.h - declarations for MBEngine.dll CMBDragObject.obj
#ifndef CMBDRAGOBJECT_H
#define CMBDRAGOBJECT_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
// evidence: delete of a SnapToPoint tests the pointer before MBfree_release
inline void operator delete(void *p) { if (p) MBfree_release(p); }

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

// evidence: colour statics take $E1..$E47 before the STL numbers (ctype $E67/$E68)
#include <string>
#include <list>

#include "MBPointStruct.h"

// own copy: this unit uses the default constructor (x = y = 0), whose inline body here differs from MBPoint.h (store order / code shape the matches depend on)
class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    bool operator==(const MBPoint &p) const {
        if (x == p.x && y == p.y)
            return true;
        return false;
    }
};

#include "MBPointStruct3.h"

#include "MBPoint3.h"

// own copy: built on this unit's own MBPoint
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    bool Contains(const MBPoint &p) const {
        return left <= p.x && top <= p.y && right >= p.x && bottom >= p.y;
    }
    long left;
    long top;
    long right;
    long bottom;
};

#include "../MBCom/IMBUnknown.h"

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// PDB ldata order in CMBDragObject.obj (0x1008ae68..0x1008af08)
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBAnim =
    { 0x7ec2a630, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurface =
    { 0x7ee752b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSurfaceObject =
    { 0x7f1e5570, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBDragObject =
    { 0x7f863e30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBResource =
    { 0x89981b80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseable =
    { 0x79d166f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

template <class T> class MBAllocator : public std::allocator<T> {
public:
    typedef unsigned long SizeType;
    typedef T *PointerType;
    T *allocate(unsigned long n, const void *) { return (T *)MBmalloc_release(n * sizeof(T)); }
    char *_Charalloc(unsigned long n) { return (char *)MBmalloc_release(n); }
    void deallocate(void *p, unsigned long) { if (p) MBfree_release(p); }
    void construct(T *p, const T &v) { new ((void *)p) T(v); }
    void destroy(T *p) { p->~T(); }
};

template <class T> class MBList : public std::list<T, MBAllocator<T> > {
};

typedef std::basic_string<char, std::char_traits<char>, MBAllocator<char> > MBString;

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T> class MBComPtr {
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
class IMBStream;
class MBPalette;

#include "IMBObject.h"

#include "IMBObjectGroup.h"

#include "MBPixelFormat.h"

#include "IMBSurface.h"

#include "IMBSurfaceObject.h"

#include "MBAnimationProperty.h"
#include "MBSequenceProperty.h"
#include "MBFrameProperty.h"

#include "IMBAnim.h"

class IMBEventListener;

#include "IMBEventManager.h"

#include "IMBMouseable.h"

class IMBDragObject : public IMBMouseable {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDragObject; }
    virtual void SetBroadcastChannel(long) = 0;
    virtual long GetBroadcastChannel(void) = 0;
    virtual void SetPickUpMessage(long) = 0;
    virtual long GetPickUpMessage(void) = 0;
    virtual void SetDropMessage(long) = 0;
    virtual long GetDropMessage(void) = 0;
    virtual void SetObject(IMBObject *) = 0;
    virtual void GetObject(IMBObject **) = 0;
    virtual void AddSnapToPoint(MBPoint, long, bool) = 0;
    virtual void RemoveSnapToPoint(MBPoint) = 0;
    virtual void ClearSnapToPoints(void) = 0;
    virtual void SetBoundingBox(MBRect) = 0;
    virtual MBRect GetBoundingBox(void) = 0;
    virtual void SetZAdjustment(long) = 0;
    virtual long GetZAdjustment(void) = 0;
};

class CMBDragObject : public IMBDragObject, public IMBObjectGroup {
public:
    CMBDragObject(void);
    ~CMBDragObject(void);
protected:
    unsigned long _mRefCnt;                     // +0x08
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
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
    virtual IMBObjectGroup *SetParent(IMBObjectGroup *newParent);
    virtual void GetBackground(IMBObject **background) const;
    virtual void SetBackground(IMBObject *newBg);
    virtual void AddObject(IMBObject *obj);
    virtual void RemoveObject(IMBObject *obj);
    virtual void ObjectMoved(IMBObject *obj);
    virtual bool IsRolledOver(const MBPoint &world_mouse_pos) const;
    virtual void EnterRollover(const MBPoint &world_mouse_pos);
    virtual void WithinRollover(const MBPoint &world_mouse_pos);
    virtual void LeaveRollover(void);
    virtual bool IsClicked(const MBPoint &world_mouse_pos, short mouseButton) const;
    virtual bool IsTrackingClick(void) const;
    virtual void StartTrackingClick(const MBPoint &world_mouse_pos, short button);
    virtual void TrackClick(const MBPoint &world_mouse_pos);
    virtual void StopTrackingClick(const MBPoint &world_mouse_pos, short button);
    virtual bool IsActive(void) const;
    virtual void Activate(void);
    virtual void Deactivate(void);
    virtual void SetPixelTest(bool set);
    virtual bool GetPixelTest(void);
    virtual void SetBroadcastChannel(long channel);
    virtual long GetBroadcastChannel(void);
    virtual void SetPickUpMessage(long message);
    virtual long GetPickUpMessage(void);
    virtual void SetDropMessage(long message);
    virtual long GetDropMessage(void);
    virtual void SetObject(IMBObject *object);
    virtual void GetObject(IMBObject **object);
    virtual void AddSnapToPoint(MBPoint point, long range, bool on_release_only);
    virtual void RemoveSnapToPoint(MBPoint point);
    virtual void ClearSnapToPoints(void);
    virtual void SetBoundingBox(MBRect bb);
    virtual MBRect GetBoundingBox(void);
    virtual void SetZAdjustment(long dz);
    virtual long GetZAdjustment(void);
protected:
    void CheckForSnap(MBPoint3 &objectpos, bool release);
    long mBroadcastChannel;                     // +0x0c
    long mPickUpEvent;                          // +0x10
    long mDropEvent;                            // +0x14
    MBComPtr<IMBObject> mObject;                // +0x18
    IMBObjectGroup *mParent;                    // +0x1c
    MBPoint3 mOffset;                           // +0x20
    long mZAdjust;                              // +0x2c
    MBRect mBoundingBox;                        // +0x30
    bool mIsActive;                             // +0x40
    bool mPixelTest;                            // +0x41
    bool mIsTrackingClick;                      // +0x42
    MBPoint mDragStartMousePos;                 // +0x44
    MBPoint3 mDragStartSurfacePos;              // +0x4c
    struct SnapToPoint {
        MBPoint point;
        long range;
        bool onreleaseonly;
    };
    MBList<SnapToPoint *> mSnapToPoints;        // +0x58
    // Stand-in (see NOTES): the PDB numbering ($S69, ctype $E67/$E68) proves one
    // more numbered item than this source has; declared only, never defined.
    static long sUnknownDragObjectNumbering;
};

IMBUnknown *MakeCMBDragObject(void);

#endif
