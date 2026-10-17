// MBPrintingWin.h - declarations for MBEngine.dll MBPrintingWin.obj (printing a surface
// on the default printer): the colour/surface header plus the screen interfaces
// (from CMBDirect3DScreen.h).
#ifndef MBPRINTINGWIN_H
#define MBPRINTINGWIN_H

#include "CMBDirectDrawSurface.h"

static const MBGUID MBIID_CMBEventManager = {0x79a51680, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};
static const MBGUID MBIID_IMBDirectXScreen = {0x7fe4fbd0, 0x9ea2, 0x11d3, {0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5}};

class MBPoint;

#include "MBScreenState.h"

#include "MBScreenAttributes.h"

static const MBGUID MBIID_IMBScreen = { 0x79d783b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBScreen.h"

#include "IMBWinScreen.h"

// own copy: a GetGUID-only view: this unit declares no MBDDEnumCallback or DirectDraw types
class IMBDirectXScreen : public IMBWinScreen {
public:
    static const MBGUID &GetGUID(void) { return MBIID_IMBDirectXScreen; }
};

static const MBGUID MBIID_IMBScreenManager = { 0x79df27a0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
#include "IMBScreenManager.h"

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
