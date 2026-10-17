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

#include "MBPointStruct.h"

#include "../MBEngine/MBPoint.h"

#include "MBSize.h"

#include "../MBEngine/MBRect.h"


class IMBUnknown {
public:
    virtual long QueryInterface(const MBGUID &, void **) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};

// own copy: this unit defines the interface IDs in CMBView.cpp, after its declarations (PDB ldata order), so the interface is declared here without GetGUID
class IMBSurface : public IMBUnknown {
};

#include "MBScreenState.h"

// own copy: this unit defines the interface IDs in CMBView.cpp, after its declarations (PDB ldata order), so the interface is declared here without GetGUID
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

// own copy: this unit defines the interface IDs in CMBView.cpp, after its declarations (PDB ldata order), so the interface is declared here without GetGUID
class IMBWorld : public IMBUnknown {
public:
    virtual MBRect GetBounds(void) const = 0;
    virtual void SetBounds(const MBRect &) = 0;
    virtual long Draw(IMBSurface *, MBRect &, MBRect &) = 0;
    virtual IMBObjectGroup &GetRoot(void) = 0;
    virtual const IMBObjectGroup &GetConstRoot(void) const = 0;
};

// own copy: this unit defines the interface IDs in CMBView.cpp, after its declarations (PDB ldata order), so the interface is declared here without GetGUID
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
