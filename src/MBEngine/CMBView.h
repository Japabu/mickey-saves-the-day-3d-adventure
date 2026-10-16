// CMBView.h - declarations for MBEngine.dll CMBView.obj (shared with CMBViewManager)
#ifndef CMBVIEW_H
#define CMBVIEW_H

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

struct MBPointStruct {
    long x;
    long y;
};

class MBPoint : public MBPointStruct {
public:
    MBPoint(void) { x = 0; y = 0; }
    MBPoint(long ax, long ay) { x = ax; y = ay; }
    MBPoint operator+(const MBPoint &p) const { return MBPoint(x + p.x, y + p.y); }
    MBPoint operator-(const MBPoint &p) const { return MBPoint(x - p.x, y - p.y); }
};

class MBSize {
public:
    MBSize(long x, long y) { cx = x; cy = y; }
    long cx;
    long cy;
};

class MBRect {
public:
    MBRect(void) { left = 0; top = 0; right = 0; bottom = 0; }
    MBRect(long l, long t, long r, long b) { left = l; top = t; right = r; bottom = b; }
    MBRect(const MBPoint &p, const MBSize &s) { left = p.x; top = p.y; right = p.x + s.cx; bottom = p.y + s.cy; }
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

class IMBSurface : public IMBUnknown {
};

enum MBScreenState { kMBScreenStateDummy };

class IMBScreen : public IMBUnknown {
public:
    virtual void SetScreenState(MBScreenState) = 0;
    virtual MBScreenState GetScreenState(void) = 0;
    virtual unsigned char CreateSurface(unsigned short, unsigned short, unsigned long, IMBSurface **) = 0;
    virtual void GetBackBuffer(IMBSurface **) = 0;
    virtual void Flip(void) = 0;
    virtual unsigned short GetWidth(void) = 0;
    virtual unsigned short GetHeight(void) = 0;
    virtual MBRect GetGlobalBounds(void) = 0;
    virtual MBPoint GlobalToScreenPoint(const MBPoint &) = 0;
};

class IMBObjectGroup;

class IMBWorld : public IMBUnknown {
public:
    virtual MBRect GetBounds(void) const = 0;
    virtual void SetBounds(const MBRect &) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0;
    virtual IMBObjectGroup &GetRoot(void) = 0;
    virtual const IMBObjectGroup &GetConstRoot(void) const = 0;
};

class IMBView : public IMBUnknown {
public:
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

class CMBView : public IMBView {
public:
    CMBView(void);
    ~CMBView(void);
protected:
    unsigned long _mRefCnt;      // +0x04
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual bool IsDirty(void);
    virtual void Draw(IMBSurface *dest);
    virtual void SetWorldPosition(MBPoint position);
    virtual MBPoint GetWorldPosition(void);
    virtual MBRect GetWorldBounds(void);
    virtual void SetScreenPosition(MBPoint position);
    virtual MBPoint GetScreenPosition(void);
    virtual MBRect GetScreenBounds(void);
    virtual void SetSize(unsigned short width, unsigned short height);
    virtual unsigned short GetWidth(void);
    virtual unsigned short GetHeight(void);
    virtual void AttachToWorld(IMBWorld *world);
    virtual void AttachToScreen(IMBScreen *screen);
    virtual bool IsAttachedToScreen(IMBScreen *screen);
    virtual void GetWorld(IMBWorld **world);
    virtual MBPoint ScreenToWorldPoint(const MBPoint &screenPoint);
protected:
    unsigned short mWidth;       // +0x08
    unsigned short mHeight;      // +0x0a
    MBPoint mWorldPosition;      // +0x0c
    MBPoint mScreenPosition;     // +0x14
    IMBWorld *mWorld;            // +0x1c
    IMBScreen *mScreen;          // +0x20
};

IMBUnknown *MakeCMBView(void);

#endif
