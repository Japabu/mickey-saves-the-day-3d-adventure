// CMBDirectDrawSurface.h - declarations for MBEngine.dll CMBDirectDrawSurface.obj
// Copied from CMBDirect3DScreen.h / CMBView.h (colour header) with the full
// IDirectDrawSurface7 vtable (Lock/Unlock/SetColorKey/...) and the default arguments
// the PDB shows on MBColorMask/MBColor (__dflt_ctor_closure).
#ifndef CMBDIRECTDRAWSURFACE_H
#define CMBDIRECTDRAWSURFACE_H

#include <windows.h>
#include <ddraw.h>

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned int size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *p);

inline void *__cdecl operator new(unsigned int size) { return MBmalloc_release(size); }
inline void __cdecl operator delete(void *p) { MBfree_release(p); }

// ---- DirectDraw 7 (not in the DirectX 5 headers) ----
typedef struct _DDSCAPS2 {
    DWORD dwCaps;
    DWORD dwCaps2;
    DWORD dwCaps3;
    DWORD dwCaps4;
} DDSCAPS2;

typedef struct _DDSURFACEDESC2 {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    LONG lPitch;
    DWORD dwBackBufferCount;
    DWORD dwRefreshRate;
    DWORD dwAlphaBitDepth;
    DWORD dwReserved;
    LPVOID lpSurface;
    DDCOLORKEY ddckCKDestOverlay;
    DDCOLORKEY ddckCKDestBlt;
    DDCOLORKEY ddckCKSrcOverlay;
    DDCOLORKEY ddckCKSrcBlt;
    DDPIXELFORMAT ddpfPixelFormat;
    DDSCAPS2 ddsCaps;
    DWORD dwTextureStage;
} DDSURFACEDESC2;

#include "IDirectDrawSurface7.h"

// ---- MB core ----
#include "../MBCom/MBGUID.h"

// PDB ldata order 0x1008f9e8..0x1008fa68
#include "../MBCom/IMBUnknown.h"
static const MBGUID MBIID_IMBClassFactory = {0x396c7e0b, 0x172d, 0x4182, {0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea}};
static const MBGUID MBIID_IMBSurface = {0x7ee752b0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectDrawSurface = {0x7f05e270, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBMemorySurface = {0x7f30abb0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBWinDIBSurface = {0x7fb10770, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBWinDC = {0x7fb72430, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventManager = {0x79913910, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBEventListener = {0x799755d0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};


extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);

#include "../MBEngine/MBRect.h"

// ---- colors (header statics: every including TU has its own $E initializers) ----
#include "MBColorMask.h"

const MBColorMask kCM_RGBA32(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
const MBColorMask kCM_ARGB32(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
const MBColorMask kCM_ARGB16(0x7c00, 0x03e0, 0x001f, 0x8000);
const MBColorMask kCM_RGB16(0xf800, 0x07e0, 0x001f, 0);
const MBColorMask kCM_BGR16(0x001f, 0x07e0, 0xf800, 0);

// own copy: this unit uses the constructor, Set or Convert, whose inline body here differs from MBColor.h (store order / code shape the matches depend on)
class MBColor {
public:
    MBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0) { mColor = 0; Set(r, g, b, a); }
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

#include "MBPixelFormat.h"

class MBPalette;

// ---- interfaces ----
#include "IMBSurface.h"

#include "IMBDirectDrawSurface.h"

#include "IMBMemorySurface.h"

#include "IMBWinDIBSurface.h"

#include "IMBWinDC.h"

#include "IMBEventListener.h"

#include "IMBEventManager.h"

// own copy: which members are inline and which are only declared differs per unit (the original's inline budget; NOTES.md)
template <class T>
class MBComPtr {
public:
    MBComPtr(void) : mInterface(0) {}
    MBComPtr(const MBGUID &clsid) : mInterface(0) { CreateObject(clsid); }
    template <class U> MBComPtr(U *const &p) : mInterface(0) { _QueryInterface(p); }
    ~MBComPtr(void) { Release(); }
    operator T *(void) const { return mInterface; }
    T *operator->(void) const { return mInterface; }
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
    // A template: MBPrintingWin.obj calls the MBComPtr<IMBDirectXScreen> one with an IMBScreen *.
    template <class U> long _QueryInterface(U *const &p) {
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
        Attach(0);
        return hr;
    }
    T *mInterface;
};

bool MBBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest, MBRect *dest_rect,
                     unsigned long dest_pitch, unsigned long depth);
bool MBBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest, MBRect *dest_rect,
                          unsigned long dest_pitch, unsigned long depth, unsigned long trans_value);
bool MBStretchBlit(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest, MBRect *dest_rect,
                            unsigned long dest_pitch, unsigned long depth);
bool MBStretchBlitTrans(void *source, MBRect *source_rect, unsigned long source_pitch, void *dest,
                                 MBRect *dest_rect, unsigned long dest_pitch, unsigned long depth,
                                 unsigned long trans_value);

class CMBDirectDrawSurface : public IMBDirectDrawSurface, private IMBWinDC, public IMBEventListener {
public:
    CMBDirectDrawSurface(void);
    ~CMBDirectDrawSurface(void);
protected:
    unsigned long _mRefCnt;
public:
    virtual long QueryInterface(const MBGUID &iid, void **obj);
    virtual unsigned long AddRef(void);
    virtual unsigned long Release(void);
    virtual unsigned long GetWidth(void);
    virtual unsigned long GetHeight(void);
    virtual unsigned long GetPitch(void);
    virtual unsigned long GetSize(void);
    virtual unsigned long GetDepth(void);
    virtual MBPixelFormat GetFormat(void);
    virtual long Lock(void **bits, unsigned long flags);
    virtual long Unlock(void **bits);
    virtual long GetDC(HDC__ **hdcPtr, unsigned long lockFlag);
    virtual long ReleaseDC(HDC__ *hdc);
    virtual MBColor GetPixelColor(unsigned long x, unsigned long y);
    virtual long SetTransparentColor(MBColor pd);
    virtual MBColor GetTransparentColor(void);
    virtual long SetPalette(MBPalette *pal);
    virtual MBPalette *GetPalette(void);
    virtual long ColorFill(MBColor color, MBRect *dest_rect);
    virtual long Blit(IMBSurface *dest, MBRect *source_rect, MBRect *dest_rect, unsigned long blit_flags);
    virtual void SetSurface(IDirectDrawSurface7 *surface);
    virtual void GetSurface(IDirectDrawSurface7 **surface);
    virtual void SetSurfaceCopy(IDirectDrawSurface7 *surface);
    virtual void GetSurfaceCopy(IDirectDrawSurface7 **surface);
    virtual void QuerySurfaceInfo(void);
    virtual void ListenToEvent(long channel, long event, void *data);
protected:
    void *mBitmapData;                 // +0x10
    IDirectDrawSurface7 *mSurface;     // +0x14
    IDirectDrawSurface7 *mSurfaceCopy; // +0x18
    unsigned char mLockedWriteable;    // +0x1c
    unsigned long mWidth;              // +0x20
    unsigned long mHeight;             // +0x24
    unsigned long mPitch;              // +0x28
    MBPixelFormat mPixelFormat;        // +0x2c
    long mLockCount;                   // +0x44
    MBColor mTransparentColor;         // +0x48
};

IMBUnknown *MakeCMBDirectDrawSurface(void);

#endif
