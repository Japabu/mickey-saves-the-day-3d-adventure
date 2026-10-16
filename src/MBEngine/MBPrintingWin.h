// MBPrintingWin.h - declarations for MBEngine.dll MBPrintingWin.obj (printing a surface
// on the default printer): the colour/surface header plus the screen interfaces
// (from CMBDirect3DScreen.h).
#ifndef MBPRINTINGWIN_H
#define MBPRINTINGWIN_H

#include "CMBDirectDrawSurface.h"

static const MBGUID MBIID_CMBEventManager = {0x79a51680, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectXScreen = {0x7fe4fbd0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

class MBPoint;

enum MBScreenState {
    kScreenStateActive = 0,
    kScreenStatePaused = 1,
    kScreenStateInactive = 2
};

struct MBScreenAttributes {
    unsigned long width;
    unsigned long height;
    unsigned long depth;
};

class IMBScreen : public IMBUnknown {
public:
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
    virtual void SetHWND(HWND__ *) = 0;
    virtual HWND__ *GetHWND(void) = 0;
};

class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
};

class IMBScreenManager : public IMBUnknown {
public:
    virtual bool RequestScreen(long, MBScreenAttributes &) = 0;
    virtual void Activate(void) = 0;
    virtual void Deactivate(void) = 0;
    virtual MBPoint GetGlobalMouseLocation(void) = 0;
    virtual bool ProcessSystemEvent(void *) = 0;
    virtual void RemoveScreen(long) = 0;
    virtual void RemoveAllScreens(void) = 0;
    virtual unsigned short GetNumScreens(void) = 0;
    virtual void GetScreenByIndex(unsigned short, IMBScreen **) = 0;
};

extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);
extern "C" __declspec(dllimport) void __cdecl MBmemclr(void *p, unsigned long size);
extern "C" __declspec(dllimport) void *__cdecl MBmemcpy(void *dst, const void *src, unsigned int n);
extern "C" __declspec(dllimport) unsigned long __cdecl MBstrlen(const char *s);

enum MBPrintOrientation {
    kMBPrintPortrait = 0,
    kMBPrintLandscape = 1
};

extern "C" long MBPrintSurfaceOneCopy(IMBSurface *surface, MBPrintOrientation orientation);
extern "C" unsigned char __cdecl MBIsPrinterConnected(void);

#endif
