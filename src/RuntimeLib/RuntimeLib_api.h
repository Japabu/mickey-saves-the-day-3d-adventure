// RuntimeLib_api.h - RuntimeLib.dll's exports: RUNTIME_API exports a declaration when RuntimeLib.dll is
// compiled (RUNTIME_BUILD, src/options.txt) and imports it everywhere else.
#ifndef RUNTIME_API
#ifdef RUNTIME_BUILD
#define RUNTIME_API __declspec(dllexport)
#else
#define RUNTIME_API __declspec(dllimport)
#endif
#endif
