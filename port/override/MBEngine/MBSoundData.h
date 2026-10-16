// port override: drops the PDB layout asserts (sizeof 8, mDataSize at +4), true only for 32-bit pointers
#pragma once
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
class MBSoundData {
    friend struct PDB_Layout;
public:
public:
    MBSoundData(const MBSoundData &soundData);
    MBSoundData(unsigned char *soundData, unsigned long dataSize);
    ~MBSoundData();
    unsigned char *GetSoundData() const;
    unsigned long GetDataSize() const;
protected:
    unsigned char *mSoundData;                           // +0x0000 size 0x4
    unsigned long mDataSize;                             // +0x0004 size 0x4
public:
    // void *__vecDelDtor(unsigned int);                 // compiler-generated
};

extern "C" __declspec(dllimport) void *__cdecl MBmalloc_release(unsigned long size);
extern "C" __declspec(dllimport) void __cdecl MBfree_release(void *ptr);
extern "C" __declspec(dllimport) void __cdecl MBmemcpy(void *dest, const void *src, unsigned long cnt);

