// dllmain.cpp - MBEngine.dll's MBCom server entry points: register and unregister
// the class factory of every engine component.
#include <windows.h>
#include <string> // evidence: ctype $E21/$E20 = <windows.h> + <string> baseline

struct MBGUID {
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];
};

// Stand-ins (see NOTES): the PDB numbering (ctype $E21/$E20, guard $S22) proves two
// more numbered items than this source has (likely the string instantiations of the
// component headers the original includes); declared only, never defined.
struct MBDllMainStandIn {
    static int sUnknownDllMainNumbering1;
    static int sUnknownDllMainNumbering2;
};

class IMBUnknown;
typedef IMBUnknown *(__cdecl *MBClassFactoryFunc)(void);

// MBCom.dll exports, called through the import thunks
extern "C" long __cdecl MBComRegisterClassObject(const MBGUID &guid, MBClassFactoryFunc func);
extern "C" long __cdecl MBComUnregisterClassObject(const MBGUID &guid, MBClassFactoryFunc func);

// interface and class IDs, in the order of the original headers (PDB ldata 0x10090180..)
static const MBGUID MBIID_IMBUnknown =
    { 0x59d231f0, 0x9c6c, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBClassFactory =
    { 0x396c7e0b, 0x172d, 0x4182, { 0xa9, 0x09, 0x48, 0xb5, 0x02, 0x3c, 0x7d, 0xea } };
static const MBGUID MBIID_IMBGameClock =
    { 0x04e3c980, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinClock =
    { 0x04e9e640, 0x9ea1, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemoryBlock =
    { 0x79496740, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPeriodicalManager =
    { 0x7958af20, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBView =
    { 0x796b0560, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWorld =
    { 0x797ee2d0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBNodeManager =
    { 0x798b1c50, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBEventManager =
    { 0x79913910, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBEventManager =
    { 0x79a51680, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBViewManager =
    { 0x79a9ac10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBViewManager =
    { 0x79b15000, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBPeriodicalManager =
    { 0x79bf10b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBMouseMonitor =
    { 0x79c52d70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMouseMonitor =
    { 0x79cb4a30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBSoundManager =
    { 0x79ee6f80, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMilesSoundManager =
    { 0x79f48c40, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBPref =
    { 0x79fc3030, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBPref =
    { 0x7a00c5c0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBDebugConsole =
    { 0x7a100da0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinDebugConsole =
    { 0x7a162a60, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBGameEngine =
    { 0x7a1c4720, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBGameEngine =
    { 0x7a2263e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBFileSpec =
    { 0x897e2150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinFileSpec =
    { 0x89843e10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceMgr =
    { 0x8991fec0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceTree =
    { 0x89a45500, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceZipfile =
    { 0x89fe7d10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinFile =
    { 0x89b39ce0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemStream =
    { 0x89c5f320, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBResourceStreamLocator =
    { 0x89d84960, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFileStreamLocator =
    { 0x89e482e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBChooseSaveFile =
    { 0x89f86050, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBObjectGroup =
    { 0x7ee135f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBAnim =
    { 0x7ecbd150, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBMemorySurface =
    { 0x7eed6f70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinDIBSurface =
    { 0x7ef38c30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBDirectDrawSurface =
    { 0x7ef9a8f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBDirectDrawScreen =
    { 0x7effc5b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBAnimButton =
    { 0x7f0bff30, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBSurfaceObject =
    { 0x7f1838b0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFont =
    { 0x7f4aa5e0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBTextControl =
    { 0x7f555830, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBSmackerMovie =
    { 0x7f64a010, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBDragObject =
    { 0x7f8c5af0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBDirectDrawScreenManager =
    { 0x7fa4cdf0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinDraw =
    { 0x7fc35db0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_IMBColorBackgroundObject =
    { 0x7fc97a70, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBColorBackgroundObject =
    { 0x7fcf9730, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBBinkMovie =
    { 0x7fd5b3f0, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBDirect3DScreen =
    { 0x7fdedf10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBUniFont =
    { 0x7ff13550, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBWinSystemFont =
    { 0x80020460, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBScript =
    { 0x8f021290, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBFPSMonitor =
    { 0x8f0e4c10, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBStringTable =
    { 0x8f1a8590, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };
static const MBGUID MBIID_CMBSoundDescription =
    { 0x844fc290, 0x9ea2, 0x11d3, { 0xbe, 0xdb, 0x00, 0xa0, 0xc9, 0xcc, 0x4c, 0xc5 } };

// the factories, one per component unit
IMBUnknown *MakeCMBEventManager(void);
IMBUnknown *MakeCMBFileStreamLocator(void);
IMBUnknown *MakeCMBWinClock(void);
IMBUnknown *MakeCMBWinDebugConsole(void);
IMBUnknown *MakeCMBNodeManager(void);
IMBUnknown *MakeCMBPeriodicalManager(void);
IMBUnknown *MakeCMBPref(void);
IMBUnknown *MakeCMBMilesSoundManager(void);
IMBUnknown *MakeCMBViewManager(void);
IMBUnknown *MakeCMBResourceMgr(void);
IMBUnknown *MakeCMBAnim(void);
IMBUnknown *MakeCMBStringTable(void);
IMBUnknown *MakeCMBMemStream(void);
IMBUnknown *MakeCMBWinFile(void);
IMBUnknown *MakeCMBWinFileSpec(void);
IMBUnknown *MakeCMBMemoryBlock(void);
IMBUnknown *MakeCMBMouseMonitor(void);
IMBUnknown *MakeCMBDirectDrawScreen(void);
IMBUnknown *MakeCMBDirectDrawScreenManager(void);
IMBUnknown *MakeCMBView(void);
IMBUnknown *MakeCMBAnimButton(void);
IMBUnknown *MakeCMBDragObject(void);
IMBUnknown *MakeCMBFont(void);
IMBUnknown *MakeCMBUniFont(void);
IMBUnknown *MakeCMBObjectGroup(void);
IMBUnknown *MakeCMBSurfaceObject(void);
IMBUnknown *MakeCMBTextControl(void);
IMBUnknown *MakeCMBWorld(void);
IMBUnknown *MakeCMBSmackerMovie(void);
IMBUnknown *MakeCMBGameEngine(void);
IMBUnknown *MakeCMBResourceStreamLocator(void);
IMBUnknown *MakeCMBSoundDescription(void);
IMBUnknown *MakeCMBChooseSaveFile(void);
IMBUnknown *MakeCMBColorBackgroundObject(void);
IMBUnknown *MakeCMBScript(void);
IMBUnknown *MakeCMBBinkMovie(void);
IMBUnknown *MakeCMBDirect3DScreen(void);
IMBUnknown *MakeCMBFPSMonitor(void);
IMBUnknown *MakeCMBWinSystemFont(void);
IMBUnknown *MakeCMBResourceTree(void);
IMBUnknown *MakeCMBResourceZipfile(void);
IMBUnknown *MakeCMBDirectDrawSurface(void);
IMBUnknown *MakeCMBWinDIBSurface(void);
IMBUnknown *MakeCMBMemorySurface(void);
IMBUnknown *MakeCMBWinDraw(void);

// 0x100736b0 ?MBComRegisterServer@@YA_NXZ
bool MBComRegisterServer(void)
{
    MBComRegisterClassObject(MBIID_CMBEventManager, MakeCMBEventManager);
    MBComRegisterClassObject(MBIID_CMBFileStreamLocator, MakeCMBFileStreamLocator);
    MBComRegisterClassObject(MBIID_CMBWinClock, MakeCMBWinClock);
    MBComRegisterClassObject(MBIID_CMBWinDebugConsole, MakeCMBWinDebugConsole);
    MBComRegisterClassObject(MBIID_CMBNodeManager, MakeCMBNodeManager);
    MBComRegisterClassObject(MBIID_CMBPeriodicalManager, MakeCMBPeriodicalManager);
    MBComRegisterClassObject(MBIID_CMBPref, MakeCMBPref);
    MBComRegisterClassObject(MBIID_CMBMilesSoundManager, MakeCMBMilesSoundManager);
    MBComRegisterClassObject(MBIID_CMBViewManager, MakeCMBViewManager);
    MBComRegisterClassObject(MBIID_CMBResourceMgr, MakeCMBResourceMgr);
    MBComRegisterClassObject(MBIID_CMBAnim, MakeCMBAnim);
    MBComRegisterClassObject(MBIID_CMBStringTable, MakeCMBStringTable);
    MBComRegisterClassObject(MBIID_CMBMemStream, MakeCMBMemStream);
    MBComRegisterClassObject(MBIID_CMBWinFile, MakeCMBWinFile);
    MBComRegisterClassObject(MBIID_CMBWinFileSpec, MakeCMBWinFileSpec);
    MBComRegisterClassObject(MBIID_CMBMemoryBlock, MakeCMBMemoryBlock);
    MBComRegisterClassObject(MBIID_CMBMouseMonitor, MakeCMBMouseMonitor);
    MBComRegisterClassObject(MBIID_CMBDirectDrawScreen, MakeCMBDirectDrawScreen);
    MBComRegisterClassObject(MBIID_CMBDirectDrawScreenManager, MakeCMBDirectDrawScreenManager);
    MBComRegisterClassObject(MBIID_CMBView, MakeCMBView);
    MBComRegisterClassObject(MBIID_CMBAnimButton, MakeCMBAnimButton);
    MBComRegisterClassObject(MBIID_CMBDragObject, MakeCMBDragObject);
    MBComRegisterClassObject(MBIID_CMBFont, MakeCMBFont);
    MBComRegisterClassObject(MBIID_CMBUniFont, MakeCMBUniFont);
    MBComRegisterClassObject(MBIID_CMBObjectGroup, MakeCMBObjectGroup);
    MBComRegisterClassObject(MBIID_CMBSurfaceObject, MakeCMBSurfaceObject);
    MBComRegisterClassObject(MBIID_CMBTextControl, MakeCMBTextControl);
    MBComRegisterClassObject(MBIID_CMBWorld, MakeCMBWorld);
    MBComRegisterClassObject(MBIID_CMBSmackerMovie, MakeCMBSmackerMovie);
    MBComRegisterClassObject(MBIID_CMBGameEngine, MakeCMBGameEngine);
    MBComRegisterClassObject(MBIID_CMBResourceStreamLocator, MakeCMBResourceStreamLocator);
    MBComRegisterClassObject(MBIID_CMBSoundDescription, MakeCMBSoundDescription);
    MBComRegisterClassObject(MBIID_CMBChooseSaveFile, MakeCMBChooseSaveFile);
    MBComRegisterClassObject(MBIID_CMBColorBackgroundObject, MakeCMBColorBackgroundObject);
    MBComRegisterClassObject(MBIID_CMBScript, MakeCMBScript);
    MBComRegisterClassObject(MBIID_CMBBinkMovie, MakeCMBBinkMovie);
    MBComRegisterClassObject(MBIID_CMBDirect3DScreen, MakeCMBDirect3DScreen);
    MBComRegisterClassObject(MBIID_CMBFPSMonitor, MakeCMBFPSMonitor);
    MBComRegisterClassObject(MBIID_CMBWinSystemFont, MakeCMBWinSystemFont);
    MBComRegisterClassObject(MBIID_CMBResourceTree, MakeCMBResourceTree);
    MBComRegisterClassObject(MBIID_CMBResourceZipfile, MakeCMBResourceZipfile);
    MBComRegisterClassObject(MBIID_CMBDirectDrawSurface, MakeCMBDirectDrawSurface);
    MBComRegisterClassObject(MBIID_CMBWinDIBSurface, MakeCMBWinDIBSurface);
    MBComRegisterClassObject(MBIID_CMBMemorySurface, MakeCMBMemorySurface);
    MBComRegisterClassObject(MBIID_CMBWinDraw, MakeCMBWinDraw);
    MBComRegisterClassObject(MBIID_IMBEventManager, MakeCMBEventManager);
    MBComRegisterClassObject(MBIID_IMBGameClock, MakeCMBWinClock);
    MBComRegisterClassObject(MBIID_IMBDebugConsole, MakeCMBWinDebugConsole);
    MBComRegisterClassObject(MBIID_IMBFileSpec, MakeCMBWinFileSpec);
    MBComRegisterClassObject(MBIID_IMBMouseMonitor, MakeCMBMouseMonitor);
    MBComRegisterClassObject(MBIID_IMBPeriodicalManager, MakeCMBPeriodicalManager);
    MBComRegisterClassObject(MBIID_IMBPref, MakeCMBPref);
    MBComRegisterClassObject(MBIID_IMBSoundManager, MakeCMBMilesSoundManager);
    MBComRegisterClassObject(MBIID_IMBViewManager, MakeCMBViewManager);
    MBComRegisterClassObject(MBIID_IMBGameEngine, MakeCMBGameEngine);
    MBComRegisterClassObject(MBIID_IMBColorBackgroundObject, MakeCMBColorBackgroundObject);
    return true;
}

// 0x10073a10 ?MBComUnregisterServer@@YA_NXZ
// (CMBWinSystemFont, registered above, is not unregistered)
bool MBComUnregisterServer(void)
{
    MBComUnregisterClassObject(MBIID_CMBEventManager, MakeCMBEventManager);
    MBComUnregisterClassObject(MBIID_CMBFileStreamLocator, MakeCMBFileStreamLocator);
    MBComUnregisterClassObject(MBIID_CMBWinClock, MakeCMBWinClock);
    MBComUnregisterClassObject(MBIID_CMBWinDebugConsole, MakeCMBWinDebugConsole);
    MBComUnregisterClassObject(MBIID_CMBNodeManager, MakeCMBNodeManager);
    MBComUnregisterClassObject(MBIID_CMBPeriodicalManager, MakeCMBPeriodicalManager);
    MBComUnregisterClassObject(MBIID_CMBPref, MakeCMBPref);
    MBComUnregisterClassObject(MBIID_CMBMilesSoundManager, MakeCMBMilesSoundManager);
    MBComUnregisterClassObject(MBIID_CMBViewManager, MakeCMBViewManager);
    MBComUnregisterClassObject(MBIID_CMBResourceMgr, MakeCMBResourceMgr);
    MBComUnregisterClassObject(MBIID_CMBAnim, MakeCMBAnim);
    MBComUnregisterClassObject(MBIID_CMBStringTable, MakeCMBStringTable);
    MBComUnregisterClassObject(MBIID_CMBMemStream, MakeCMBMemStream);
    MBComUnregisterClassObject(MBIID_CMBWinFile, MakeCMBWinFile);
    MBComUnregisterClassObject(MBIID_CMBWinFileSpec, MakeCMBWinFileSpec);
    MBComUnregisterClassObject(MBIID_CMBMemoryBlock, MakeCMBMemoryBlock);
    MBComUnregisterClassObject(MBIID_CMBMouseMonitor, MakeCMBMouseMonitor);
    MBComUnregisterClassObject(MBIID_CMBDirectDrawScreen, MakeCMBDirectDrawScreen);
    MBComUnregisterClassObject(MBIID_CMBDirectDrawScreenManager, MakeCMBDirectDrawScreenManager);
    MBComUnregisterClassObject(MBIID_CMBView, MakeCMBView);
    MBComUnregisterClassObject(MBIID_CMBAnimButton, MakeCMBAnimButton);
    MBComUnregisterClassObject(MBIID_CMBDragObject, MakeCMBDragObject);
    MBComUnregisterClassObject(MBIID_CMBFont, MakeCMBFont);
    MBComUnregisterClassObject(MBIID_CMBUniFont, MakeCMBUniFont);
    MBComUnregisterClassObject(MBIID_CMBObjectGroup, MakeCMBObjectGroup);
    MBComUnregisterClassObject(MBIID_CMBSurfaceObject, MakeCMBSurfaceObject);
    MBComUnregisterClassObject(MBIID_CMBTextControl, MakeCMBTextControl);
    MBComUnregisterClassObject(MBIID_CMBWorld, MakeCMBWorld);
    MBComUnregisterClassObject(MBIID_CMBSmackerMovie, MakeCMBSmackerMovie);
    MBComUnregisterClassObject(MBIID_CMBGameEngine, MakeCMBGameEngine);
    MBComUnregisterClassObject(MBIID_CMBResourceStreamLocator, MakeCMBResourceStreamLocator);
    MBComUnregisterClassObject(MBIID_CMBSoundDescription, MakeCMBSoundDescription);
    MBComUnregisterClassObject(MBIID_CMBChooseSaveFile, MakeCMBChooseSaveFile);
    MBComUnregisterClassObject(MBIID_CMBColorBackgroundObject, MakeCMBColorBackgroundObject);
    MBComUnregisterClassObject(MBIID_CMBScript, MakeCMBScript);
    MBComUnregisterClassObject(MBIID_CMBBinkMovie, MakeCMBBinkMovie);
    MBComUnregisterClassObject(MBIID_CMBDirect3DScreen, MakeCMBDirect3DScreen);
    MBComUnregisterClassObject(MBIID_CMBFPSMonitor, MakeCMBFPSMonitor);
    MBComUnregisterClassObject(MBIID_CMBResourceTree, MakeCMBResourceTree);
    MBComUnregisterClassObject(MBIID_CMBResourceZipfile, MakeCMBResourceZipfile);
    MBComUnregisterClassObject(MBIID_CMBDirectDrawSurface, MakeCMBDirectDrawSurface);
    MBComUnregisterClassObject(MBIID_CMBWinDIBSurface, MakeCMBWinDIBSurface);
    MBComUnregisterClassObject(MBIID_CMBMemorySurface, MakeCMBMemorySurface);
    MBComUnregisterClassObject(MBIID_CMBWinDraw, MakeCMBWinDraw);
    MBComUnregisterClassObject(MBIID_IMBEventManager, MakeCMBEventManager);
    MBComUnregisterClassObject(MBIID_IMBGameClock, MakeCMBWinClock);
    MBComUnregisterClassObject(MBIID_IMBDebugConsole, MakeCMBWinDebugConsole);
    MBComUnregisterClassObject(MBIID_IMBFileSpec, MakeCMBWinFileSpec);
    MBComUnregisterClassObject(MBIID_IMBMouseMonitor, MakeCMBMouseMonitor);
    MBComUnregisterClassObject(MBIID_IMBPeriodicalManager, MakeCMBPeriodicalManager);
    MBComUnregisterClassObject(MBIID_IMBPref, MakeCMBPref);
    MBComUnregisterClassObject(MBIID_IMBSoundManager, MakeCMBMilesSoundManager);
    MBComUnregisterClassObject(MBIID_IMBViewManager, MakeCMBViewManager);
    MBComUnregisterClassObject(MBIID_IMBGameEngine, MakeCMBGameEngine);
    MBComUnregisterClassObject(MBIID_IMBColorBackgroundObject, MakeCMBColorBackgroundObject);
    return true;
}


// Matched by this code:
// 0x10073d70 _$E21
// 0x10073d90 _$E20
