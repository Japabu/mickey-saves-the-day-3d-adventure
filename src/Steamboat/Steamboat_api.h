// Steamboat_api.h - Steamboat.exe's exports: STEAMBOAT_API exports a declaration when Steamboat.exe is
// compiled (STEAMBOAT_BUILD, src/options.txt) and imports it everywhere else.
#ifndef STEAMBOAT_API
#ifdef STEAMBOAT_BUILD
#define STEAMBOAT_API __declspec(dllexport)
#else
#define STEAMBOAT_API __declspec(dllimport)
#endif
#endif
