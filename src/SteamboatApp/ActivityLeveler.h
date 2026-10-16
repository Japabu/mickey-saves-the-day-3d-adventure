#pragma once
#define OFS(t, m) ((unsigned int)&(((t *)0)->m))
struct PDB_Layout;
class CSBRenderManager;
class FlipWatcher;
class IMBAnim;
class IMBDirectDrawSurface;
class IMBDirectXScreen;
class IMBEventListener;
class IMBEventManager;
class IMBFile;
class IMBFileSpec;
class IMBFont;
class IMBGameClock;
class IMBGameEngine;
class IMBMouseMonitor;
class IMBMouseable;
class IMBMovie;
class IMBNode;
class IMBNodeManager;
class IMBObject;
class IMBObjectGroup;
class IMBPeriodical;
class IMBPeriodicalManager;
class IMBPref;
class IMBResource;
class IMBResourceFile;
class IMBResourceFileList;
class IMBScreen;
class IMBScreenManager;
class IMBScreenPoly;
class IMBSoundDescription;
class IMBSoundManager;
class IMBStream;
class IMBStringTable;
class IMBTextControl;
class IMBUnknown;
class IMBView;
class IMBViewManager;
class IMBWinScreen;
class IMBWorld;
class ISBAnimButton;
class ISBClickable;
class ISBGameState;
class ISBInventory;
class ISBNodeManager;
class ISBSongbird;
class ISBSongbirdNote;
class ISBWorld;
struct MBGUID;
class IMBUnknown;
extern "C" long MBComCreateInstance(IMBUnknown *pOuter, const MBGUID &guid, IMBUnknown **unknown);
#include "../MBEngine/MBComPtr.h"

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);

// sizes and member offsets recorded in the PDB
struct PDB_Layout {
};
#undef OFS
