// CMBDragObject.h - declarations for MBEngine.dll CMBDragObject.obj
#ifndef CMBDRAGOBJECT_H
#define CMBDRAGOBJECT_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(size_t size) { return MBmalloc_release(size); }
// evidence: delete of a SnapToPoint tests the pointer before MBfree_release
inline void operator delete(void *p) { if (p) MBfree_release(p); }

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

// Color constants: internal-linkage objects defined in the colour header; each
// TU that includes it gets 16 dynamic initializers ($E1..$E47, PDB ldata).
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
    bool operator!=(const MBColor &c) const {
        return mRed != c.mRed || mGreen != c.mGreen || mBlue != c.mBlue || mAlpha != c.mAlpha;
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

// evidence: colour statics take $E1..$E47 before the STL numbers (ctype $E67/$E68)
#include <string>
#include <list>

struct MBPointStruct {
    long x;
    long y;
};

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

struct MBPointStruct3 {
    long x;
    long y;
    long z;
};

class MBPoint3 : public MBPointStruct3 {
public:
    MBPoint3(long ax, long ay, long az) { x = ax; y = ay; z = az; }
    MBPoint3(void) { x = y = z = 0; }
    MBPoint3 operator+(const MBPoint3 &p) const { return MBPoint3(x + p.x, y + p.y, z + p.z); }
    MBPoint3 &operator+=(const MBPoint3 &p) { x += p.x; y += p.y; z += p.z; return *this; }
};

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

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

// PDB ldata order in CMBDragObject.obj (0x1008ae68..0x1008af08)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
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

enum MBPixelFormat { kMBPixelFormatDummy };

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

class IMBSurfaceObject : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBSurfaceObject; }
    virtual void GetSurface(IMBSurface **) = 0;
    virtual void SetSurface(IMBSurface *) = 0;
};

enum MBAnimationProperty { kMBAnimationPropertyDummy };
enum MBSequenceProperty { kMBSequencePropertyDummy };
enum MBFrameProperty { kMBFramePropertyDummy };

class IMBAnim : public IMBObject {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBAnim; }
    virtual long Load(unsigned long, IMBStream *, bool) = 0;
    virtual long Save(IMBStream *) = 0;
    virtual void Clone(IMBAnim **) = 0;
    // VC6 lays out overloaded virtuals in reverse declaration order
    virtual long SetAnimationProperty(MBAnimationProperty, const MBString &) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned long &) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned short) = 0;
    virtual long SetAnimationProperty(MBAnimationProperty, unsigned char) = 0;
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned short) = 0;
    virtual long SetSequenceProperty(long, MBSequenceProperty, unsigned char) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBPoint &) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, const MBString &) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned long) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned short) = 0;
    virtual long SetFrameProperty(long, long, MBFrameProperty, unsigned char) = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned long &) const = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned short &) const = 0;
    virtual long GetAnimationProperty(MBAnimationProperty, unsigned char &) const = 0;
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned short &) const = 0;
    virtual long GetSequenceProperty(long, MBSequenceProperty, unsigned char &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, MBPoint &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned long &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned short &) const = 0;
    virtual long GetFrameProperty(long, long, MBFrameProperty, unsigned char &) const = 0;
    virtual long UseAnimationProperty(MBAnimationProperty, bool) = 0;
    virtual bool HasAnimationProperty(MBAnimationProperty) const = 0;
    virtual long UseSequenceProperty(long, MBSequenceProperty, bool) = 0;
    virtual bool HasSequenceProperty(long, MBSequenceProperty) const = 0;
    virtual long UseFrameProperty(long, long, MBFrameProperty, bool) = 0;
    virtual bool HasFrameProperty(long, long, MBFrameProperty) const = 0;
    virtual void SetCastID(long, long, long) = 0;
    virtual long GetCastID(long, long) const = 0;
    virtual void Play(void) = 0;
    virtual void Stop(void) = 0;
    virtual long Pause(void) = 0;
    virtual long Resume(void) = 0;
    virtual bool IsPaused(void) const = 0;
    virtual bool IsStalled(void) const = 0;
    virtual bool IsRendering(void) const = 0;
    virtual bool IsPlaying(void) const = 0;
    virtual void Scale(float, float, float) = 0;
    virtual unsigned long GetNumSequences(void) const = 0;
    virtual long AddSequence(void) = 0;
    virtual bool SetCurrentSequence(long) = 0;
    virtual long GetCurrentSequenceIndex(void) const = 0;
    virtual unsigned long GetNumFrames(long) const = 0;
    virtual long AddFrame(long) = 0;
    virtual bool SetCurrentFrame(long) = 0;
    virtual long GetCurrentFrameIndex(void) const = 0;
    virtual unsigned long GetNumCasts(void) const = 0;
    virtual long AddCastMember(IMBSurface *, const MBPoint &) = 0;
    virtual void GetCastMemberSurface(long, IMBSurface **) = 0;
    virtual void SetCastMemberSurface(long, IMBSurface *) = 0;
    virtual MBPoint GetCastMemberRegistrationPoint(long) const = 0;
    virtual void SetCastMemberRegistrationPoint(long, const MBPoint &) = 0;
    virtual MBPoint GetCastMemberAttachmentPoint(long) const = 0;
    virtual void SetCastMemberAttachmentPoint(long, const MBPoint &) = 0;
};

class IMBEventListener;

class IMBEventManager : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBEventManager; }
    virtual void BroadcastEvent(long, long, void *) = 0;
    virtual void AddListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(long, IMBEventListener *) = 0;
    virtual void RemoveListener(IMBEventListener *) = 0;
};

class IMBMouseable : public IMBUnknown {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBMouseable; }
    virtual bool IsRolledOver(const MBPoint &) const = 0;
    virtual void EnterRollover(const MBPoint &) = 0;
    virtual void WithinRollover(const MBPoint &) = 0;
    virtual void LeaveRollover(void) = 0;
    virtual bool IsClicked(const MBPoint &, short) const = 0;
    virtual bool IsTrackingClick(void) const = 0;
    virtual void StartTrackingClick(const MBPoint &, short) = 0;
    virtual void TrackClick(const MBPoint &) = 0;
    virtual void StopTrackingClick(const MBPoint &, short) = 0;
    virtual bool IsActive(void) const = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual void SetPixelTest(bool) = 0;
    virtual bool GetPixelTest(void) = 0;
};

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
