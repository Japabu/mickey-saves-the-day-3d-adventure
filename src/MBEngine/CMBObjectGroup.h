// CMBObjectGroup.h - declarations for MBEngine.dll CMBObjectGroup.obj
#ifndef CMBOBJECTGROUP_H
#define CMBOBJECTGROUP_H

#include <string> // evidence: original TU has $E20/$E21 for std::ctype<unsigned short>::id
#include <map>    // one MBMultimap (_Nil/_Nilrefs) moves the <string> baseline $E18/$E19 up by 2

extern "C" __declspec(dllimport) void *MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void MBfree_release(void *p);

inline void *operator new(unsigned int size) { return MBmalloc_release(size); }
inline void operator delete(void *p) { MBfree_release(p); }

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
    bool operator==(const MBGUID &o) const {
        return data1 == o.data1 && data2 == o.data2 && data3 == o.data3 &&
               ((const unsigned long *)data4)[0] == ((const unsigned long *)o.data4)[0] &&
               ((const unsigned long *)data4)[1] == ((const unsigned long *)o.data4)[1];
    }
};

// PDB ldata order in CMBObjectGroup.obj: IMBUnknown, IMBClassFactory, IMBObject,
// IMBObjectGroup (0x1008b218..0x1008b258)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBObject =
    { 0x7ed4fc70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBObjectGroup =
    { 0x7ed99200, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

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

template <class K, class V, class Pr> class MBMultimap : public std::multimap<K, V, Pr, MBAllocator<V> > {
};

template <class T> inline const T &MBmax(const T &a, const T &b) { return a < b ? b : a; }
template <class T> inline const T &MBmin(const T &a, const T &b) { return b < a ? b : a; }

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
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
    MBPoint3 &operator+=(const MBPoint3 &p) { x += p.x; y += p.y; z += p.z; return *this; }
};

// No user operator=: Draw and ClipRectsToObject copy rects without a self check.
class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBRect &r) { left = r.left; top = r.top; right = r.right; bottom = r.bottom; }
    long Width(void) const { return right - left; }
    long Height(void) const { return bottom - top; }
    MBSize GetSize(void) const { return MBSize(Width(), Height()); }
    void SetSize(const MBSize &s) { right = left + s.cx; bottom = top + s.cy; }
    bool Intersect(const MBRect &r, MBRect &out) const {
        if (right >= r.left && left <= r.right && bottom >= r.top && top <= r.bottom) {
            out.left = MBmax(left, r.left);
            out.top = MBmax(top, r.top);
            out.right = MBmin(right, r.right);
            out.bottom = MBmin(bottom, r.bottom);
            return true;
        }
        return false;
    }
    void Union(const MBRect &r) {
        left = MBmin(left, r.left);
        top = MBmin(top, r.top);
        right = MBmax(left + Width(), r.left + r.Width());
        bottom = MBmax(top + Height(), r.top + r.Height());
    }
    void Offset(long dx, long dy) { left += dx; top += dy; right += dx; bottom += dy; }
    long left;
    long top;
    long right;
    long bottom;
};

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

typedef MBMultimap<long, IMBObject *, std::less<long> > MBObjectMap;

class CMBObjectGroup : public IMBObjectGroup {
public:
    CMBObjectGroup(void);
    ~CMBObjectGroup(void);
protected:
    unsigned long _mRefCnt;                 // +0x04
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
private:
    bool ClipRectsToObject(IMBObject *obj, MBRect &srcRect, MBRect &destRect) const;
    MBObjectMap::iterator FindObject(IMBObject *obj);
    IMBObjectGroup *mParent;                // +0x08
    IMBObject *mBackground;                 // +0x0c
    mutable MBRect mCachedBounds;           // +0x10
    mutable bool mCachedBoundsValid;        // +0x20
    MBPoint3 mOffset;                       // +0x24
    long mHideCount;                        // +0x30
    bool mDirty;                            // +0x34
    MBObjectMap mObjects;                   // +0x38
};

IMBUnknown *MakeCMBObjectGroup(void);

#endif
