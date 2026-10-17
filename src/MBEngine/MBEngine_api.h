// MBEngine_api.h - MBEngine.dll's exports: MBENGINE_API exports a declaration when MBEngine.dll is
// compiled (MBENGINE_BUILD, src/options.txt) and imports it everywhere else.
#ifndef MBENGINE_API
#ifdef MBENGINE_BUILD
#define MBENGINE_API __declspec(dllexport)
#else
#define MBENGINE_API __declspec(dllimport)
#endif
#endif
