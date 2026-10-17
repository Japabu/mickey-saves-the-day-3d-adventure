#pragma once
#define OFS(t, m) ((unsigned int)&(((t *)0)->m))
struct PDB_Layout;
class MBSoundData;
class IMBAnim;
class IMBBitmapFont;
class IMBDirectXScreen;
class IMBEventManager;
class IMBFile;
class IMBFileSpec;
class IMBFileStreamLocator;
class IMBFont;
class IMBGameClock;
class IMBMemStream;
class IMBMemoryBlock;
class IMBMouseMonitor;
class IMBMouseable;
class IMBNode;
class IMBNodeManager;
class IMBObject;
class IMBPeriodical;
class IMBPeriodicalManager;
class IMBPref;
class IMBResourceFile;
class IMBResourceFileList;
class IMBResourceStreamLocator;
class IMBScreen;
class IMBScreenManager;
class IMBSoundDescription;
class IMBSoundManager;
class IMBStream;
class IMBStringTable;
class IMBSurface;
class IMBSurfaceObject;
class IMBSystemFont;
class IMBTextValidator;
class IMBView;
class IMBViewManager;
class IMBWinDC;
class IMBWinFile;
class IMBWorld;
struct MBGUID;
class IMBUnknown;
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
#include "../MBEngine/MBComPtr.h"

// MBSoundData: size 0x8
#include "MBSoundData.h"

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);
extern "C" __declspec(dllimport) void __cdecl MBmemcpy(void *dest, const void *src, unsigned long cnt);

// sizes and member offsets recorded in the PDB
// own copy: this unit's own layout asserts, not a class of the game
struct PDB_Layout {
    typedef char A0[(sizeof(MBSoundData)) == 0x8 ? 1 : -1];
    typedef char A1[(OFS(MBSoundData, mSoundData)) == 0x0 ? 1 : -1];
    typedef char A2[(OFS(MBSoundData, mDataSize)) == 0x4 ? 1 : -1];
};
#undef OFS
