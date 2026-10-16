#pragma once
// dllmain.h: SteamboatApp.dll's MBCom server entry points and the callbacks the engine calls at
// start-up, shut-down and screen creation (declarations from SteamboatApp.pdb).
#include "SBInterfaces.h"

struct _GUID;
struct HMONITOR__;

extern ISBNodeManager *steamboatNodeMgr;

extern "C" unsigned char __cdecl MBComRegisterServer(void);
extern "C" unsigned char __cdecl MBComUnregisterServer(void);
extern "C" int __stdcall DDDeviceSelectionCallback(_GUID *lpGUID, char *lpDriverDescription, char *lpDriverName,
                                                   void *lpContext, HMONITOR__ *hm);
extern "C" void __cdecl OnMBEngineStartup(IMBGameEngine *engine);
extern "C" void __cdecl OnMBEngineShutdown(IMBGameEngine *engine);
extern "C" long __cdecl OnMBEngineCreateScreen(IMBGameEngine *engine, IMBScreenManager *screenManager);
