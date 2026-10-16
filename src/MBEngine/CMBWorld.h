// CMBWorld.h - declarations for MBEngine.dll CMBWorld.obj
#ifndef CMBWORLD_H
#define CMBWORLD_H

typedef unsigned int size_t;

extern "C" __declspec(dllimport) void *MBmalloc_release(size_t size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

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

// PDB ldata order in CMBWorld.obj: IMBUnknown, IMBClassFactory, IMBObject,
// IMBObjectGroup, CMBObjectGroup, IMBWorld (0x1008b718..0x1008b768)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBObjectGroup =
    { 0x7ee135f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBWorld =
    { 0x7978c610, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }
template <class T> inline const T &MBmin(const T &a, const T &b) { return b < a ? b : a; }

class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    MBRect &operator=(const MBRect &r) {
        if (this != &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
        return *this;
    }
    long Width(void) const { return right - left; }
    MBSize GetSize(void) const { return MBSize(Width(), Height()); }
    void SetSize(const MBSize &s) { right = left + s.cx; bottom = top + s.cy; }
    long Height(void) const { return bottom - top; }
    unsigned char Intersect(const MBRect &r, MBRect &out) const {
        if (right >= r.left && left <= r.right && bottom >= r.top && top <= r.bottom) {
            out.left = MBmax(left, r.left);
            out.top = MBmax(top, r.top);
            out.right = MBmin(right, r.right);
            out.bottom = MBmin(bottom, r.bottom);
            return true;
        }
        return false;
    }
    void Offset(long dx, long dy) { left += dx; top += dy; right += dx; bottom += dy; }
    long left;
    long top;
    long right;
    long bottom;
};

struct MBPointStruct3 { long x; long y; long z; };
class MBPoint3 : public MBPointStruct3 {};

class IMBSurface;
class IMBObjectGroup;

class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

class IMBObject : public IMBUnknown {
public:
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

class IMBWorld : public IMBUnknown {
public:
    virtual MBRect GetBounds(void) const = 0;
    virtual void SetBounds(const MBRect &) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0;
    virtual IMBObjectGroup &GetRoot(void) = 0;
    virtual const IMBObjectGroup &GetConstRoot(void) const = 0;
};

extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

class CMBWorld : public IMBWorld {
public:
    CMBWorld(void);
    ~CMBWorld(void);
protected:
    unsigned long _mRefCnt;      // +0x04
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual MBRect GetBounds(void) const;
    virtual void SetBounds(const MBRect &bounds);
    virtual long Draw(IMBSurface *dest, MBRect &worldRect, MBRect &destRect);
    virtual IMBObjectGroup &GetRoot(void);
    virtual const IMBObjectGroup &GetConstRoot(void) const;
protected:
    MBRect mBounds;              // +0x08
    IMBObjectGroup *mRoot;       // +0x18
};

IMBUnknown *MakeCMBWorld(void);

#endif
