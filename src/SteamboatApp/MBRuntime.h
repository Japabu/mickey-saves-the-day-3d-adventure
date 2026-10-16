#pragma once
// Functions of the engine's runtime DLL (mbruntime.dll) that the game calls.
extern "C" __declspec(dllimport) unsigned long MBHashString(const char *);
extern "C" __declspec(dllimport) long MBrandom(long first, long last);
extern "C" __declspec(dllimport) void MBsrand(unsigned long seed);
extern "C" __declspec(dllimport) void MBDebugStr(const char *format, ...);
extern "C" __declspec(dllimport) char *MBstrrchr(const char *string, int c);

// The engine's screen and mouse helpers (called through the import thunks).
class IMBScreen;
class IMBScreenManager;
class MBPoint;
extern "C" void MBGetScreenManager(IMBScreenManager **screenManager);
extern "C" bool MBGetScreenMouseLocation(MBPoint &screenPoint, IMBScreen **screen);
class IMBWorld;
extern "C" bool MBGetWorldMouseLocation(MBPoint &worldPoint, IMBWorld **world);
class IMBFileSpec;
extern "C" bool MBGetCurrentDirectory(IMBFileSpec **path);
