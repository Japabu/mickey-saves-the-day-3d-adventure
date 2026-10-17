#pragma once
// also declared, because a function of this unit asked for it: IMBGameClock
// also declared, because a function of this unit asked for it: void __thiscall CMBAnim::AdvanceFrame()
#include "CMBAnim.h"
#define OFS(t, m) ((unsigned int)&(((t *)0)->m))
struct PDB_Layout;
struct O_MBAllocator_MBAnimCast_91ba0658;
struct O_MBAllocator_MBAnimSequence_2fd5a426;
struct O_MBVector_MBAnimCast_392c07f3;
struct O_MBVector_MBAnimSequence_4b6b5653;
struct O_std_allocator_MBAnimCast_106d74fd;
struct O_std_allocator_MBAnimSequence_56d31141;
struct O_std_vector_MBAnimCast_MBAllocator_MBAnimCast_c23844b4;
struct O_std_vector_MBAnimSequence_MBAllocator_MBAnimSequence_318a4d6d;
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


struct O_std_allocator_MBAnimCast_106d74fd {};

struct O_MBAllocator_MBAnimCast_91ba0658 {};

struct O_std_vector_MBAnimCast_MBAllocator_MBAnimCast_c23844b4 { int _raw[4]; };

struct O_MBVector_MBAnimCast_392c07f3 { int _raw[4]; };

struct O_std_allocator_MBAnimSequence_56d31141 {};

struct O_MBAllocator_MBAnimSequence_2fd5a426 {};

struct O_std_vector_MBAnimSequence_MBAllocator_MBAnimSequence_318a4d6d { int _raw[4]; };

struct O_MBVector_MBAnimSequence_4b6b5653 { int _raw[4]; };


// sizes and member offsets recorded in the PDB
// own copy: this unit's own layout asserts, not a class of the game
struct PDB_Layout {
};
#undef OFS
