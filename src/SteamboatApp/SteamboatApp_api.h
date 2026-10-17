// SteamboatApp_api.h - SteamboatApp.dll's exports: STEAMBOATAPP_API exports a declaration when SteamboatApp.dll is
// compiled (STEAMBOATAPP_BUILD, src/options.txt) and imports it everywhere else.
#ifndef STEAMBOATAPP_API
#ifdef STEAMBOATAPP_BUILD
#define STEAMBOATAPP_API __declspec(dllexport)
#else
#define STEAMBOATAPP_API __declspec(dllimport)
#endif
#endif
